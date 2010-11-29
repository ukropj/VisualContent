#include "Model/FRAlgorithm.h"
#include "Model/Edge.h"
#include "Model/Node.h"
#include "Model/Type.h"
#include "Model/Graph.h"
#include "Window/CoreWindow.h"
#include "Util/Config.h"
#include "Viewer/OsgNode.h"
#include <qdebug.h>

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

	sizeFactor = 30;
	flexibility = 0.2f;
	useMaxDistance = true;
	K = sizeFactor;
	M = K / 6.0f;
	coolingSteps = 3;

	Window::CoreWindow::log(Window::CoreWindow::ALG, "NO GRAPH");
}

void FRAlgorithm::setGraph(Graph *newGraph) {
	if (newGraph == NULL)
		return;

	state = PAUSED;
	while (isIterating) {
		qDebug() << "s";
		QThread::msleep(10);
		state = PAUSED;
	}

	notEnd = true;

	if (graph != NULL)
		delete graph; // old graph deleted
	graph = newGraph;

	randomize();
}

void FRAlgorithm::setParameters(float sizeFactor, float flexibility,
		bool useMaxDistance) {
	this->sizeFactor = sizeFactor;
	this->flexibility = flexibility;
	this->useMaxDistance = useMaxDistance;

	if (this->graph != NULL) {
		K = sizeFactor;
		M = K / 6.0f;
		graph->setFrozen(false);
	} else {
		qWarning("No graph");
	}
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
		qDebug() << ALPHA;
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
//					qDebug() << "Frozen";
				}
				isIterating = false;
				QThread::msleep(100);
				if (!notEnd) {
					Window::CoreWindow::log(Window::CoreWindow::ALG, "STOPPED");
//					qDebug() << "Stopped";
					return;
				}
			}
			if (!isIterating) {
				Window::CoreWindow::log(Window::CoreWindow::ALG, "RUNNING");
//				qDebug() << "Running";
			}
			isIterating = true;
			if (!iterate()) {
				cool();
//				graph->setFrozen(true);
				qDebug() << "should freeze";
			}
		}
	} else {
		qWarning("No graph");
	}
	isIterating = false;
}

void FRAlgorithm::cool() { // todo ??
//	if (coolingSteps-- >= 0) {
//		ALPHA /= 2;
//		qDebug() << "cooled, A=" << ALPHA;
//	}
}

bool FRAlgorithm::iterate() {
	bool changed = false;
	QMap<qlonglong, Node*>* nodes = graph->getNodes();
	QMap<qlonglong, Edge*>* edges = graph->getEdges();

	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		i.value()->resetForces();
	}

	//uzly
	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		// pre vsetky uzly..
		Node* u = i.value();
		for (NodeIt j = nodes->constBegin(); j != nodes->constEnd(); j++) {
			// pre vsetky uzly..
			Node* v = j.value();
			if (!u->equals(v)) {
				addRepulsive(u, v); // odpudiva sila beznej velkosti
				addRepulsiveProj(u, v);
			}
		}
	}

	if (state != RUNNING)
		return true;

	//hrany
	for (EdgeIt i = edges->constBegin(); i != edges->constEnd(); i++) {
		Edge* e = i.value();
		addAttractive(e); // pritazliva sila beznej velkosti
	}

	if (state != RUNNING)
		return true;

	// aplikuj sily na uzly
	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		Node* u = i.value();
		if (!u->isFrozen() && !u->isFixed()) {
//			last = u->getPosition();
			bool fo = applyForces(u);
			changed = changed || fo;
//			if (fo)
//				qDebug() << "delta: " << ((last - u->getPosition()).length());
		}
	}

	// vracia true ak sa ma pokracovat dalsou iteraciou
	return changed;
}

bool FRAlgorithm::applyForces(Node* node) {
	// nakumulovana sila
	fv = node->getForce();
	// zmensenie
	fv *= ALPHA;
	fv += node->getVelocity();

	float l = fv.length();
	if (l > MIN_MOVEMENT) { // nie je sila primala?
//		qDebug() << "l: " << l;
		if (l > MAX_MOVEMENT) { // je sila privelka?
			fv.normalize();
			fv *= MAX_MOVEMENT;
		}
		// pricitame aktualnu rychlost

		// ulozime novu polohu
		node->setPosition(node->getPosition() + fv);

		// energeticka strata = 1-flexibilita
		fv *= flexibility;
		node->setVelocity(fv); // ulozime novu rychlost

//		fv = node->getProjForce() * ALPHA;
//		node->setPosition(node->getPosition() + fv);
//		fv *= flexibility;
//		node->setProjVelocity(fv); // ulozime novu rychlost
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

	fv *= attr(dist, qMax(K, edge->getSrcNode()->getRadius() +
			edge->getDstNode()->getRadius())) * factor;// velkost sily

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
	fv *= rep(dist, qMax(K, u->getRadius() + v->getRadius())) * factor;
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
//		factor /= 2.0f;

	osg::Vec3f eye = ou->getEye();
	// is any expanded node behind the other one?
	float udist = distance(up, eye);
	float vdist = distance(vp, eye);
	if (!(uExp && udist >= vdist) && !(vExp && vdist >= udist))
//		return;
		factor /= 4.0f;

	// is any expanded node on screen?
	// XXX is this check improving or lowering performance?
	if ((!vExp && !ou->isOnScreen()) || (!uExp && !ov->isOnScreen()))
		return;

	up = u->getPosition();
	vp = v->getPosition();

	osg::Vec3f edgeDir = vp - up;
	osg::Vec3f viewVec = eye - ((up + vp) / 2.0f); // from eye to middle of u,v
	fv = viewVec ^ (edgeDir ^ viewVec);

	float length = edgeDir.normalize();
	fv.normalize();
	dist = length * qAbs(fv * edgeDir);
	if (dist == 0)
		return; // standard repulsive force will handle this special case

	double angle = acos(ou->getUpVector() * fv);
	float ideal = ou->getDistanceToEdge(osg::PI / 2.0f - angle) // todo optimalize this
			+ ov->getDistanceToEdge(-osg::PI / 2.0f - angle) + M;

	fv *= proj(dist, ideal) * factor;

	u->addForce(fv);
}

float FRAlgorithm::proj(double distance, double ideal) {
	/* Projective repulsive function behaviour
	 * for dist in ranges:
	 * <q * ideal, Inf)			-> no repulsion
	 * <ideal, q * ideal)		-> (weak) linear repulsion, nodes are not overlapping
	 * 								<-p * ideal, 0)
	 * (0, ideal)				-> (strong) hyperbolic repulsion, nodes are overlapping
	 * 								(-Inf, -p * ideal)
	 *
	 * meaning of parameters p and q (both arre relative to ideal):
	 * q is point where repulsion stops, must be more than 1!
	 * 		f(q * ideal) == 0
	 * p is multiplier of strong repulsion, must be at least 1!
	 * 		f(ideal) = -p * ideal;
	 */

	float q = 1.5f;	// repulsion will stop soon after overlap is removed
	float p = 4.0f;	// 5 times stronger than usual repulsion

	if (dist >= q * ideal) {
		return 0;
	} else if (dist >= ideal) {
		return (p/(q-1) * (dist - q * ideal));
	} else if (dist > 0) {
		return -(p * (ideal * ideal) / dist);
	} else {
		return 0;
	}
}

/* Vzorec na vypocet odpudivej sily */
float FRAlgorithm::rep(double distance, double ideal) {
	return (float) (-(ideal * ideal) / distance);
}

/* Vzorec na vypocet pritazlivej sily */
float FRAlgorithm::attr(double distance, double ideal) {
	return (float) ((distance * distance) / ideal);
}

/* Vzorec na vypocet dostredivej sily */
float FRAlgorithm::centr(double distance) {
	return (float) distance;
}

float FRAlgorithm::distance(osg::Vec3f u, osg::Vec3f v) {
	osg::Vec3f x = u - v;
	return x.length();
}
