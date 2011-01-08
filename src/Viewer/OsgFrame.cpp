/*
 * OsgFrame.cpp
 *
 *  Created on: 4.1.2011
 *      Author: jakub
 */

#include "Viewer/OsgFrame.h"
#include "Viewer/OsgNode.h"
#include "Viewer/BasicButtons.h"
#include "Util/TextureWrapper.h"
#include "Util/CameraHelper.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <stdlib.h>
#include <QDebug>

using namespace Vwr;

OsgFrame::OsgFrame() {
	refNode = NULL;
	nullButton = new NullButton(this);
	activeButton = nullButton;
	originPos = osg::Vec2f(0.0, 0.0);
	currentPos = osg::Vec2f(0.0, 0.0);
	lastDragPos = osg::Vec2f(0.0, 0.0);

	mt = new osg::AutoTransform();
	mt2 = new osg::AutoTransform();

	FrameButton* mb = new MoveButton(this, osg::Vec3f(30, -20, 2));
	FrameButton* hb = new HideButton(this, osg::Vec3f(30, 30, 2));
	FrameButton* rb = new ResizeButton(this, osg::Vec3f(-20, 30, 2));
	FrameButton* fb = new FixButton(this, osg::Vec3f(-30, -30, 2));
	buttons.insert(mb->getName(), mb);
	buttons.insert(hb->getName(), hb);
	buttons.insert(rb->getName(), rb);
	buttons.insert(fb->getName(), fb);
	mt->addChild(mb);
	mt->addChild(hb);
	mt->addChild(rb);
	mt2->addChild(fb);

	mt->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	mt2->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	mt->setAutoScaleToScreen(true);
	mt2->setAutoScaleToScreen(true);
	addChild(mt);
	addChild(mt2);

	setNodeMask(false);
}

OsgFrame::~OsgFrame() {
}

void OsgFrame::show(OsgNode* node) {
	if (node == NULL || node->equals(refNode))
		return;
	if (refNode != NULL) {
		refNode->setFrame(NULL);
		deactivateAction();
	}
	refNode = node;
	refNode->setFrame(this);
	node->setExpanded(true);
	updatePosition();
	setNodeMask(true);
}

void OsgFrame::hide() {
	if (refNode != NULL) {
		refNode->setFrame(NULL);
		refNode = NULL;
	}
	setNodeMask(false);
	deactivateAction();
}

void OsgFrame::updatePosition() {
	setPosition(refNode->getPosition());
	mt->setPosition(mt->getRotation() * (refNode->getSize()/2.0f));
	mt2->setPosition(mt2->getRotation() * (refNode->getSize()/-2.0f));
}

bool OsgFrame::activateAction(osg::Geode* button) {
	if (refNode == NULL || button == NULL)
		return false;
	activeButton = buttons.value(button->getName(), nullButton);
	return true;
}

void OsgFrame::deactivateAction() {
	activeButton = nullButton;
}

bool OsgFrame::handlePush(const osgGA::GUIEventAdapter& event) {
	if (isShowing()) {
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
	if (isShowing()) {
		currentPos.set(event.getX(), event.getY());
		activeButton->handleDrag();
		lastDragPos.set(currentPos.x(), currentPos.y());
		return true;
	} else {
		return false;
	}
}

bool OsgFrame::handleRelease(const osgGA::GUIEventAdapter& event) {
	if (isShowing()) {
		currentPos.set(event.getX(), event.getY());
		activeButton->handleRelease();
		return true;
	} else {
		return false;
	}
}
