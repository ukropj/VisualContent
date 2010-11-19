#include "Model/FRAlgorithm.h"
#include "Model/Edge.h"
#include "Model/Node.h"
#include "Model/Type.h"
#include "Model/Graph.h"
#include "Window/CoreWindow.h"
#include "Util/Config.h"
#include "Viewer/OsgNode.h"

using namespace Model;

typedef QMap<qlonglong, Node*>::const_iterator NodeIt;
typedef QMap<qlonglong, Edge*>::const_iterator EdgeIt;

//Konstruktor pre vlakno s algoritmom
FRAlgorithm::FRAlgorithm() {
	ALPHA = Util::Config::getValue("Layout.Algorithm.Alpha").toFloat();
	MIN_MOVEMENT
			= Util::Config::getValue("Layout.Algorithm.MinMovement").toFloat();
	MAX_MOVEMENT
			= Util::Config::getValue("Layout.Algorithm.MaxMovement").toFloat();
	MAX_DISTANCE
			= Util::Config::getValue("Layout.Algorithm.MaxDistance").toFloat();

	state = NO_GRAPH;
	isIterating = false;
	graph = NULL;
	center = osg::Vec3f(0, 0, 0);
	fv = osg::Vec3f();
	last = osg::Vec3f();
	up = osg::Vec3f();
	vp = osg::Vec3f();
	useMaxDistance = false;
	notEnd = true;

	Window::CoreWindow::log(Window::CoreWindow::ALG, "NO GRAPH");
}

void FRAlgorithm::setGraph(Graph *newGraph) {
	state = PAUSED;
	while (isIterating) {
		qDebug() << "s";
		QThread::msleep(100);
		state = PAUSED;
	}

	notEnd = true;

	qDebug() << "deleting graph:";
	if (newGraph != NULL) {
		if (graph != NULL)
			delete graph; // old graph deleted
		graph = newGraph;
	}
	qDebug() << "graph deleted";

	randomize();
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
double FRAlgorithm::computeCalm() {// todo delete
	//	double R = 300;
	//	float n = graph->getNodes()->count();
	//	return sizeFactor * pow((4 * R * R * R * PI) / (n * 3), 1 / 3);
	return sizeFactor * 1;
}
/* Rozmiestni uzly na nahodne pozicie */
void FRAlgorithm::randomize() {
	State origState = state;
	state = PAUSED;
	while (isIterating) {
		qDebug() << "s";
		QThread::msleep(100);
		state = PAUSED;
	}

	for (NodeIt i = graph->getNodes()->constBegin(); i
			!= graph->getNodes()->constEnd(); i++) {
		Node* node = i.value();
		if (!node->isFixed()) {
			osg::Vec3f randPos = getRandomLocation();
			node->setPosition(randPos);
		}
	}
	state = origState;
	graph->setFrozen(false);
}

osg::Vec3f FRAlgorithm::getRandomLocation() {
	double l = getRandomDouble() * 300;
	double alpha = getRandomDouble() * 2 * osg::PI;
	double beta = getRandomDouble() * 2 * osg::PI;
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
	msleep(Util::Config::getValue("Layout.Thread.StartSleepTime").toLong());
	isIterating = true;

	if (this->graph != NULL) {
		while (notEnd) {
			// slucka pozastavenia - ak je pauza
			// alebo je graf zmrazeny (spravidla pocas editacie)
			while (state != RUNNING || graph->isFrozen()) {
				if (isIterating && graph->isFrozen()) {
					Window::CoreWindow::log(Window::CoreWindow::ALG, "FROZEN");
					qDebug() << "Frozen";
				}
				isIterating = false;
				QThread::msleep(100);
				if (!notEnd) {
					Window::CoreWindow::log(Window::CoreWindow::ALG, "STOPPED");
					qDebug() << "Stopped";
					return;
				}
			}
			if (!isIterating) {
				Window::CoreWindow::log(Window::CoreWindow::ALG, "RUNNING");
				qDebug() << "Running";
			}
			isIterating = true;
			if (!iterate()) {
				graph->setFrozen(true);
			}
		}
	} else {
		qWarning("No graph");
	}
	isIterating = false;
}

bool FRAlgorithm::iterate() {
	bool changed = false;
	QMap<qlonglong, Node*>* nodes = graph->getNodes();
	QMap<qlonglong, Edge*>* edges = graph->getEdges();

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
				if (e->getSrcNode()->getOsgNode()->isExpanded()) factor /= 1.5;
				if (e->getDstNode()->getOsgNode()->isExpanded()) factor /= 1.5;
		addAttractive(e, factor); // pritazliva sila beznej velkosti
	}

	if (state == PAUSED)
		return true;

	// aplikuj sily na uzly
	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		Node* u = i.value();
		if (!u->isFrozen() && !u->isFixed()) {
			last = u->getPosition();
			bool fo = applyForces(u);
			bool co = true;
//			if (u->getOsgNode() != NULL && u->getOsgNode()->isExpanded()) {
//				for (NodeIt j = nodes->constBegin(); j != nodes->constEnd(); j++) {
//					Node* v = j.value();
//					if (!u->equals(v)) {
//						co = moveApart(u, v) || co;
//					}
//				}
//			}
			changed = changed || (co && fo);
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
		node->setPosition(node->getPosition() + fv);

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
	up = edge->getSrcNode()->getPosition();
	vp = edge->getDstNode()->getPosition();
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

/* Pricitanie odpudivych sil */
void FRAlgorithm::addRepulsive(Node* u, Node* v, float factor) {
	up = u->getPosition();
	vp = v->getPosition();
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
void FRAlgorithm::addRepulsiveProj(Node* u, Node* v, float factor) {
	Vwr::OsgNode* ou = u->getOsgNode();
	Vwr::OsgNode* ov = v->getOsgNode();
	// are both nodes rendering?
	if (ou == NULL || ov == NULL)
		return;

	bool uExp = ou->isExpanded();
	bool vExp = ov->isExpanded();
	// is any node expanded?
	if (!uExp && !vExp)
		return;

	osg::Vec3f eye = ou->getEye();
	// is any expanded node behind the other one?
	float udist = (up - eye).length();
	float vdist = (vp - eye).length();
	if (!(uExp && udist >= vdist) && !(vExp && vdist >= udist)) {
		//		if (uExp)
		//			qDebug() << "not behind ";
		return;
	}

	// is any node on screen?
	if ((!vExp && !ou->isOnScreen()) || (!uExp && !ov->isOnScreen()))
		return;

	//	if (uExp)
	//		qDebug() << "obscured";

	up = u->getPosition();
	vp = v->getPosition();

	osg::Vec3f edgeDir = up - vp;
	osg::Vec3f viewVec = eye - (up + vp) / 2.0f;
	fv = viewVec ^ (edgeDir ^ viewVec);

	float length = edgeDir.normalize();
	fv.normalize();

	//	qDebug() << "len: " << length;
	//	if (uExp)
	//		qDebug() << "deg: " << acos(edgeDir * ref) * 360 / (2 * osg::PI);
	//	qDebug() << "cos: " << edgeDir * ref;

	dist = length * qAbs(edgeDir * fv);

	//	if (uExp)
	//			qDebug() << dist;

	// dist = overlapping distance
	double angle = acos(ou->getUpVector()*fv);
	// XXX 1.5 design better function
	dist = (ou->getDistanceToEdge(osg::PI/2.0f - angle) + ov->getDistanceToEdge(-osg::PI/2.0f - angle))*1.5f - dist;
//	dist = (ou->getRadius() + ov->getRadius()) - dist;

	//	if (uExp)
	//		qDebug() << dist;

	if (dist <= 0) { // are projections overlapping?
		return;
	}

	fv *= (dist * dist / K) * factor; // force size

	u->addForce(fv);
}

//bool FRAlgorithm::moveApart(Node* u, Node* v) {
//	Vwr::OsgNode* ou = u->getOsgNode();
//	Vwr::OsgNode* ov = v->getOsgNode();
//	// are both nodes rendering?
//	if (ou == NULL || ov == NULL)
//		return false;
//
//	bool uExp = ou->isExpanded();
//	bool vExp = ov->isExpanded();
//	// is any node expanded?
//	if (!uExp && !vExp)
//		return false;
//
//	osg::Vec3f eye = ou->getEye();
//	// is any expanded node behind the other one?
//	float udist = distance(up, eye);
//	float vdist = distance(vp, eye);
//	if (!(uExp && udist >= vdist) && !(vExp && vdist >= udist)) {
//		//		if (uExp)
//		//			qDebug() << "not behind ";
//		return false;
//	}
//
//	// is any node on screen?
//	if ((!vExp && !ou->isOnScreen()) || (!uExp && !ov->isOnScreen()))
//		return false;
//
//	//	if (uExp)
//	//		qDebug() << "obscured";
//
//	up = u->getPosition();
//	vp = v->getPosition();
//
//	osg::Vec3f edgeDir = up - vp;
//	osg::Vec3f viewVec = eye - (up + vp) / 2.0f;
//	fv = viewVec ^ (edgeDir ^ viewVec);
//
//	float length = edgeDir.normalize();
//	fv.normalize();
//
//	//	qDebug() << "len: " << length;
//	//	if (uExp)
//	//		qDebug() << "deg: " << acos(edgeDir * ref) * 360 / (2 * osg::PI);
//	//	qDebug() << "cos: " << edgeDir * ref;
//
//	dist = length * qAbs(edgeDir * fv);
//
//	if (uExp)
//		qDebug() << dist << " " << ou->getRadius() << " " << ov->getRadius();
//
//	// dist = overlapping distance
//	//	dist = (ou->getDistanceToEdge(fv) + ov->getDistanceToEdge(-fv)) - dist;
//	dist = (ou->getRadius() + ov->getRadius()) - dist;
//
//	if (dist <= 0) { // are projections overlapping?
//		return false;
//	}
//
//	if (uExp)
//		qDebug() << dist;
//
//	dist /= 2.0f;
//	//	fv *= (dist * dist / (2 * K)) * factor; // force size
//	u->setPosition(up + (fv * dist));
//	v->setPosition(vp - (fv * dist));
//	//	u->addForce(fv);
//	return true;
//}

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
