/*
 * ControlFrame.cpp
 *
 *  Created on: 4.1.2011
 *      Author: jakub
 */

#include "Viewer/ControlFrame.h"
#include "Viewer/AbstractNode.h"
#include "Viewer/OsgNodeGroup.h"
#include "Viewer/BasicButtons.h"
#include "Util/TextureWrapper.h"
#include "Util/CameraHelper.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Projection>
#include <osg/BlendFunc>

#include <stdlib.h>
#include <QDebug>

using namespace Vwr;

osg::Vec2f ControlFrame::MIN_SIZE = osg::Vec2f(60, 60);

ControlFrame::ControlFrame(SceneGraph* sceneGraph) {
	this->sceneGraph = sceneGraph;
	myNode = NULL;

	originPos = osg::Vec2f(0.0, 0.0);
	currentPos = osg::Vec2f(0.0, 0.0);
	lastDragPos = osg::Vec2f(0.0, 0.0);

	createButtons();
	createBorder();
	activeButton = nullButton;

	setNodeMask(false);
}

ControlFrame::~ControlFrame() {
	if (myNode != NULL)
		disconnect(myNode, 0 ,this, 0);
//	qDebug() << "Frame deleted";
}

void ControlFrame::createButtons() {
	nullButton = new NullButton(this);

	osg::ref_ptr<FrameButton> hb = new HideButton(this, 0, 0);
	osg::ref_ptr<FrameButton> rb = new ResizeButton(this, -1, 0);
	osg::ref_ptr<FrameButton> mb = new MoveButton(this, -2, 0);
	osg::ref_ptr<FrameButton> eb = new ExpandButton(this, 0, -1);
	osg::ref_ptr<FrameButton> cb = new CompactButton(this, 1, -1);
	osg::ref_ptr<FrameButton> fb = new FixButton(this, -1, -1);
	osg::ref_ptr<FrameButton> xb = new XRayButton(this, 0, -2);
	osg::ref_ptr<FrameButton> uclb = new UnclusterButton(this, -2, 0);
	osg::ref_ptr<FrameButton> clb = new ClusterButton(this, -1, 0);

	mt = new osg::AutoTransform();
	mt2 = new osg::AutoTransform();
	insertButton(hb, mt);
	insertButton(rb, mt);
	insertButton(mb, mt);
	insertButton(eb, mt);
	insertButton(cb, mt);
	insertButton(clb, mt2);
	insertButton(uclb, mt2);
	insertButton(fb, mt2);
	insertButton(xb, mt);

	addChild(mt);
	addChild(mt2);
}

void ControlFrame::insertButton(osg::ref_ptr<FrameButton> button, osg::Transform* tf) {
	buttons.insert(button->getName(), button);
	tf->addChild(button);
}

void ControlFrame::createBorder() {
	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

	coordinates->push_back(osg::Vec3(1, 1, 0));
	coordinates->push_back(osg::Vec3(1, -1, 0));
	coordinates->push_back(osg::Vec3(-1, -1, 0));
	coordinates->push_back(osg::Vec3(-1, 1, 0));

	colors->push_back(osg::Vec4(1, 0, 0, 1));

	geometry->setVertexArray(coordinates);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);

	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	rect = new osg::Geode;
	rect->setName("frame_border");
	rect->addDrawable(geometry);
	rect->setStateSet(stateSet);

	addChild(rect);
}

void ControlFrame::updateButtons(AbstractNode* node, bool nodeAdded) {
	return;
	// TODO how to call this when node state changes?
	// TODO how to handle groups, additions and removals?

	if (node == NULL)
		return;
	buttons.value(ExpandButton::name())->setEnabled(!node->isExpanded());
	buttons.value(CompactButton::name())->setEnabled(node->isExpanded());
	buttons.value(XRayButton::name())->setEnabled(node->isExpanded());
	buttons.value(ResizeButton::name())->setEnabled(node->isExpanded());

	buttons.value(ClusterButton::name())->setEnabled(node->isClusterable());
//	buttons.value(UnclusterButton::name())->setEnabled(node->hasParent());
	// TODO
}

void ControlFrame::show() {
	if (!isNodeSet())
		return;
//	qDebug() << "show";
	setNodeMask(true);
}

void ControlFrame::hide() {
//	qDebug() << "hide";
	setNodeMask(false);
	if (isActive())
		deactivateAction();
}

AbstractNode* ControlFrame::getNode() const {
	return myNode;
}

void ControlFrame::addNode(AbstractNode* node) {
	if (node == NULL)
		return;
	OsgNodeGroup* myGroup = dynamic_cast<OsgNodeGroup*>(myNode);
	if (myGroup != NULL) {
		myGroup->addNode(node, true);
	} else {
		setNode(Vwr::OsgNodeGroup::merge(node, myNode));
	}
}

void ControlFrame::setNode(AbstractNode* node) {
	if (node == myNode) {
//		qDebug() << "already framed";
		return;
	}
//	qDebug() << "setting";

	if (myNode != NULL) {
		disconnect(myNode, 0 ,this, 0);
		myNode->setSelected(false);
		myNode = NULL;
	}
	if (node != NULL) {
		node->setSelected(true);
//		node->setExpanded(true);
		OsgNodeGroup* group = dynamic_cast<OsgNodeGroup*>(node);
		if (group != NULL) {
			connect(group, SIGNAL(nodeAdded(AbstractNode*)),
					this, SLOT(nodeAdded(AbstractNode*)));
			connect(group, SIGNAL(nodeRemoved(AbstractNode*)),
					this, SLOT(nodeRemoved(AbstractNode*)));
		}
	}
	myNode = node;
	updateButtons(node);
	deactivateAction();
//	qDebug() << "Node set " << (myNode ? myNode->toString() : "NULL");
}

bool ControlFrame::isNodeSet() const {
	return myNode != NULL;
}

bool ControlFrame::isActive() const {
	return activeButton != nullButton;
}

void ControlFrame::updateGeometry() {
	if (!isNodeSet())
		return;

	float xMin, yMin, xMax, yMax;
	myNode->getProjRect(xMin, yMin, xMax, yMax);

	float d = 0;
	if ((d = xMax-xMin) < MIN_SIZE.x()) {
		xMax += (MIN_SIZE.x()-d)/2.0f;
		xMin -= (MIN_SIZE.x()-d)/2.0f;
	}
	if ((d = yMax-yMin) < MIN_SIZE.y()) {
		yMax += (MIN_SIZE.y()-d)/2.0f;
		yMin -= (MIN_SIZE.y()-d)/2.0f;
	}

	mt->setPosition(osg::Vec3f(xMax, yMax, 0));
	mt2->setPosition(osg::Vec3f(xMin, yMin, 0));

	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	coordinates->push_back(osg::Vec3(xMax, yMax, 0));
	coordinates->push_back(osg::Vec3(xMax, yMin, 0));
	coordinates->push_back(osg::Vec3(xMin, yMin, 0));
	coordinates->push_back(osg::Vec3(xMin, yMax, 0));

	rect->getDrawable(0)->asGeometry()->setVertexArray(coordinates);
}

void ControlFrame::updateProjection() {
	osg::Camera* cameraHud = dynamic_cast<osg::Camera*>(getParent(0));
	if (cameraHud != NULL) {
		cameraHud->setProjectionMatrix(Util::CameraHelper::getProjectionMatrix());
	}
}

bool ControlFrame::activateAction(osg::Geode* button) {
	FrameButton* newButton;
	if (!isNodeSet() || button == NULL) {
		newButton = nullButton;
	} else {
		newButton = buttons.value(button->getName(), nullButton);
	}

	if (newButton == nullButton) {
		deactivateAction();
	} else if (newButton != activeButton) {
//		activeButton->deactivate();
		activeButton = newButton;
//		activeButton->activate();
	}
//	qDebug() << "act:" << activeButton->getName().c_str();
	return isActive();
}

void ControlFrame::deactivateAction() {
//	qDebug() << "deact: " << activeButton->getName().c_str();
	activeButton->deactivate();
	activeButton = nullButton;
}

bool ControlFrame::handlePush(const osgGA::GUIEventAdapter& event) {
//	qDebug() << "push";
	if (isNodeSet() && activeButton->isEnabled()) {
		originPos.set(event.getX(), event.getY());
		currentPos.set(originPos.x(), originPos.y());
		lastDragPos.set(originPos.x(), originPos.y());
		activeButton->handlePush();
		return true;
	} else {
		return false;
	}
}

bool ControlFrame::handleDrag(const osgGA::GUIEventAdapter& event) {
	if (isNodeSet() && activeButton->isEnabled()) {
		currentPos.set(event.getX(), event.getY());
		activeButton->handleDrag();
		lastDragPos.set(currentPos.x(), currentPos.y());
		return true;
	} else {
		return false;
	}
}

bool ControlFrame::handleRelease(const osgGA::GUIEventAdapter& event) {
//	qDebug() << "release";
	if (isNodeSet() && activeButton->isEnabled()) {
		currentPos.set(event.getX(), event.getY());
		activeButton->handleRelease();
		return true;
	} else {
		return false;
	}
}

void ControlFrame::nodeAdded(AbstractNode* node) {
//	qDebug() << "node " << node->toString() << " added to ControlFrame";
	node->setSelected(true);
	updateButtons(node, true);
}

void ControlFrame::nodeRemoved(AbstractNode* node) {
//	qDebug() << "node " << node->toString() << " removed from ControlFrame";
	node->setSelected(false);
	updateButtons(myNode);
}

