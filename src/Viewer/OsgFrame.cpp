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
#include <stdlib.h>
#include <QDebug>

using namespace Vwr;

OsgFrame::OsgFrame() {
	myNode = NULL;

	nullButton = new NullButton(this);
	activeButton = nullButton;

	originPos = osg::Vec2f(0.0, 0.0);
	currentPos = osg::Vec2f(0.0, 0.0);
	lastDragPos = osg::Vec2f(0.0, 0.0);

	mt = new osg::AutoTransform();
	mt2 = new osg::AutoTransform();

	FrameButton* mb = new MoveButton(this, osg::Vec3f(30, -20, 2));
	FrameButton* eb = new ExpandButton(this, osg::Vec3f(30, -70, 2));
	FrameButton* cb = new CompactButton(this, osg::Vec3f(30, -120, 2));
	FrameButton* hb = new HideButton(this, osg::Vec3f(30, 30, 2));
	FrameButton* rb = new ResizeButton(this, osg::Vec3f(-20, 30, 2));
	FrameButton* fb = new FixButton(this, osg::Vec3f(-30, -30, 2));
	buttons.insert(mb->getName(), mb);
	buttons.insert(eb->getName(), eb);
	buttons.insert(cb->getName(), cb);
	buttons.insert(hb->getName(), hb);
	buttons.insert(rb->getName(), rb);
	buttons.insert(fb->getName(), fb);
	mt->addChild(mb);
	mt->addChild(eb);
	mt->addChild(cb);
	mt->addChild(hb);
	mt->addChild(rb);
	mt2->addChild(fb);

//	mt->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
//	mt2->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
//	mt->setAutoScaleToScreen(true);
//	mt2->setAutoScaleToScreen(true);
//	mt->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
//	mt2->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	addChild(mt);
	addChild(mt2);

	setNodeMask(false);


//	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
//	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
//	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
//
//	coordinates->push_back(osg::Vec3(100, 100, 0));
//	coordinates->push_back(osg::Vec3(100, -100, 0));
//	coordinates->push_back(osg::Vec3(-100, -100, 0));
//	coordinates->push_back(osg::Vec3(-100, 100, 0));
//	coordinates->push_back(osg::Vec3(100, 100, 0));
//
//	colors->push_back(osg::Vec4(1, 1, 1, 1));
//
//	geometry->setVertexArray(coordinates);
//	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 5));
//	geometry->setColorArray(colors);
////	geometry->setStateSet(quadStateSet);
//	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
//
//	rect = new osg::Geode;
//	rect->addDrawable(geometry);
//
//	addChild(rect);
}

OsgFrame::~OsgFrame() {
	setNode(NULL);
}

void OsgFrame::show() {
	if (!isNodeSet())
		return;
	qDebug() << "show";

	updatePosition();
	setNodeMask(true);
}

void OsgFrame::hide() {
	qDebug() << "hide";

	setNodeMask(false);
	if (isActive())
		deactivateAction();
}

void OsgFrame::setNode(AbstractNode* node) {
	if (node == myNode) {
		qDebug() << "already framed";
		return;
	}
	qDebug() << "set";

	if (myNode != NULL) {
		myNode->setSelected(false);
		disconnect(myNode, 0 ,this, 0);
	}

	if (node != NULL) {
		node->setSelected(true);
		node->setExpanded(true);
		connect(node, SIGNAL(changedPosition(osg::Vec3f, osg::Vec3f)),
				this, SLOT(myNodePosChanged(osg::Vec3f, osg::Vec3f)));
		connect(node, SIGNAL(changedSize(osg::Vec3f, osg::Vec3f)),
				this, SLOT(myNodeSizeChanged(osg::Vec3f, osg::Vec3f)));
		connect(node, SIGNAL(nodeAdded(AbstractNode*)),
				this, SLOT(nodeAdded(AbstractNode*)));
		connect(node, SIGNAL(nodeRemoved(AbstractNode*)),
				this, SLOT(nodeRemoved(AbstractNode*)));
	}
	myNode = node;

	deactivateAction();
	updatePosition();
}

bool OsgFrame::isNodeSet() {
	return myNode != NULL;
}

void OsgFrame::updatePosition() {
	if (!isNodeSet())
		return;
	osg::Vec3f v = Util::CameraHelper::byFull(myNode->getPosition());
	v.z() = 0.0f;
	setPosition(v);

	osg::Vec3f size = myNode->getSize() / 2.0f;
	size.z() = 0.0f;
//	setPosition(myNode->getPosition());
	mt->setPosition(mt->getRotation() * size);
//	mt->setPosition(v);
	mt2->setPosition(mt2->getRotation() * (-size));


//	Util::CameraHelper::printVec(size);

//	osg::Vec3f a = Util::CameraHelper::byView(myNode->getPosition());
//	osg::Vec3f b = Util::CameraHelper::byView(myNode->getPosition());
//	a += osg::Vec3f(size.x(), size.y(), size.z());
//	b += osg::Vec3f(-size.x(), -size.y(), size.z());
//	a = Util::CameraHelper::byWindow(Util::CameraHelper::byProjection(a));
//	b = Util::CameraHelper::byWindow(Util::CameraHelper::byProjection(b));

//	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
//	coordinates->push_back(osg::Vec3(a.x(), a.y(), 0));
//	coordinates->push_back(osg::Vec3(a.x(), b.y(), 0));
//	coordinates->push_back(osg::Vec3(b.x(), b.y(), 0));
//	coordinates->push_back(osg::Vec3(b.x(), a.y(), 0));
//	coordinates->push_back(osg::Vec3(a.x(), a.y(), 0));
//
//	rect->getDrawable(0)->asGeometry()->setVertexArray(coordinates);
}

void OsgFrame::updateProjection() {
	osg::Camera* cameraHud = dynamic_cast<osg::Camera*>(getParent(0));
	if (cameraHud != NULL) {
		cameraHud->setProjectionMatrix(Util::CameraHelper::getProjectionMatrix());
	}
	updatePosition();
}

void OsgFrame::activateAction(osg::Geode* button) {
	FrameButton* newButton;
	if (!isNodeSet() || button == NULL) {
		newButton = nullButton;
	} else {
		newButton = buttons.value(button->getName(), nullButton);
	}

	if (newButton != activeButton) {
		activeButton->deactivate();
		activeButton = newButton;
		activeButton->activate();
	}
	qDebug() << "act:" << activeButton->getName().c_str();
}

void OsgFrame::deactivateAction() {
	qDebug() << "deact: null_button";
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

void OsgFrame::myNodePosChanged(osg::Vec3f oldPos, osg::Vec3f newPos) {
	updatePosition();
}

void OsgFrame::myNodeSizeChanged(osg::Vec3f oldSize, osg::Vec3f newSize) {
	updatePosition();
}

