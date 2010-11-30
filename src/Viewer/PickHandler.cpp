/*
 * PickHandler.cpp
 *
 *  Created on: 25.10.2010
 */

#include "Viewer/PickHandler.h"
#include "Viewer/SceneGraph.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Window/CoreWindow.h"
#include "Util/Config.h"
#include <iostream>

#include <osg/MatrixTransform>
#include <osg/Projection>

using namespace Vwr;

PickHandler::PickHandler(Vwr::CameraManipulator * cameraManipulator,
		Vwr::SceneGraph * sceneGraph) {
	//vytvorenie timera a vynulovanie premennych
	//	timer = new QTimer();
	//	connect(timer, SIGNAL(timeout()), this, SLOT(mouseTimerTimeout()));
	//	pushEvent = NULL;
	//	pushAction = NULL;
	//	releaseEvent = NULL;
	//	releaseAction = NULL;

	this->cameraManipulator = cameraManipulator; // XXX why do we need this?
	this->sceneGraph = sceneGraph;

	originPos = osg::Vec2f(0.0, 0.0);
	lastPos = osg::Vec2f(0.0, 0.0);

	isDrawingSelectionQuad = false;
	isResizingNode = false;
	mode = NONE;
	selectionType = ALL;
	QApplication::setOverrideCursor(Qt::ArrowCursor);

	multiPickEnabled = false;
	createSelectionQuad();
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	switch (event.getEventType()) {

	case osgGA::GUIEventAdapter::PUSH: {
		//		qDebug() << "Mouse pressed";
		//		releaseEvent = NULL;
		// ak dojde push a nie je zapnuty timer tak spusti sa timer a ulozia eventy
		//		if (!timer->isActive()) {
		//			timer->start(
		//					Util::Config::getValue("Mouse.DoubleClickTime").toInt());
		//			pushEvent = &event;
		//			pushAction = &action;
		//			return false; // events not handled here! (see mouseTimerTimeout())
		//		}
		return handlePush(event, action);
	}

	case osgGA::GUIEventAdapter::DOUBLECLICK: {
		//ak dosiel double click tak vypnut timer
		//		timer->stop();
		//		releaseEvent = NULL;
		return handleDoubleclick(event, action);
	}

	case osgGA::GUIEventAdapter::MOVE: {
		return handleMove(event, action);
	}

	case osgGA::GUIEventAdapter::DRAG: {
		//ak je drag a ide timer tak vypnut timer a vyvolat push
		//zaruci sa tak spravne spracovany drag
		//		if (timer->isActive()) {
		//			timer->stop();
		//			handlePush(*pushEvent, *pushAction);
		//		}
		//uz kvoli dalsiemu pokracovaniu dragu
		return handleDrag(event, action);
	}

	case osgGA::GUIEventAdapter::RELEASE: {
		//		qDebug() << "Mouse released";
		//ak je release a je timer aktivny tak sa ulozi event a nevyvola sa
		//		if (timer->isActive()) {
		//			releaseEvent = &event;
		//			releaseAction = &action;
		//			return false; // events not handled here! (see mouseTimerTimeout())
		//		}
		return handleRelease(event, action);
	}

	case osgGA::GUIEventAdapter::KEYDOWN: {
		return handleKeyDown(event, action);
	}

	case ::osgGA::GUIEventAdapter::KEYUP: {
		return handleKeyUp(event, action);
	}

	default:
		return false;
	}
}

void PickHandler::mouseTimerTimeout() {
	//	//ak dobehne timer tak pouzivatel len klikol takze sa zastavi
	//	timer->stop();
	//	//vyvola sa push event
	//	handlePush(*pushEvent, *pushAction);
	//	//v pripade ze bol aj release tak sa vyvola aj release
	//	if (releaseEvent != NULL)
	//		handleRelease(*releaseEvent, *releaseAction);

	// otazka je naco to cele bolo dobre?
	// preco potrebujeme opozdovat realny handling push eventov?
}

bool PickHandler::handlePush(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	//	qDebug() << " push handled";

	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
		sceneGraph->setFrozen(true);
		return false;
	}



	originPos.set(event.getX(), event.getY());
	originNormPos.set(event.getXnormalized(), event.getYnormalized());

	if (isAlt(event)) { // toggle expanded
		OsgNode* node  = pickOne(getViewer(action), event);
		if (selectedNodes.contains(node)) {
			NodeList::const_iterator i;
			i = selectedNodes.constBegin();
			while (i != selectedNodes.constEnd()) {
				(*i)->toggleExpanded();
				++i;
			}
			return true;
		} else {
			if (node != NULL) {
				node->toggleExpanded();
				return true;
			}
		}
		return false;
	} else { // normal selection
		multiPickEnabled = isShift(event);

		bool ret = select(pickOne(getViewer(action), event));

		NodeList::const_iterator i;
		i = selectedNodes.constBegin();
		while (i != selectedNodes.constEnd()) {
			(*i)->setFrozen(true);
			++i;
		}

		if (selectedNodes.isEmpty() || multiPickEnabled) {
			// drawing quad might follow
		}
		return ret;
	}
}

bool PickHandler::handleRelease(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {

	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
		sceneGraph->setFrozen(false);
		return false;
	}
	//	qDebug() << " release handled\n";

	originPos.set(event.getX(), event.getY());

	if (isDrawingSelectionQuad) {
		isDrawingSelectionQuad = false;
		projection->setNodeMask(0);

		if (!multiPickEnabled)
			deselectAll();
		QSet<OsgNode*> nodes = pickMore(getViewer(action), event);
		QSet<OsgNode*>::const_iterator ni = nodes.constBegin();
		multiPickEnabled = true; // temporary to select form quad
		while (ni != nodes.constEnd()) {
			select(*ni); // select node found in quad
			++ni;
		}
		multiPickEnabled = false; // not important even if was true before
	}

	NodeList::const_iterator i = selectedNodes.constBegin();
	while (i != selectedNodes.constEnd()) {
		(*i)->setFrozen(false);
		++i;
	}
	sceneGraph->setFrozen(false);

	return true;
}

bool PickHandler::handleDoubleclick(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		return false;
	qDebug() << "doubleClick";
	//	OsgNode* node = pick(event, action);
	//	if (node != NULL) {
	//		node->toggleExpanded();
	//		return true;
	//	}
	return false;
}

bool PickHandler::handleMove(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	std::cout << event.getX() << "," << event.getY() << std::endl; // XXX is this never called???

	return false;
}

bool PickHandler::handleDrag(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	if (event.getButtonMask() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		return false;
	//	qDebug() << "dragged";
	lastPos.set(event.getX(), event.getY());

	if (!selectedNodes.isEmpty() && !multiPickEnabled
			&& !isDrawingSelectionQuad) { // drag node(s)
		NodeList::const_iterator i = selectedNodes.constBegin();
		osg::Vec2f dragVect = lastPos - originPos;

		while (i != selectedNodes.constEnd()) {
			(*i)->setPosition(getMousePos((*i)->getPosition(), dragVect,
					getViewer(action)));
			++i;
		}

		originPos.set(lastPos.x(), lastPos.y());
		sceneGraph->setFrozen(false);
		return true;
	} else { // draw selecting rectangle
		if (!isDrawingSelectionQuad) { // init quad
			isDrawingSelectionQuad = true;
			drawSelectionQuad(); // to rewrite old coords before first showing
			initSelectionQuad(getViewer(action));
		}
		drawSelectionQuad();
		return true;
	}
	return false;
}

bool PickHandler::handleKeyDown(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
//	qDebug () << event.getKey();
//	qDebug () << event.getModKeyMask();
	if (isAlt(event)) {
		QApplication::setOverrideCursor(Qt::PointingHandCursor);
		return true;
	}
	return false;
}

bool PickHandler::handleKeyUp(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
//	qDebug () << event.getKey();
//	qDebug () << event.getModKeyMask();
	QApplication::restoreOverrideCursor();
	return false;
}

osg::Vec3f PickHandler::getMousePos(osg::Vec3f origPos, osg::Vec2f dragVector,
		osgViewer::Viewer* viewer) {
	osg::Camera* cam = viewer->getCamera();
	osg::Matrixd& viewM = cam->getViewMatrix();
	osg::Matrixd& projM = cam->getProjectionMatrix();
	osg::Matrixd screenM = cam->getViewport()->computeWindowMatrix();
	osg::Matrixd compositeM = viewM * projM * screenM;
	osg::Matrixd compositeMi = compositeMi.inverse(compositeM);

	osg::Vec3f screenPoint = origPos * compositeM;
	osg::Vec3f newPosition = osg::Vec3f(screenPoint.x() + dragVector.x(),
			screenPoint.y() + dragVector.y(), screenPoint.z() + 0);

	return newPosition * compositeMi;
}

void printVect(osg::Vec3f vec) {
	qDebug() << "[" << vec.x() << "," << vec.y() << "," << vec.z() << "]";
}

OsgNode* PickHandler::pickOne(osgViewer::Viewer* viewer,
		const osgGA::GUIEventAdapter& event) {
	if (viewer == NULL)
		return NULL;

	OsgNode* pickedNode = getNodeAt(viewer, event.getX(), event.getY());

	if (pickedNode == NULL) {
		qDebug() << "NO PICK";
	} else {
		qDebug() << "NODE PICKED";
#if 0
		osg::Camera* camera = viewer->getCamera();
		osg::Matrixd viewM = camera->getViewMatrix();
		osg::Matrixd projM = camera->getProjectionMatrix();
		osg::Matrixd windM = camera->getViewport()->computeWindowMatrix();

		osg::Vec3f pos = pickedNode->getCurrentPosition(false);

		qDebug() << "XY: " << event.getX() << "," << event.getY();
		//		qDebug() << "normXY: " << event.getXnormalized() << ","
		//				<< event.getYnormalized();
		printVect(pos);
		printVect((pos * viewM));
		//		printVect((pos * (viewM * projM)));
		osg::Matrixd m = viewM * projM * windM;
		printVect(pos * m);
		//		printVect(((pos * m) * osg::Matrixd::inverse(m)));
		//		osg::Vec3f pos2 = osg::Vec3f(event.getX(), event.getY(), 0.0f);
		//		pos2 = pos2 * osg::Matrixd::inverse(m);
		//		printVect(pos2);

		osg::Vec3f r = pos + osg::Vec3f(pickedNode->getRadius(), 0, 0);
		float rad = ((pos * m) - (r * m)).length();
		qDebug() << "r: " << rad;
#endif
	}

	return pickedNode;
}

QSet<OsgNode*> PickHandler::pickMore(osgViewer::Viewer* viewer,
		const osgGA::GUIEventAdapter& event) {
	QSet<OsgNode*> pickedNodes;
	if (viewer == NULL)
		return pickedNodes;

	float x, y, w, h;

	if (originNormPos.x() < event.getXnormalized()) {
		x = originNormPos.x();
		w = event.getXnormalized();
	} else {
		x = event.getXnormalized();
		w = originNormPos.x();
	}

	if (originNormPos.y() < event.getYnormalized()) {
		y = originNormPos.y();
		h = event.getYnormalized();
	} else {
		y = event.getYnormalized();
		h = originNormPos.y();
	}

	pickedNodes = getNodesInQuad(viewer, x, y, w, h);

	if (pickedNodes.isEmpty())
		qDebug() << "NO PICK";
	else {
		qDebug() << "NODES PICKED";
	}
	return pickedNodes;
}

OsgNode* PickHandler::getNodeAt(osgViewer::Viewer* viewer, const double x,
		const double y) {
	osgUtil::LineSegmentIntersector::Intersections intersections;

	if (viewer->computeIntersections(x, y, intersections)) {
		for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr =
				intersections.begin(); hitr != intersections.end(); hitr++) {
			if (hitr->nodePath.size() <= 2)
				continue;
			osg::NodePath nodePath = hitr->nodePath;

			OsgNode* n =
					dynamic_cast<OsgNode *> (nodePath[nodePath.size() - 2]);
			// NOTE: 2 because structure is Node-Geode-(Drawable)
			// OsgNode is second Node to last
			if (n != NULL) {
				osg::Geode* g = dynamic_cast<osg::Geode *> (nodePath.back());
				if (n->isPickable(g))
					return n;
			}
		}
	}
	return NULL;
}

QSet<OsgNode*> PickHandler::getNodesInQuad(osgViewer::Viewer* viewer,
		const double xMin, const double yMin, const double xMax,
		const double yMax) {
	QSet<OsgNode*> nodes;

	osgUtil::PolytopeIntersector* picker = new osgUtil::PolytopeIntersector(
			osgUtil::Intersector::PROJECTION, xMin, yMin, xMax, yMax);
	osgUtil::IntersectionVisitor iv(picker);
	viewer->getCamera()->accept(iv);

	if (picker->containsIntersections()) {
		osgUtil::PolytopeIntersector::Intersections intersections =
				picker->getIntersections();
		for (osgUtil::PolytopeIntersector::Intersections::iterator hitr =
				intersections.begin(); hitr != intersections.end(); hitr++) {
			if (hitr->nodePath.size() <= 2)
				continue;
			osg::NodePath nodePath = hitr->nodePath;
			//	std::cout << nodePath.size()
			//			<< ": \"" << nodePath.back()->getName() << "\""
			//			<< std::endl;

			OsgNode* n =
					dynamic_cast<OsgNode *> (nodePath[nodePath.size() - 2]);
			if (n != NULL) {
				osg::Geode* g = dynamic_cast<osg::Geode *> (nodePath.back());
				if (n->isPickable(g))
					nodes.insert(n); // don't insert duplicates
			}
		}
	}
	return nodes;
}

bool PickHandler::select(OsgNode* node) {
	if (node == NULL) {
		if (!selectedNodes.isEmpty() && !multiPickEnabled) {
			sceneGraph->setFrozen(false);
			deselectAll();
			return true;
		}
		return false;
	}

	if (selectedNodes.contains(node) && !multiPickEnabled) {
		node->setSelected(true);
		return true;
	}

	if (multiPickEnabled) {
		if (selectedNodes.contains(node)) { // remove from nodes
			node->setSelected(false);
			node->setFrozen(false);
			selectedNodes.removeOne(node);
			//			qDebug() << " one deselected";
			return true;
		} else { // add to nodes
			selectedNodes.append(node);
		}
	} else {
		deselectAll();
		selectedNodes.append(node);
	}

	//	qDebug() << " one selected";
	node->setSelected(true);
	return true;
}

void PickHandler::deselectAll() {
	NodeList::const_iterator i = selectedNodes.constBegin();
	while (i != selectedNodes.constEnd()) {
		(*i)->setSelected(false);
		(*i)->setFrozen(false);
		++i;
	}

	selectedNodes.clear();
}

//bool PickHandler::doEdgePick(osg::NodePath nodePath,
//		unsigned int primitiveIndex) {
//	return false;
//	osg::Geode * geode = dynamic_cast<osg::Geode *> (nodePath[nodePath.size()
//			- 1]);
//
//	if (geode != 0) {
//		osg::Drawable * d = geode->getDrawable(0);
//		osg::Geometry * geometry = d->asGeometry();
//
//		if (geometry != NULL) {
//			OsgEdge * e =
//					dynamic_cast<OsgEdge *> (geometry->getPrimitiveSet(
//							primitiveIndex));
//
//			if (e != NULL) {
//				if (isAltPressed && mode == InputMode::NONE) {
//					osg::ref_ptr<osg::Vec3Array> coords = e->getCooridnates();
//
//					cameraManipulator->setCenter(
//							Util::DataHelper::getMassCenter(coords));
//					cameraManipulator->setDistance(
//							Util::Config::getInstance()->getValue(
//									"Viewer.PickHandler.PickedEdgeDistance").toFloat());
//				} else if (mode != InputMode::NONE) {
//					if (!pickedEdges.contains(e)) {
//						pickedEdges.append(e);
//						e->setSelected(true);
//						//						pickMsg("Edge selected");
//					}
//
//					//					if (isCtrlPressed) {
//					//						unselectPickedEdges(e);
//					//					}
//
//					return true;
//				}
//
//				return true;
//			}
//		}
//	}
//	//	pickMsg("Nothing selected");
//	return false;
//}

void PickHandler::createSelectionQuad() {
	osg::ref_ptr<osg::StateSet> quadStateSet = new osg::StateSet;
	quadStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	quadStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	quadStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	quadStateSet->setAttributeAndModes(new osg::BlendFunc,
			osg::StateAttribute::ON);
	quadStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	quadStateSet->setRenderBinDetails(11, "RenderBin");

	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

	coordinates->push_back(osg::Vec3(0, 1, -1));
	coordinates->push_back(osg::Vec3(0, 1, -1));
	coordinates->push_back(osg::Vec3(0, 1, -1));
	coordinates->push_back(osg::Vec3(0, 1, -1));

	colors->push_back(osg::Vec4(1, 1, 1, 0.1f));

	geometry->setVertexArray(coordinates);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,
			4));
	geometry->setColorArray(colors);
	geometry->setStateSet(quadStateSet);
	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);

	selectionQuad = new osg::Geode;
	selectionQuad->addDrawable(geometry);

	osg::ref_ptr<osg::MatrixTransform> modelViewMatrix =
			new osg::MatrixTransform;
	modelViewMatrix->setMatrix(osg::Matrix::identity());

	modelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	osg::ref_ptr<osg::Group> group = new osg::Group;
	projection = new osg::Projection;
	group->addChild(projection);
	projection->addChild(modelViewMatrix);
	modelViewMatrix->addChild(selectionQuad);

	sceneGraph->getCustomNodeList()->push_back(group);
	projection->setNodeMask(0);
}

void PickHandler::initSelectionQuad(osgViewer::Viewer * viewer) {
	osgViewer::ViewerBase::Windows win;
	viewer->getWindows(win);
	int x, y, w, h;
	win.at(0)->getWindowRectangle(x, y, w, h);
	projection->setMatrix(osg::Matrix::ortho2D(x, w, y, h));
	projection->setNodeMask(1); // make visible
}

void PickHandler::drawSelectionQuad() {
	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;

	coordinates->push_back(osg::Vec3(originPos.x(), originPos.y(), -1));
	coordinates->push_back(osg::Vec3(originPos.x(), lastPos.y(), -1));
	coordinates->push_back(osg::Vec3(lastPos.x(), lastPos.y(), -1));
	coordinates->push_back(osg::Vec3(lastPos.x(), originPos.y(), -1));

	selectionQuad->getDrawable(0)->asGeometry()->setVertexArray(coordinates);
}

osg::Vec3 PickHandler::getSelectionCenter() {

	NodeList::const_iterator ni = selectedNodes.constBegin();
	osg::Vec3f sum(0, 0, 0);

	while (ni != selectedNodes.constEnd()) {
		sum += (*ni)->getPosition();
		++ni;
	}
	int num = selectedNodes.size();
	if (num == 0)
		num = 1;

	return osg::Vec3f(sum.x() / num, sum.y() / num, sum.z() / num);
}

bool PickHandler::isShift(const osgGA::GUIEventAdapter& event) {
	int key = event.getModKeyMask();
	if (key & osgGA::GUIEventAdapter::MODKEY_SHIFT) {
		//		StatusLogger::log(StatusLogger::KEYS, "SHIFT");
		qDebug() << "SHIFT";
		return true;
	}
	return false;
}

bool PickHandler::isCtrl(const osgGA::GUIEventAdapter& event) {
	int key = event.getModKeyMask();
	if (key & osgGA::GUIEventAdapter::MODKEY_CTRL) {
		//		StatusLogger::log(StatusLogger::KEYS, "CTRL");
		qDebug() << "CTRL";
		return true;
	}
	return false;
}

bool PickHandler::isAlt(const osgGA::GUIEventAdapter& event) {
	int key = event.getModKeyMask();
	if (key & osgGA::GUIEventAdapter::MODKEY_ALT) {
		//		StatusLogger::log(StatusLogger::KEYS, "CTRL");
		qDebug() << "ALT";
		return true;
	}
	return false;
}

