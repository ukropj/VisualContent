#include "Model/FRAlgorithm.h"
#include "Model/Edge.h"
#include "Model/Node.h"
#include "Model/Graph.h"
#include "Window/CoreWindow.h"
#include "Util/Config.h"
#include <iostream>

using namespace Model;

typedef QMap<qlonglong, osg::ref_ptr<Node> >::const_iterator NodeIt;
typedef QMap<qlonglong, Edge* >::const_iterator EdgeIt;

//Konstruktor pre vlakno s algoritmom
FRAlgorithm::FRAlgorithm() {
	PI = acos((double) -1);
	ALPHA = Util::Config::getValue("Layout.Algorithm.Alpha").toFloat();
	MIN_MOVEMENT
			= Util::Config::getValue("Layout.Algorithm.MinMovement").toFloat();
	MAX_MOVEMENT
			= Util::Config::getValue("Layout.Algorithm.MaxMovement").toFloat();
	MAX_DISTANCE
			= Util::Config::getValue("Layout.Algorithm.MaxDistance").toFloat();

	state = NO_GRAPH;
	Window::CoreWindow::log(Window::CoreWindow::ALG, "NO GRAPH");
	notEnd = true;
	osg::Vec3f p(0, 0, 0);
	center = p;
	fv = osg::Vec3f();
	last = osg::Vec3f();
	newLoc = osg::Vec3f();
	up = osg::Vec3f();
	vp = osg::Vec3f();
	useMaxDistance = false;
}

void FRAlgorithm::setGraph(Graph *graph) {
	state = PAUSED;
	while (isIterating)
		;//XXX
	notEnd = true;
	this->graph = graph;
	this->randomize();
}

void FRAlgorithm::setParameters(float sizeFactor, float flexibility,
		int animationSpeed, bool useMaxDistance) {
	this->sizeFactor = sizeFactor;
	this->flexibility = flexibility;
	this->useMaxDistance = useMaxDistance;

	if (this->graph != NULL) {
		K = computeCalm();
		graph->setFrozen(false);
	} else {
		qWarning("No graph");
	}
}

/* Urci pokojovu dlzku strun */
double FRAlgorithm::computeCalm() {
	//	double R = 300;
	//	float n = graph->getNodes()->count();
	//	return sizeFactor * pow((4 * R * R * R * PI) / (n * 3), 1 / 3);
	return sizeFactor * 1;
}
/* Rozmiestni uzly na nahodne pozicie */
void FRAlgorithm::randomize() {
	State orig = state;
	while (isIterating)
		state = PAUSED;
	;//XXX

	for (NodeIt i = graph->getNodes()->constBegin(); i
			!= graph->getNodes()->constEnd(); i++) {
		Node* node = i.value();
		if (!node->isFixed()) {
			osg::Vec3f randPos = getRandomLocation();
			node->setTargetPosition(randPos);
		}
	}
	state = orig;
	graph->setFrozen(false);
}

osg::Vec3f FRAlgorithm::getRandomLocation() {
	double l = getRandomDouble() * 300;
	double alpha = getRandomDouble() * 2 * PI;
	double beta = getRandomDouble() * 2 * PI;
	osg::Vec3f newPos = osg::Vec3f((float) (l * sin(alpha)), (float) (l * cos(
			alpha) * cos(beta)), (float) (l * cos(alpha) * sin(beta)));
	return newPos;
}
double FRAlgorithm::getRandomDouble() {
	int p = rand();
	return (double) rand() / (double) RAND_MAX;
}

void FRAlgorithm::play() {
	if (graph != NULL) {
		K = computeCalm();
		graph->setFrozen(false);
		state = RUNNING;
		notEnd = true;
		Window::CoreWindow::log(Window::CoreWindow::ALG, "RUNNING");
		if (!isRunning())
			start(); // don't call start manualy to start this thread
	}
}

void FRAlgorithm::pause() {
	if (graph != NULL) {
		state = PAUSED;
		Window::CoreWindow::log(Window::CoreWindow::ALG, "PAUSED");
	}
}

void FRAlgorithm::wakeUp() {
	if (graph != NULL) {
		graph->setFrozen(false);
	}
}

bool FRAlgorithm::isPlaying() {
	return state == RUNNING;
}

void FRAlgorithm::stop() {
	notEnd = false;
}

void FRAlgorithm::run() {
	this->sleep(Util::Config::getValue("Layout.Thread.StartSleepTime").toLong());
	isIterating = true;

	if (this->graph != NULL) {
		while (notEnd) {
			// slucka pozastavenia - ak je pauza
			// alebo je graf zmrazeny (spravidla pocas editacie)
			while (state != RUNNING || graph->isFrozen()) {
				if (isIterating && graph->isFrozen()) {
					Window::CoreWindow::log(Window::CoreWindow::ALG, "FROZEN");
				}
				isIterating = false;
				QThread::msleep(100);
				if (!notEnd) {
					Window::CoreWindow::log(Window::CoreWindow::ALG, "STOPPED");
					return;
				}
			}
			if (!isIterating) {
				Window::CoreWindow::log(Window::CoreWindow::ALG, "RUNNING");
			}
			isIterating = true;
			if (!iterate()) {
				graph->setFrozen(true);
			}
		}
	} else {
		qWarning("No graph");
	}
}

bool FRAlgorithm::iterate() {
	bool changed = false;
	QMap<qlonglong, osg::ref_ptr<Node> >* nodes = graph->getNodes();
	QMap<qlonglong, Edge* >* edges = graph->getEdges();

	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		i.value()->resetForce();
	}

	//uzly
	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		// pre vsetky uzly..
		Node* u = i.value();
		for (NodeIt j = nodes->constBegin(); j != nodes->constEnd(); j++) {
			// pre vsetky uzly..
			Node* v = j.value();
			if (!u->equals(v)) {
				addRepulsive(u, v, 1); // odpudiva sila beznej velkosti
				addRepulsiveProj(u, v, 1);
			}
		}
	}

	//hrany
	for (EdgeIt i = edges->constBegin(); i != edges->constEnd(); i++) {
		Edge* e = i.value();
		float factor = 1;
		//		if (e->getSrcNode()->isExpanded()) factor /= 2;
		//		if (e->getDstNode()->isExpanded()) factor /= 2;
		addAttractive(e, factor); // pritazliva sila beznej velkosti
	}

	// aplikuj sily na uzly
	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		Node* u = i.value();
		if (!u->isFrozen() && !u->isFixed()) {
			last = u->getTargetPosition();
			bool fo = applyForces(u);
			changed = changed || fo;
		} else {
			changed = false;
		}
	}

	// vracia true ak sa ma pokracovat dalsou iteraciou
	return true; // XXX algoritm will never freeze!
	return changed;
}

bool FRAlgorithm::applyForces(Node* node) {
	// nakumulovana sila
	osg::Vec3f fv = node->getForce();
	// zmensenie
	fv *= ALPHA;
	float l = fv.length();
	if (l > MIN_MOVEMENT) { // nie je sila primala?
		if (l > MAX_MOVEMENT) { // je sila privelka?
			fv.normalize();
			fv *= MAX_MOVEMENT;
			//			std::cout << "max movement (5 used)" << std::endl;
		}
		// pricitame aktualnu rychlost
		fv += node->getVelocity();

		// ulozime novu polohu
		node->setTargetPosition(node->getTargetPosition() + fv);

		// energeticka strata = 1-flexibilita
		fv *= flexibility;
		node->setVelocity(fv); // ulozime novu rychlost
		return true;
	} else {
		node->resetVelocity(); // vynulovanie rychlosti
		return false;
	}
}

/* Pricitanie pritazlivych sil */
void FRAlgorithm::addAttractive(Edge* edge, float factor) {
	up = edge->getSrcNode()->getTargetPosition();
	vp = edge->getDstNode()->getTargetPosition();
	dist = distance(up, vp);
	if (dist == 0)
		return;
	fv = vp - up; // smer sily
	fv.normalize();
	fv *= attr(dist) * factor;// velkost sily
	edge->getSrcNode()->addForce(fv);
	fv = center - fv;
	edge->getDstNode()->addForce(fv);
}

/* Pricitanie pritazlivych sil od metazla */
void FRAlgorithm::addMetaAttractive(Node* u, Node* meta, float factor) {
	up = u->getTargetPosition();
	vp = meta->getTargetPosition();
	dist = distance(up, vp);
	if (dist == 0)
		return;
	fv = vp - up;// smer sily
	fv.normalize();
	fv *= attr(dist) * factor;// velkost sily
	u->addForce(fv);
}

/* Pricitanie odpudivych sil */
void FRAlgorithm::addRepulsive(Node* u, Node* v, float factor) {
	up = u->getTargetPosition();
	vp = v->getTargetPosition();
	dist = distance(up, vp);
	if (useMaxDistance && dist > MAX_DISTANCE) {
		//		std::cout << "max dist" << std::endl;
		return;
	}
	if (dist == 0) {
		// pri splynuti uzlov medzi nimi vytvorime malu vzdialenost
		vp.set(vp.x() + (rand() % 10), vp.y() + (rand() % 10), vp.z() + (rand()
				% 10));
		dist = distance(up, vp);
	}
	fv = (vp - up);// smer sily

	fv.normalize();
	fv *= rep(dist) * factor;// velkost sily
	u->addForce(fv); // iba k jednemu uzlu!
}

/* Pricitanie projektvnych odpudivych sil */
// TODO refactor this method
void FRAlgorithm::addRepulsiveProj(Node* u, Node* v, float factor) {
	if (camera == NULL)
		return;

	up = u->getCurrentPosition(false);
	vp = v->getCurrentPosition(false);

	osg::Matrixd viewM = camera->getViewMatrix();

	// test if the expanded node is befind the other node
	osg::Vec3f upTemp = viewM.getRotate() * up;
	osg::Vec3f vpTemp = viewM.getRotate() * vp;
	if (!(u->isExpanded() && upTemp.z() <= vpTemp.z()) && !(v->isExpanded()
			&& vpTemp.z() <= upTemp.z())) {
		return;
	}
	// NOTE: z decreases when going away from camera

	osg::Matrixd projM = camera->getProjectionMatrix();
	osg::Matrixd windM = camera->getViewport()->computeWindowMatrix();
	osg::Matrixd m = viewM * projM * windM; // complete transformation matrix from world so screen coordinates

	// return if any of nodes is not on screen
	upTemp = up * (viewM * projM);
	vpTemp = vp * (viewM * projM);
	if (qAbs(upTemp.x()) > 1 || qAbs(upTemp.y()) > 1 ||
			qAbs(vpTemp.x()) > 1 || qAbs(vpTemp.y()) > 1)
		return;

	// determine (projected) radius of each node
	osg::Vec3f r = up + osg::Vec3f(u->getRadius(), 0, 0);
	float radU = ((up * m) - (r * m)).length(); // TODO this might not be the correct/best way
	r = vp + osg::Vec3f(v->getRadius(), 0, 0);
	float radV = ((vp * m) - (r * m)).length();

	//	if (u->isExpanded())
	//		std::cout << radU << std::endl;

	// determine distance between projected nodes
	fv = (vp * m) - (up * m);
	dist = fv.length();

	if (dist >= radU + radV) { // are projections overlapping?
		return;
	}

	// transform dist back to world coordinates XXX pseudomath

	dist -= (radU + radV);
	dist = (-dist) * (u->getRadius() / radU);
	//	dist = dist * (u->getRadius()+v->getRadius()) / (radU +radV);

//	std::cout << dist << "," << K <<std::endl;

	// use only view rotation to find force direction
	fv = viewM.getRotate() * (vp - up);// smer sily
	fv.z() = 0;

	fv.normalize(); // force direction


	fv *= -(dist* K)  * factor; // force size

	//	if (u->isExpanded())
	//			std::cout << "len: " << fv.length() << "  "<< dist<<std::endl;

	fv = viewM.getRotate().inverse() * fv; // transform fv back

	u->addForce(fv); // add to node (u only!)
}

/* Vzorec na vypocet odpudivej sily */
float FRAlgorithm::rep(double distance) {
	return (float) (-(K * K) / distance);
}

/* Vzorec na vypocet pritazlivej sily */
float FRAlgorithm::attr(double distance) {
	return (float) ((distance * distance) / K);
}

/* Vzorec na vypocet dostredivej sily */
float FRAlgorithm::centr(double distance) {
	return (float) distance;
}

double FRAlgorithm::distance(osg::Vec3f u, osg::Vec3f v) {
	osg::Vec3f x = u - v;
	return (double) x.length();
}
