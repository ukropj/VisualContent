#include "Model/FRAlgorithm.h"
#include "Model/Edge.h"
#include "Model/Node.h"
#include "Model/Graph.h"
#include "Window/CoreWindow.h"
#include "Util/Config.h"

using namespace Model;

typedef QMap<qlonglong, osg::ref_ptr<Node> >::const_iterator NodeIt;
typedef QMap<qlonglong, osg::ref_ptr<Edge> >::const_iterator EdgeIt;

//Konstruktor pre vlakno s algoritmom
FRAlgorithm::FRAlgorithm() {
	PI = acos((double) -1);
	ALPHA = 0.005;
	MIN_MOVEMENT = 0.05;
	MAX_MOVEMENT = 30;
	MAX_DISTANCE = 400;
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
	double R = 300;
	float n = graph->getNodes()->count();
	return sizeFactor * pow((4 * R * R * R * PI) / (n * 3), 1 / 3);
}
/* Rozmiestni uzly na nahodne pozicie */
void FRAlgorithm::randomize() {
	State orig = state;
	while (isIterating)
		state = PAUSED;;//XXX

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
	QMap<qlonglong, osg::ref_ptr<Edge> >* edges = graph->getEdges();

	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		i.value()->resetForce();
	}

	// TODO change to for or while cycles (QMapIterator)
	//	foreach (Node* node, graph->getNodes()->values())
	//		{ // pre vsetky uzly..
	//			node->resetForce(); // vynulovanie posobiacej sily
	//		}
	//	{//meta uzly
	//
	//		QMap<qlonglong, osg::ref_ptr<Node> >::iterator j;
	//		QMap<qlonglong, osg::ref_ptr<Node> >::iterator k;
	//		j = graph->getMetaNodes()->begin();
	//		for (int i = 0; i < graph->getMetaNodes()->count(); i++, ++j) { // pre vsetky metauzly..
	//			j.value()->resetForce(); // vynulovanie posobiacej sily
	//			k = graph->getMetaNodes()->begin();
	//			for (int h = 0; h < graph->getMetaNodes()->count(); h++, ++k) { // pre vsetky metauzly..
	//				if (!j.value()->equals(k.value())) {
	//					// odpudiva sila medzi metauzlami
	//					addRepulsive(j.value(), k.value(),
	//							Graph::getMetaStrength());
	//				}
	//			}
	//		}
	//	}
	//	{//meta hrany
	//
	//		QMap<qlonglong, osg::ref_ptr<Edge> >::iterator j;
	//		j = graph->getMetaEdges()->begin();
	//		for (int i = 0; i < graph->getMetaEdges()->count(); i++, ++j) { // pre vsetky metahrany..
	//			Node *u = j.value()->getSrcNode();
	//			Node *v = j.value()->getDstNode();
	//			// uzly nikdy nebudu ignorovane
	//			u->setIgnored(false);
	//			v->setIgnored(false);
	//			if (graph->getMetaNodes()->contains(u->getId())) {
	//				// pritazliva sila, posobi na v
	//				addMetaAttractive(v, u, Graph::getMetaStrength());
	//			}
	//			if (graph->getMetaNodes()->contains(v->getId())) {
	//				// pritazliva sila, posobi na u
	//				addMetaAttractive(u, v, Graph::getMetaStrength());
	//			}
	//		}
	//	}

	//uzly
	for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); i++) {
		// pre vsetky uzly..
		Node* u = i.value();
		for (NodeIt j = nodes->constBegin(); j != nodes->constEnd(); j++) {
			// pre vsetky uzly..
			Node* v = j.value();
			if (!u->equals(v)) {
				addRepulsive(u, v, 1); // odpudiva sila beznej velkosti
			}
		}
	}

	//hrany
	for (EdgeIt i = edges->constBegin(); i != edges->constEnd(); i++) {
		addAttractive(i.value(), 1); // pritazliva sila beznej velkosti
	}

	//	if (state == PAUSED) { // XXX
	//		return true;
	//	}

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

	//	// aplikuj sily na metauzly
	//	{
	//		QMap<qlonglong, osg::ref_ptr<Node> >::iterator j;
	//		j = graph->getMetaNodes()->begin();
	//		for (int i = 0; i < graph->getMetaNodes()->count(); i++, ++j) { // pre vsetky metauzly..
	//			if (!j.value()->isFixed()) {
	//				bool fo = applyForces(j.value());
	//				changed = changed || fo;
	//			}
	//		}
	//	}

	// vracia true ak sa ma pokracovat dalsou iteraciou
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
			fv *= 5;
		}
		// pricitame aktualnu rychlost
		fv += node->getVelocity();
		// ulozime novu polohu
		node->setTargetPosition(node->getTargetPosition() + fv);

		// energeticka strata = 1-flexibilita
		fv *= flexibility;
		node->setVelocity(fv); // ulozime novu rychlost
		//node->setForce(*fv);
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
	u->addForce(fv);
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
