/*
 * PickHandler.cpp
 *
 *  Created on: 25.10.2010
 */

#include "Viewer/PickHandler.h"
#include "Viewer/SceneGraph.h"
#include "Viewer/OsgNode.h"
#include "Viewer/DataMapping.h"
#include "Viewer/AbstractNode.h"
#include "Viewer/OsgNodeGroup.h"
#include "Viewer/ControlFrame.h"
#include "Util/CameraHelper.h"

#include <iostream>
#include <QApplication>

#include <osg/MatrixTransform>
#include <osg/Projection>

using namespace Vwr;
typedef QList<OsgNode* >::const_iterator NodeIterator;
float PickHandler::MIN_SEL_DIAG = 5;

PickHandler::PickHandler(SceneGraph * sceneGraph) {
	//vytvorenie timera a vynulovanie premennych
	//	timer = new QTimer();
	//	connect(timer, SIGNAL(timeout()), this, SLOT(mouseTimerTimeout()));
	//	pushEvent = NULL;
	//	pushAction = NULL;
	//	releaseEvent = NULL;
	//	releaseAction = NULL;

	this->sceneGraph = sceneGraph;

	originPos = osg::Vec2f(0.0, 0.0);
	lastPos = osg::Vec2f(0.0, 0.0);

	isDrawingSelectionQuad = false;
	isResizingNode = false;
	multiPickEnabled = false;
	pressedKey = 0;
	mode = NORMAL;

	createSelectionQuad();
	nodeFrame = sceneGraph->getNodeFrame();
}

PickHandler::~PickHandler() {
//	qDebug() << "PickHandler deleted";
}

void PickHandler::reset() {
	deselectAll();
	multiPickEnabled = false;
	isResizingNode = false;
	isDrawingSelectionQuad = false;
	pressedKey = 0;
	QApplication::restoreOverrideCursor();
	nodeFrame->hide();
	nodeFrame->setNode(NULL);
	nodeFrame->updateProjection();
	selectionQuad->setNodeMask(0);
	originPos = osg::Vec2f(0.0, 0.0);
	lastPos = osg::Vec2f(0.0, 0.0);
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
}

bool PickHandler::handlePush(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	//	qDebug() << " push handled";

	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
		sceneGraph->setFrozen(true);
		return false;
	}

	originPos.set(event.getX(), event.getY());
	lastPos.set(originPos.x(), originPos.y());

	if (mode == DEBUG) {
		if (isAlt(event)) { // toggle nodes
			OsgNode* node  = pickOne(getViewer(action), event);
			if (selectedNodes.contains(node)) {
				NodeIterator i = selectedNodes.constBegin();
				while (i != selectedNodes.constEnd()) {
					(*i)->setExpanded(!(*i)->isExpanded());
					++i;
				}
				return true;
			} else {
				if (node != NULL) {
					node->setExpanded(!node->isExpanded());
					return true;
				}
			}
			return false;
		} else if ('a' == pressedKey) {	// start resizing
			select(pickOne(getViewer(action), event), true);
			if (selectedNodes.size() == 1 && selectedNodes.first()->isExpanded()) {
				selectedNodes.first()->setFrozen(true);
				isResizingNode = true;
				QApplication::setOverrideCursor(Qt::SizeAllCursor);
				return true;
			} else {
				return false;
			}
		} else { // normal selection
			multiPickEnabled = isShift(event);

			bool ret = select(pickOne(getViewer(action), event));

			NodeIterator i = selectedNodes.constBegin();
			while (i != selectedNodes.constEnd()) {
				(*i)->setFrozen(true);
				++i;
			}

			if (selectedNodes.isEmpty() || multiPickEnabled) {
				// drawing quad might follow
			}
			return ret;
		}
	} else {
		multiPickEnabled = isShift(event); // TODO move to frame?
		nodeFrame->deactivateAction();


		OsgNode* node = pickOne(getViewer(action), event);
		if (nodeFrame->isActive() || node != NULL) {
			if (node != NULL) {
				if (multiPickEnabled) {
					nodeFrame->setNode(OsgNodeGroup::merge(nodeFrame->getNode(), node));
				} else {
					nodeFrame->setNode(node);
				}
				sceneGraph->setFrozen(false);
				nodeFrame->show();
			}
		} else if (!multiPickEnabled) {
			nodeFrame->hide();
			nodeFrame->setNode(NULL);
		}
		return nodeFrame->handlePush(event);
	}
}

bool PickHandler::handleRelease(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {

	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
		sceneGraph->setFrozen(false);
		return false;
	}
	//	qDebug() << " release handled\n";

	if (mode == DEBUG) {
		if (isDrawingSelectionQuad) {
			isDrawingSelectionQuad = false;
			selectionQuad->setNodeMask(0);

			if (!multiPickEnabled)
				deselectAll();
			multiPickEnabled = true; // temporary to select form quad
			QList<OsgNode*> nodes = pickMore(getViewer(action), event);
			NodeIterator i = nodes.constBegin();
			while (i != nodes.constEnd()) {
				select(*i); // select node found in quad
				++i;
			}
			multiPickEnabled = false; // not important even if was true before
		}
		if (isResizingNode) {
			isResizingNode = false;
			QApplication::restoreOverrideCursor();
		}
		NodeIterator i = selectedNodes.constBegin();
		while (i != selectedNodes.constEnd()) {
			(*i)->setFrozen(false);
			++i;
		}
		sceneGraph->setFrozen(false);
		return true;
	} else {
		if (isDrawingSelectionQuad) {
			isDrawingSelectionQuad = false;
			selectionQuad->setNodeMask(0);

			QList<OsgNode*> nodes = pickMore(getViewer(action), event);
			NodeIterator i = nodes.constBegin();
			if (!nodes.isEmpty()) {
				OsgNodeGroup* group = new OsgNodeGroup();
				while (i != nodes.constEnd()) {
					group->addNode(*i, true, false);
					++i;
				}
				group->updateSizeAndPos();
				if (multiPickEnabled) {
					nodeFrame->addNode(group);
				} else {
					nodeFrame->setNode(group);
				}
				group = NULL;
				nodeFrame->show();
				sceneGraph->setFrozen(false);
			} else {
				qDebug() << "hide on release";
				nodeFrame->hide();
				nodeFrame->setNode(NULL);
			}
		} else {
			if (nodeFrame->handleRelease(event)) {
				sceneGraph->setFrozen(false);
				return true;
			} else {
				return false;
			}
		}
	}
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
	osg::Vec2f thisPos(event.getX(), event.getY());

	if (mode == DEBUG) {
		if (!selectedNodes.isEmpty() && !multiPickEnabled
				&& !isDrawingSelectionQuad) {
			if (isResizingNode) {	// resize node (s)
				NodeIterator i = selectedNodes.constBegin();
				osg::Vec2f nodePos = Util::CameraHelper::worldToScreen((*i)->getPosition());
				osg::Vec2f newVect = thisPos - nodePos;
				osg::Vec2f oldVect = lastPos - nodePos;

				(*i)->resize(newVect.length() / oldVect.length());
				lastPos.set(thisPos.x(), thisPos.y());
			} else { 				// drag node(s)
				NodeIterator i = selectedNodes.constBegin();
				osg::Vec2f dragVect = thisPos - lastPos;

				while (i != selectedNodes.constEnd()) {
					(*i)->setPosition(Util::CameraHelper::moveByScreenVector((*i)->getPosition(), dragVect));
					++i;
				}

				lastPos.set(thisPos.x(), thisPos.y());
				sceneGraph->setFrozen(false);
				return true;
			}
		} else {					// draw selecting rectangle
			lastPos.set(thisPos.x(), thisPos.y());
			if (!isDrawingSelectionQuad) { // init quad
				isDrawingSelectionQuad = true;
				drawSelectionQuad(); // to rewrite old coords before first showing
				initSelectionQuad();
			}
			drawSelectionQuad();
			return true;
		}
	} else {
		if (!multiPickEnabled && !isDrawingSelectionQuad && nodeFrame->handleDrag(event)) {
			sceneGraph->setFrozen(false); // node moved
			return true;
		} else {					// draw selecting rectangle
			lastPos.set(thisPos.x(), thisPos.y());
			if (!isDrawingSelectionQuad &&
					(originPos - lastPos).length() > MIN_SEL_DIAG) { // init quad
				isDrawingSelectionQuad = true;
				drawSelectionQuad(); // rewrite old coords before showing
				initSelectionQuad();
			}
			if (isDrawingSelectionQuad)
				drawSelectionQuad();
			return true;
		}
	}
}

bool PickHandler::handleKeyDown(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	//	qDebug () << event.getKey();
	//	qDebug () << event.getModKeyMask();

	pressedKey = event.getKey(); // TODO more keys??
	if (mode == DEBUG && isAlt(event)) {
		QApplication::setOverrideCursor(Qt::PointingHandCursor);
		return true;
	}
	return false;
}

bool PickHandler::handleKeyUp(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	//	qDebug () << event.getKey();
	//	qDebug () << event.getModKeyMask();
	if (mode == DEBUG && isAlt(event)) {
		QApplication::restoreOverrideCursor();
	}
	pressedKey = 0; // TODO more keys??
	return false;
}

OsgNode* PickHandler::pickOne(osgViewer::Viewer* viewer,
		const osgGA::GUIEventAdapter& event) {
	if (viewer == NULL)
		return NULL;

	OsgNode* pickedNode = getNodeAt(viewer, event.getX(), event.getY());

	/*if (pickedNode == NULL) {
		qDebug() << "NO PICK";
	} else {
		qDebug() << "NODE PICKED";*/
#if 0
		osg::Camera* camera = viewer->getCamera();
		osg::Matrixd viewM = camera->getViewMatrix();
		osg::Matrixd projM = camera->getProjectionMatrix();
		osg::Matrixd windM = camera->getViewport()->computeWindowMatrix();

		osg::Vec3f pos = pickedNode->getPosition();

		qDebug() << "XY: " << event.getX() << "," << event.getY();
		//		qDebug() << "normXY: " << event.getXnormalized() << ","
		//				<< event.getYnormalized();
		//		Util::CameraHelper::printVec(pos);
		//		Util::CameraHelper::printVec(pos * viewM);
		//		Util::CameraHelper::printVec(pos * (viewM * projM));
		osg::Matrixd m = viewM * projM * windM;
		pos = pos * m;
		//		Util::CameraHelper::printVec(pos * m);
		qDebug() << "NP: " << pos.x() << "," << pos.y();
		//		Util::CameraHelper::printVec((pos * m) * osg::Matrixd::inverse(m));
		//		osg::Vec3f pos2 = osg::Vec3f(event.getX(), event.getY(), 0.0f);
		//		pos2 = pos2 * osg::Matrixd::inverse(m);
		//		Util::CameraHelper::printVec(pos2);
#endif
	//}

	return pickedNode;
}

QList<OsgNode* > PickHandler::pickMore(osgViewer::Viewer* viewer,
		const osgGA::GUIEventAdapter& event) {
	QList<OsgNode* > pickedNodes;
	if (viewer == NULL)
		return pickedNodes;

	float x, y, w, h;

	if (originPos.x() < event.getX()) {
		x = originPos.x();
		w = event.getX();
	} else {
		x = event.getX();
		w = originPos.x();
	}

	if (originPos.y() < event.getY()) {
		y = originPos.y();
		h = event.getY();
	} else {
		y = event.getY();
		h = originPos.y();
	}

	pickedNodes = getNodesInQuad(viewer, x, y, w, h);

	/*if (pickedNodes.isEmpty()) {
		qDebug() << "NO PICK";
	} else {
		qDebug() << "NODES PICKED";
	}*/
	return pickedNodes;
}


OsgNode* PickHandler::getNodeAt(osgViewer::Viewer* viewer, const double x, const double y) {
	osgUtil::LineSegmentIntersector::Intersections intersections;
	OsgNode* pickedNode = NULL;
	if (viewer->computeIntersections(x, y, intersections)) {
		int i = 0;
		for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr =
				intersections.begin(); hitr != intersections.end(); hitr++, i++) {
			if (hitr->nodePath.size() <= 2)
				continue;
			if (pickedNode == NULL)
				pickedNode = getNode(hitr->nodePath);
			if (i == intersections.size() - 1 && mode == NORMAL) {
				osg::Geode* g = dynamic_cast<osg::Geode *> (hitr->nodePath.back());
				if (nodeFrame->activateAction(g))
					return NULL;
			}
		}
	} else {
//		qDebug() << "no intersections";
	}
	return pickedNode;
}

QList<OsgNode*> PickHandler::getNodesInQuad(osgViewer::Viewer* viewer,
		const double xMin, const double yMin, const double xMax,
		const double yMax) {
	QList<OsgNode*> nodes;
	osg::ref_ptr<osgUtil::PolytopeIntersector> picker = new osgUtil::PolytopeIntersector(
			osgUtil::Intersector::WINDOW, xMin, yMin, xMax, yMax);
	osgUtil::IntersectionVisitor iv(picker.get());
	viewer->getCamera()->accept(iv);

	if (picker->containsIntersections()) {
		osgUtil::PolytopeIntersector::Intersections intersections =
				picker->getIntersections();
		for (osgUtil::PolytopeIntersector::Intersections::iterator hitr =
				intersections.begin(); hitr != intersections.end(); hitr++) {
			if (hitr->nodePath.size() <= 2)
				continue;

			OsgNode* n = getNode(hitr->nodePath);
			if (n != NULL && !nodes.contains(n))
				nodes.append(n);
		}
	}
	return nodes;
}


OsgNode* PickHandler::getNode(osg::NodePath nodePath) {
	osg::Geode* g = dynamic_cast<osg::Geode *> (nodePath.back());
	if (g != NULL) {
//		qDebug() << "Picked geode: " << g->getName().c_str();
		OsgNode* node = NULL;
		osg::Group* parent = g->getParent(0);

		osg::NodePath::const_iterator i = nodePath.end() - 1;
		while (node == NULL && i != nodePath.begin()) {
//				qDebug() << ((*i)->getName().c_str());
			node = dynamic_cast<OsgNode* > (*i);
			i--;
		}
		if (node != NULL && node->isPickable(g))
			return node;
	} else {
//		qDebug() << "Picked geode: " << "null";
	}
	return NULL;
}

bool PickHandler::select(OsgNode* node, bool singleOnly) {
	if (node == NULL) {
		if (!selectedNodes.isEmpty() && !multiPickEnabled && !nodeFrame->isActive()) {
			sceneGraph->setFrozen(false);
			deselectAll();
			return true;
		}
		return false;
	}

	if (selectedNodes.contains(node) && !multiPickEnabled) {
		if (!singleOnly || selectedNodes.size() == 1) {
			node->setSelected(true);
			return true;
		}
	}

	if (multiPickEnabled && !singleOnly) {
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
	NodeIterator i = selectedNodes.constBegin();
	while (i != selectedNodes.constEnd()) {
		(*i)->setSelected(false);
		(*i)->setFrozen(false);
		++i;
	}
	selectedNodes.clear();
}

void PickHandler::createSelectionQuad() {
	osg::ref_ptr<osg::StateSet> quadStateSet = new osg::StateSet;
	quadStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	quadStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	quadStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	quadStateSet->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	quadStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	quadStateSet->setRenderBinDetails(11, "RenderBin");

	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

	coordinates->push_back(osg::Vec3(0, 1, -1));
	coordinates->push_back(osg::Vec3(0, 1, -1));
	coordinates->push_back(osg::Vec3(0, 1, -1));
	coordinates->push_back(osg::Vec3(0, 1, -1));

	osg::Vec4f color = DataMapping::getDefaultColor(DataMapping::NODE);
	color.w() = 0.1f;
	colors->push_back(color);

	geometry->setVertexArray(coordinates);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	geometry->setColorArray(colors);
	geometry->setStateSet(quadStateSet);
	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);

	selectionQuad = new osg::Geode;
	selectionQuad->addDrawable(geometry);

	osg::Camera* hudCamera = new osg::Camera;
	hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	hudCamera->setProjectionMatrixAsOrtho2D(0,1,0,1);
	hudCamera->setViewMatrix(osg::Matrix::identity());
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	hudCamera->addChild(selectionQuad);

	sceneGraph->addPermanentNode(hudCamera);
	selectionQuad->setNodeMask(0);
}

void PickHandler::initSelectionQuad() {
//	quadProjection->setMatrix(Util::CameraHelper::getProjectionMatrix());
//	quadProjection->setNodeMask(1);

	osg::Camera* cameraHud = dynamic_cast<osg::Camera*>(selectionQuad->getParent(0));
	if (cameraHud != NULL) {
		cameraHud->setProjectionMatrix(Util::CameraHelper::getProjectionMatrix());
		selectionQuad->setNodeMask(1);
	}
}

void PickHandler::drawSelectionQuad() {
	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	coordinates->push_back(osg::Vec3(originPos.x(), originPos.y(), -1));
	coordinates->push_back(osg::Vec3(originPos.x(), lastPos.y(), -1));
	coordinates->push_back(osg::Vec3(lastPos.x(), lastPos.y(), -1));
	coordinates->push_back(osg::Vec3(lastPos.x(), originPos.y(), -1));

	selectionQuad->getDrawable(0)->asGeometry()->setVertexArray(coordinates);
}

QList<OsgNode* > PickHandler::getSelectedNodes() const {
	return selectedNodes;
}

osg::Vec3 PickHandler::getSelectionCenter() const {
	osg::Vec3f sum(0, 0, 0);

	NodeIterator i = selectedNodes.constBegin();
	while (i != selectedNodes.constEnd()) {
		sum += (*i)->getPosition();
		++i;
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
		//		qDebug() << "SHIFT";
		return true;
	}
	return false;
}

bool PickHandler::isCtrl(const osgGA::GUIEventAdapter& event) {
	int key = event.getModKeyMask();
	if (key & osgGA::GUIEventAdapter::MODKEY_CTRL) {
		//		StatusLogger::log(StatusLogger::KEYS, "CTRL");
		//		qDebug() << "CTRL";
		return true;
	}
	return false;
}

bool PickHandler::isAlt(const osgGA::GUIEventAdapter& event) {
	int key = event.getModKeyMask();
	if (key & osgGA::GUIEventAdapter::MODKEY_ALT) {
		//		StatusLogger::log(StatusLogger::KEYS, "CTRL");
		//		qDebug() << "ALT";
		return true;
	}
	return false;
}

bool PickHandler::isKey(char key, const osgGA::GUIEventAdapter& event) {
	//	qDebug() << event.getKey();
	//	qDebug() << event.getModKeyMask();
	if (event.getKey() & key) {
		return true;
	}
	return false;
}

void PickHandler::windowResized() {
	nodeFrame->updateProjection();
}

