#include "Model/FRAlgorithm.h"
#include "Model/Edge.h"
#include "Model/Node.h"
#include "Model/Graph.h"
#include "Window/CoreWindow.h"
#include "Util/Config.h"
#include "Util/CameraHelper.h"
#include "Viewer/OsgNode.h"
#include <QDebug>

using namespace Model;

FRAlgorithm::FRAlgorithm() {
	ALPHA = Util::Config::getValue("Layout.Algorithm.Alpha").toFloat();
	MIN_MOVEMENT = Util::Config::getValue("Layout.Algorithm.MinMovement").toFloat();
	MAX_MOVEMENT = Util::Config::getValue("Layout.Algorithm.MaxMovement").toFloat();
	MAX_DISTANCE = Util::Config::getValue("Layout.Algorithm.MaxDistance").toFloat();

	state = NO_GRAPH;
	isIterating = false;
	graph = NULL;
	center = osg::Vec3f(0, 0, 0);
	last = osg::Vec3f();
	notEnd = true;

	sizeFactor = Util::Config::getValue("Layout.Algorithm.CalmDistance").toFloat();
	flexibility = Util::Config::getValue("Layout.Algorithm.Flexibility").toFloat();
	useMaxDistance = MAX_DISTANCE > 0;
	K = sizeFactor;
	M = K / 8.0f;

	Window::CoreWindow::log(Window::CoreWindow::ALG, "NO GRAPH");
}

FRAlgorithm::~FRAlgorithm() {
}

void FRAlgorithm::setGraph(Graph *newGraph) {
	if (newGraph == NULL)
		return;
	State origState = state;

	state = PAUSED;
	while (isIterating) {
		qDebug() << "s";
		QThread::msleep(100);
		state = PAUSED;
	}
	if (graph != NULL)
		delete graph; // old graph deleted
	graph = newGraph;
	notEnd = true;
	ALPHA = Util::Config::getValue("Layout.Algorithm.Alpha").toFloat();

	state = origState;
	randomize();
}

void FRAlgorithm::setParameters(float sizeFactor, float flexibility,
		bool useMaxDistance) {
	this->sizeFactor = sizeFactor;
	this->flexibility = flexibility;
	this->useMaxDistance = useMaxDistance;

	if (this->graph != NULL) {
		K = sizeFactor;
		M = K / 8.0f;
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
		qDebug() << "r";
		QThread::msleep(100);
		state = PAUSED;
	}

	QMap<qlonglong, Node*>* nodes = graph->getNodes();
	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); ++i) {
		Node* node = i.value();
		if (!node->isFixed() && !node->isIgnored()) {
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
//		qDebug() << ALPHA;
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
//			QTime t = QTime::currentTime();
			isIterating = true;
			if (!iterate()) {
				graph->setFrozen(true);
				resetNodes();
//				qDebug() << "should freeze";
			}
//			QTime t2 = QTime::currentTime();
//			qDebug() << t.msecsTo(t2);
		}
	} else {
		qWarning("No graph");
	}
	isIterating = false;
}

void FRAlgorithm::resetNodes() {
	QMap<qlonglong, Node*>* nodes = graph->getNodes();
	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); ++i) {
		Node* n = i.value();
		n->resetVelocity(); // reset velocity
	}
	// helps to reduce oscillations when this is NOT reseted
	// for each individual node in applyForces as before
}

bool FRAlgorithm::iterate() {
	bool changed = false;
	QMap<qlonglong, Node*>* nodes = graph->getNodes();

	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); ++i) {
		i.value()->resetForce();
	}

	for (NodeIt ui = nodes->constBegin(); ui != nodes->constEnd(); ++ui) {
		Node* u = ui.value();
		if (!u->isCluster() || !u->isIgnored()) {
			for (NodeIt vi = ui+1; vi != nodes->constEnd(); ++vi) {
				Node* v = vi.value();
				addStandardForces(u, v);
				addProjectiveForce(u, v);
			}
		}
	}

	if (state != RUNNING) // premature break
		return true;

	// apply accumulated forces to nodes
	for (NodeIt ui = nodes->constBegin(); ui != nodes->constEnd(); ++ui) {
		Node* u = ui.value();
		if (!u->isFrozen() && !u->isFixed() && !u->isIgnored()) {
			bool fo = applyForces(u);
			changed = changed || fo;
		}
	}

	// if true, iteration wil continue, if false, graph freezes
	return changed;
}

void FRAlgorithm::addStandardForces(Node* realU, Node* realV) {
	if (realU->isCluster() || realV->isCluster()) // clusters will be updated by descendants
		return;

	// find visible ancestors
	Node* u;
	if (realU->isIgnored()) {	// realU is not visible
		u = realU->getTopCluster(); // all calculations will be performed on
									// hierarchically closest visible cluster
		if (u == NULL) return; // realU is cluster that is currently unclustered
	} else {					// realU is visible
		u = realU;					// all calculations as usual
	}
	Node* v;
	if (realV->isIgnored()) {
		v = realV->getTopCluster();
		if (v == NULL) return;
	} else {
		v = realV;
	}

//	if (u->equals(v)) // XXX why is this causong problems ??
//		return;

	// compute factors (use original input nodes)
	float repFactor = (realU->getWeight() + realV->getWeight()) / 2.0f;
	Edge* e = realU->getEdgeTo(realV);
	float attrFactor = e == NULL ? 0 : e->getWeight();
	osg::Vec3f vec = getVector(u, v);

	// replace [0, 0, 0] by small random vector
	if (!u->equals(v) && vec.length() == 0) {
//		qDebug() << "dist == 0 " << u->toString() << " - " << v->toString();
		vec.set(getRandomDouble() * 10, getRandomDouble() * 10, getRandomDouble() * 10);
	}
	// compute forces
	float dist = vec.normalize();					// distance between nodes
	float attrF = attr(dist, K) * attrFactor;
	float replF = rep(dist, K) * repFactor;
	osg::Vec3f fv = vec * (attrF + replF);
	// add forces to nodes
	u->addForce(fv);
	v->addForce(-fv);
}

void FRAlgorithm::addProjectiveForce(Node* u, Node* v) {
	// possible to overlap?
	if (!Vwr::OsgNode::mayOverlap(u->getOsgNode(), v->getOsgNode()))
		return;
	osg::Vec3f pvec = getProjVector(u, v);			// compute projective vector
	if (pvec.length() == 0)
		return;
	float pdist = pvec.normalize();					// projective distance between nodes
	float pideal = getMinProjDistance(u, v, pvec); 	// minimal projective distance
	float projF = proj(pdist, pideal);				// projective force
	osg::Vec3f fv = pvec * projF;					// add projective force
	u->addForce(fv);
	v->addForce(-fv);
	// NOTE: node weight is not accounted for, force depends on node radius
}

bool FRAlgorithm::applyForces(Node* node) {
	osg::Vec3f fv = node->getForce();
	// scale
	fv *= ALPHA;
	// add current velocity
	fv += node->getVelocity();
	// divide by weight
	fv /= sqrt(node->getWeight());

	float l = fv.length();
	if (l > MIN_MOVEMENT) { // not too small ?
		if (l > MAX_MOVEMENT) { // not too big
			fv.normalize();
			fv *= MAX_MOVEMENT;
		}
		// save new position
		node->setPosition(node->getPosition() + fv);
		// save new velocity
		node->setVelocity(fv * flexibility);
		return true;
	} else {
		return false;
	}
}

osg::Vec3f FRAlgorithm::getVector(Node* u, Node* v) {
	osg::Vec3f up = u->getPosition();
	osg::Vec3f vp = v->getPosition();
	if (up == vp) {
		vp.set(vp.x() + (rand() % 10), vp.y() + (rand() % 10), vp.z() + (rand()% 10));
	}
	return vp - up;
}

osg::Vec3f FRAlgorithm::getProjVector(Node* u, Node* v) {
	osg::Vec3f up = u->getPosition();
	osg::Vec3f vp = v->getPosition();

	osg::Vec3f edgeDir = vp - up;
	osg::Vec3f viewVec = Util::CameraHelper::getEye() - ((up + vp) / 2.0f); // from eye to middle of u,v
	osg::Vec3f pv = viewVec ^ (edgeDir ^ viewVec);

	float length = edgeDir.normalize();
	pv.normalize();
	float dist = length * qAbs(pv * edgeDir);
	if (dist == 0) {
		return osg::Vec3f(0, 0, 0);
	}
	return pv * dist;
}

float FRAlgorithm::getMinProjDistance(Node* u, Node* v, osg::Vec3f pv) {
	if (pv.length() == 0)
		return 0;
	Vwr::OsgNode* ou = u->getOsgNode();
	Vwr::OsgNode* ov = v->getOsgNode();
//	double angle = acos(Util::CameraHelper::getUp() * pv);
	float ideal = ou->getRadius() + ov->getRadius() + M;
//	float ideal = ou->getDistanceToEdge(osg::PI / 2.0f - angle) // todo optimalize this
//			+ ov->getDistanceToEdge(-osg::PI / 2.0f - angle) + M;
	return ideal;
}

float FRAlgorithm::proj(double distance, double ideal) {
	float f = -(4 * ideal * ideal / distance) + 2 * ideal;
	if (f > 0)
		return 0;
	return f;
}

/* Vzorec na vypocet odpudivej sily */
float FRAlgorithm::rep(double distance, double ideal) {
	if (useMaxDistance && distance > MAX_DISTANCE)
		return 0;
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
