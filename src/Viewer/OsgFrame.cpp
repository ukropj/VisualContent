/*
 * OsgFrame.cpp
 *
 *  Created on: 4.1.2011
 *      Author: jakub
 */

#include "Viewer/OsgFrame.h"
#include "Viewer/AbstractNode.h"
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

OsgFrame::OsgFrame() {
	myNode = NULL;

	originPos = osg::Vec2f(0.0, 0.0);
	currentPos = osg::Vec2f(0.0, 0.0);
	lastDragPos = osg::Vec2f(0.0, 0.0);

	createButtons();
	createBorder();
	activeButton = nullButton;

	setNodeMask(false);
}

OsgFrame::~OsgFrame() {
	if (myNode != NULL)
		disconnect(myNode, 0 ,this, 0);
//	qDebug() << "Frame deleted";
}

void OsgFrame::createButtons() {
	nullButton = new NullButton(this);

	osg::ref_ptr<FrameButton> mb = new MoveButton(this, osg::Vec3f(30, -20, 2));
	osg::ref_ptr<FrameButton> eb = new ExpandButton(this, osg::Vec3f(30, -120, 2));
	osg::ref_ptr<FrameButton> cb = new CompactButton(this, osg::Vec3f(30, -70, 2));
	osg::ref_ptr<FrameButton> hb = new HideButton(this, osg::Vec3f(30, 30, 2));
	osg::ref_ptr<FrameButton> rb = new ResizeButton(this, osg::Vec3f(-20, 30, 2));
	osg::ref_ptr<FrameButton> fb = new FixButton(this, osg::Vec3f(-30, -30, 2));

	mt = new osg::AutoTransform();
	mt2 = new osg::AutoTransform();
	insertButton(mb, mt);
	insertButton(eb, mt);
	insertButton(cb, mt);
	insertButton(hb, mt);
	insertButton(rb, mt);
	insertButton(fb, mt2);

//	mt->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
//	mt2->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
//	mt->setAutoScaleToScreen(true);
//	mt2->setAutoScaleToScreen(true);

	addChild(mt);
	addChild(mt2);
}

void OsgFrame::insertButton(osg::ref_ptr<FrameButton> button, osg::Transform* tf) {
	buttons.insert(button->getName(), button);
	tf->addChild(button);
}

void OsgFrame::createBorder() {
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

void OsgFrame::show() {
	if (!isNodeSet())
		return;
//	qDebug() << "show";
	setNodeMask(true);
}

void OsgFrame::hide() {
//	qDebug() << "hide";
	setNodeMask(false);
	if (isActive())
		deactivateAction();
}

AbstractNode* OsgFrame::getNode() const {
	return myNode;
}

void OsgFrame::setNode(AbstractNode* node) {
	if (node == myNode) {
//		qDebug() << "already framed";
		return;
	}
//	qDebug() << "setting";

	if (myNode != NULL) {
		myNode->setSelected(false);
		disconnect(myNode, 0 ,this, 0);
	}
	if (node != NULL) {
		node->setSelected(true);
		node->setExpanded(true);
		connect(node, SIGNAL(nodeAdded(AbstractNode*)),
				this, SLOT(nodeAdded(AbstractNode*)));
		connect(node, SIGNAL(nodeRemoved(AbstractNode*)),
				this, SLOT(nodeRemoved(AbstractNode*)));
	}
	myNode = node;
	deactivateAction();
}

bool OsgFrame::isNodeSet() const {
	return myNode != NULL;
}

bool OsgFrame::isActive() const {
	return activeButton != nullButton;
}

void OsgFrame::updateGeometry() {
	if (!isNodeSet())
		return;

	float xMin, yMin, xMax, yMax;
	myNode->getProjRect(xMin, yMin, xMax, yMax);

	mt->setPosition(osg::Vec3f(xMax, yMax, 0));
	mt2->setPosition(osg::Vec3f(xMin, yMin, 0));

	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	coordinates->push_back(osg::Vec3(xMax, yMax, 0));
	coordinates->push_back(osg::Vec3(xMax, yMin, 0));
	coordinates->push_back(osg::Vec3(xMin, yMin, 0));
	coordinates->push_back(osg::Vec3(xMin, yMax, 0));

	rect->getDrawable(0)->asGeometry()->setVertexArray(coordinates);
}

void OsgFrame::updateProjection() {
	osg::Camera* cameraHud = dynamic_cast<osg::Camera*>(getParent(0));
	if (cameraHud != NULL) {
		cameraHud->setProjectionMatrix(Util::CameraHelper::getProjectionMatrix());
	}
}

bool OsgFrame::activateAction(osg::Geode* button) {
	FrameButton* newButton;
	if (!isNodeSet() || button == NULL) {
		newButton = nullButton;
	} else {
		newButton = buttons.value(button->getName(), nullButton);
	}

	if (newButton == nullButton) {
		deactivateAction();
	} else if (newButton != activeButton) {
		activeButton->deactivate();
		activeButton = newButton;
		activeButton->activate();
	}
//	qDebug() << "act:" << activeButton->getName().c_str();
	return isActive();
}

void OsgFrame::deactivateAction() {
	//qDebug() << "deact: null_button";
	activeButton->deactivate();
	activeButton = nullButton;
}

bool OsgFrame::handlePush(const osgGA::GUIEventAdapter& event) {
	if (isNodeSet()) {
		originPos.set(event.getX(), event.getY());
		currentPos.set(originPos.x(), originPos.y());
		lastDragPos.set(originPos.x(), originPos.y());
		activeButton->handlePush();
		return true;
	} else {
		return false;
	}
}

bool OsgFrame::handleDrag(const osgGA::GUIEventAdapter& event) {
	if (isNodeSet()) {
		currentPos.set(event.getX(), event.getY());
		activeButton->handleDrag();
		lastDragPos.set(currentPos.x(), currentPos.y());
		return true;
	} else {
		return false;
	}
}

bool OsgFrame::handleRelease(const osgGA::GUIEventAdapter& event) {
	if (isNodeSet()) {
		currentPos.set(event.getX(), event.getY());
		activeButton->handleRelease();
		return true;
	} else {
		return false;
	}
}

void OsgFrame::nodeAdded(AbstractNode* node) {
	node->setExpanded(true);
	node->setSelected(true);
}

void OsgFrame::nodeRemoved(AbstractNode* node) {
	node->setSelected(false);
}
