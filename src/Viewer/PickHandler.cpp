/*
 * PickHandler.cpp
 *
 *  Created on: 25.10.2010
 */

#include "Viewer/PickHandler.h"
#include "Viewer/SceneGraph.h"
#include "Viewer/CameraManipulator.h"
#include "Window/CoreWindow.h"
#include "Util/DataHelper.h"
#include "Util/Config.h"
#include <iostream>

#include <osg/MatrixTransform>
#include <osg/Projection>

using namespace Vwr;

PickHandler::PickHandler(Vwr::CameraManipulator * cameraManipulator,
		Vwr::SceneGraph * coreGraph) {
	//vytvorenie timera a vynulovanie premennych
	//	timer = new QTimer();
	//	connect(timer, SIGNAL(timeout()), this, SLOT(mouseTimerTimeout()));
	//	pushEvent = NULL;
	//	pushAction = NULL;
	//	releaseEvent = NULL;
	//	releaseAction = NULL;

	this->cameraManipulator = cameraManipulator; // XXX why do we need this?
	this->coreGraph = coreGraph;

	originPos = osg::Vec2f(0.0, 0.0);
	lastPos = osg::Vec2f(0.0, 0.0);

	isDrawingSelectionQuad = false;
	isManipulatingNodes = false;
	mode = NONE;
	selectionType = NODE;

	multiPickEnabled = false;
	createSelectionQuad();
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	switch (event.getEventType()) {

	case osgGA::GUIEventAdapter::PUSH: {
		qDebug() << "Mouse pressed";
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
		qDebug() << "Mouse released";
		//ak je release a je timer aktivny tak sa ulozi event a nevyvola sa
		//		if (timer->isActive()) {
		//			releaseEvent = &event;
		//			releaseAction = &action;
		//			return false; // events not handled here! (see mouseTimerTimeout())
		//		}
		return handleRelease(event, action);
	}

		//	case osgGA::GUIEventAdapter::KEYDOWN: {
		//		return handleKeyDown(event, action);
		//	}
		//
		//	case ::osgGA::GUIEventAdapter::KEYUP: {
		//		return handleKeyUp(event, action);
		//	}

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
	qDebug() << " push handled";

	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		return false;

	originPos.set(event.getX(), event.getY());
	originNormPos.set(event.getXnormalized(), event.getYnormalized());
	Model::Node* node = NULL;
	bool ret;
	NodeList::const_iterator i;

	switch (mode) {
	case NONE:
		node = pickOne(event, action);
		if (node != NULL) {
			node->toggleExpanded();
			return true;
		}
		return false;
	case SELECT:
		multiPickEnabled = isShift(event);

		ret = select(pickOne(event, action));

		i = selectedNodes.constBegin();
		while (i != selectedNodes.constEnd()) {
			(*i)->setFrozen(true);
			++i;
		}

		if (selectedNodes.isEmpty() || multiPickEnabled) {
			// drawing quad might follow
		}
		return ret;
	default:
		return false;
	}
}

bool PickHandler::handleRelease(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {

	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		return false;
	qDebug() << " release handled\n";

	//	osgViewer::Viewer* viewer = getViewer(action);
	//	if (!viewer)
	//		return false;
	originPos.set(event.getX(), event.getY());

	if (isDrawingSelectionQuad) {
		isDrawingSelectionQuad = false;
		projection->setNodeMask(0);

		if (!multiPickEnabled)
			deselectAll();
		QList<Model::Node*> nodes = pickMore(event, action);
		QList<Model::Node*>::const_iterator ni = nodes.constBegin();
		multiPickEnabled = true; // temp to select form quad
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

	return true;
}

bool PickHandler::handleDoubleclick(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		return false;
	qDebug() << "doubleClick";
	//	Model::Node* node = pick(event, action);
	//	if (node != NULL) {
	//		node->toggleExpanded();
	//		return true;
	//	}
	return false;
}

bool PickHandler::handleMove(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	if (event.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		return false;
	//	lastPos.set(event.getX(), event.getY());
	return false;
}

bool PickHandler::handleDrag(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	if (event.getButtonMask() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		return false;
	//	qDebug() << "dragged";
	lastPos.set(event.getX(), event.getY());

	switch (mode) {
	case NONE:
		return false;
	case SELECT:
		if (!selectedNodes.isEmpty() && !multiPickEnabled) { // drag node(s)
			NodeList::const_iterator i = selectedNodes.constBegin();

			while (i != selectedNodes.constEnd()) {
				(*i)->setTargetPosition(//todo current
						getMousePos((*i)->getTargetPosition(),
								getViewer(action)));
				++i;
			}
			originPos.set(lastPos.x(), lastPos.y());
			coreGraph->setFrozen(false);
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
	default:
		return false;
	}
	return false;
}

osg::Vec3f PickHandler::getMousePos(osg::Vec3f origPos,
		osgViewer::Viewer* viewer) {
	osg::Camera* cam = viewer->getCamera();
	osg::Matrixd& viewM = cam->getViewMatrix();
	osg::Matrixd& projM = cam->getProjectionMatrix();
	osg::Matrixd screenM = cam->getViewport()->computeWindowMatrix();
	osg::Matrixd compositeM = viewM * projM * screenM;
	osg::Matrixd compositeMi = compositeMi.inverse(compositeM);

	float
			scale =
					Util::Config::getValue("Viewer.Display.NodeDistanceScale").toFloat();

	osg::Vec3f screenPoint = origPos * compositeM; // TODO change to currnet pos
	osg::Vec3f newPosition = osg::Vec3f(screenPoint.x() - (originPos.x()
			- lastPos.x()) / scale, screenPoint.y() - (originPos.y()
			- lastPos.y()) / scale, screenPoint.z());

	return newPosition * compositeMi;
}

Model::Node* PickHandler::pickOne(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	osgViewer::Viewer* viewer = getViewer(action);
	Model::Node* pickedNode = getNodeAt(viewer, event.getXnormalized(),
			event.getYnormalized());// todo change to local
	if (pickedNode == NULL)
		qDebug() << "NO PICK";
	else {
		qDebug() << "NODE PICKED";
	}
	return pickedNode;
}

QList<Model::Node*> PickHandler::pickMore(const osgGA::GUIEventAdapter& event,
		osgGA::GUIActionAdapter& action) {
	osgViewer::Viewer* viewer = getViewer(action);

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

	QList<Model::Node*> pickedNodes = getNodesInQuad(viewer, x, y, w, h);

	if (pickedNodes.isEmpty())
		qDebug() << "NO PICK";
	else {
		qDebug() << "NODES PICKED";
	}
	return pickedNodes;
}

Model::Node* PickHandler::getNodeAt(osgViewer::Viewer* viewer, const double nX,
		const double nY) {
	double m = 0.00005f;
	if (!viewer->getSceneData()) {
		return NULL; // Nothing to pick.
	}
	// picking node at point means picking in very small quad
	// some form of ray oriented picking should be used instead
	QList<Model::Node*> nodes = getNodesInQuad(viewer, nX - m, nY - m, nX + m,
			nY + m);
	if (!nodes.isEmpty()) {
		return nodes.first();
	}
	return NULL;
}

QList<Model::Node*> PickHandler::getNodesInQuad(osgViewer::Viewer* viewer,
		const double xMin, const double yMin, const double xMax,
		const double yMax) {
	QList<Model::Node*> nodes;
	if (!viewer->getSceneData()) {
		return nodes; // Nothing to pick.
	}
	osgUtil::PolytopeIntersector* picker = new osgUtil::PolytopeIntersector(
			osgUtil::Intersector::PROJECTION, xMin, yMin, xMax, yMax);
	osgUtil::IntersectionVisitor iv(picker);
	viewer->getCamera()->accept(iv);

	if (picker->containsIntersections()) {
		osgUtil::PolytopeIntersector::Intersections intersections =
				picker->getIntersections();
		for (osgUtil::PolytopeIntersector::Intersections::iterator hitr =
				intersections.begin(); hitr != intersections.end(); hitr++) {
			if (hitr->nodePath.empty())
				continue;
			osg::NodePath nodePath = hitr->nodePath;
			if (nodePath.size() <= 1)
				continue;
			Model::Node* n =
					dynamic_cast<Model::Node *> (nodePath[nodePath.size() - 1]);
			nodes.append(n);
		}
	}
	return nodes;
}

bool PickHandler::select(Model::Node* node) {
	if (node == NULL) {
		if (!selectedNodes.isEmpty() && !multiPickEnabled) {
			coreGraph->setFrozen(false);
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
			qDebug() << " one deselected";
			return true;
		} else { // add to nodes
			selectedNodes.append(node);
		}
	} else {
		deselectAll();
		selectedNodes.append(node);
	}

	qDebug() << " one selected";
	node->setSelected(true);
	return true;
}

void PickHandler::deselectAll() {
	NodeList::const_iterator i = selectedNodes.constBegin();

	while (i != selectedNodes.constEnd()) {
		(*i)->setSelected(false);
		(*i)->setFrozen(false);
		qDebug() << " all deselected";
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
//			Model::Edge * e =
//					dynamic_cast<Model::Edge *> (geometry->getPrimitiveSet(
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

	coreGraph->getCustomNodeList()->push_back(group);
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

osg::Vec3 PickHandler::getSelectionCenter(bool nodesOnly) {
	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	float
			scale =
					Util::Config::getValue("Viewer.Display.NodeDistanceScale").toFloat();

	//	if (!nodesOnly) {
	//		QLinkedList<osg::ref_ptr<Model::Edge> >::const_iterator ei =
	//				pickedEdges.constBegin();
	//
	//		while (ei != pickedEdges.constEnd()) {
	//			coordinates->push_back(Util::DataHelper::getMassCenter(
	//					(*ei)->getCooridnates()));
	//			++ei;
	//		}
	//	}

	NodeList::const_iterator ni = selectedNodes.constBegin();

	while (ni != selectedNodes.constEnd()) {
		coordinates->push_back((*ni)->getTargetPosition());
		++ni;
	}

	osg::Vec3 center;

	if (coordinates->size() > 0)
		center = Util::DataHelper::getMassCenter(coordinates);

	return center;
}

bool PickHandler::isShift(const osgGA::GUIEventAdapter& event) {
	int key = event.getModKeyMask();
	qDebug() << key;
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

