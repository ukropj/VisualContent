/*
 * OsgFrame.cpp
 *
 *  Created on: 4.1.2011
 *      Author: jakub
 */

#include "Viewer/OsgFrame.h"
#include "Viewer/OsgNode.h"
#include "Util/TextureWrapper.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <stdlib.h>
#include <QDebug>

using namespace Vwr;

OsgFrame::OsgFrame() {
	refNode = NULL;
	currentAction = NONE;
	originPos = osg::Vec2f(0.0, 0.0);
	lastPos = osg::Vec2f(0.0, 0.0);

	mt = new osg::AutoTransform();
	mt2 = new osg::AutoTransform();

	mt->addChild(createButton(HIDE, osg::Vec3f(30, 30, 2), "b_close.png"));
	mt->addChild(createButton(MOVE, osg::Vec3f(30, -20, 2), "b_move.png"));
	mt->addChild(createButton(RESIZE, osg::Vec3f(-20, 30, 2), "b_resize.png"));
	mt2->addChild(createButton(FIX, osg::Vec3f(-30, -30, 2), "b_lock.png"));
	addChild(mt);
	addChild(mt2);

	mt->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	mt->setAutoScaleToScreen(true);
	mt2->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	mt2->setAutoScaleToScreen(true);
	setNodeMask(false);
}

OsgFrame::~OsgFrame() {
}

osg::ref_ptr<osg::Geode> OsgFrame::createButton(ButtonType type, osg::Vec3f pos, QString imagePath) {
	float width = 40;
	float height = 40;

	osg::Geometry* g = osg::createTexturedQuadGeometry(
			osg::Vec3(pos.x()-width/2.0f, pos.y()-height/2.0f, 0),
			osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
	g->setUseDisplayList(false);

	osg::ref_ptr<osg::Texture2D> texture =
			Util::TextureWrapper::readTextureFromFile("img/texture/"+imagePath, false);
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;
	stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(10, "DepthSortedBin");

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	stateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(g);
	geode->setName(QString("%1").arg(type).toStdString());
	geode->setStateSet(stateSet);

	return geode;
}

void OsgFrame::show(OsgNode* node) {
	if (node == NULL || node->equals(refNode))
		return;
	if (refNode != NULL) {
		refNode->setFrame(NULL);
	}
	currentAction = NONE;
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
	qDebug() << "BUTTON: " << QString(button->getName().c_str());
	int type = atoi(button->getName().c_str());
	switch (type) {
	case MOVE:
		currentAction = MOVE;qDebug() << "MOVE";
		break;
	case RESIZE:
		currentAction = RESIZE;qDebug() << "RESIZE";
		break;
	case HIDE:
		currentAction = HIDE;qDebug() << "HIDE";
		break;
	default:
		deactivateAction();
		return false;
	}
	return true;
}

void OsgFrame::deactivateAction() {
	currentAction = NONE;qDebug() << "NONE";
}

bool OsgFrame::handlePush(const osgGA::GUIEventAdapter& event, osgViewer::Viewer* viewer) {
	if (isShowing()) {
		originPos.set(event.getX(), event.getY());
		lastPos.set(originPos.x(), originPos.y());

		switch(currentAction) {
		case HIDE :
			refNode->setExpanded(false);
			hide();
			break;
		case MOVE :
		case RESIZE :
			refNode->setFrozen(true);
			break;
		}

		return true;
	} else {
		return false;
	}
}

bool OsgFrame::handleDrag(const osgGA::GUIEventAdapter& event, osgViewer::Viewer* viewer) {
	if (isShowing()) {
		osg::Vec2f thisPos(event.getX(), event.getY());

		switch(currentAction) {
		case RESIZE	: {
			osg::Vec2f nodePos = toScreenCoordinates(refNode->getPosition(), viewer);
			osg::Vec2f newVect = thisPos - nodePos;
			osg::Vec2f oldVect = lastPos - nodePos;

			refNode->resize(newVect.length() / oldVect.length());
			lastPos.set(thisPos.x(), thisPos.y());
			return true;
		}
		case MOVE : {
			osg::Vec2f dragVect = thisPos - lastPos;

			refNode->setPosition(getMousePos(refNode->getPosition(), dragVect, viewer));

			lastPos.set(thisPos.x(), thisPos.y());
			return true;
		}
		default :
			return false;
		}
	} else {
		return false;
	}
}

bool OsgFrame::handleRelease(const osgGA::GUIEventAdapter& event, osgViewer::Viewer* viewer) {
	if (isShowing()) {
		refNode->setFrozen(false);
		return true;
	} else {
		return false;
	}
}

// TODO move to helper class
osg::Vec2f OsgFrame::toScreenCoordinates(osg::Vec3f scenePos, osgViewer::Viewer* viewer) {
	osg::Camera* camera = viewer->getCamera();
	osg::Matrixd viewM = camera->getViewMatrix();
	osg::Matrixd projM = camera->getProjectionMatrix();
	osg::Matrixd windM = camera->getViewport()->computeWindowMatrix();
	osg::Matrixd compositeM = viewM * projM * windM;
	scenePos = scenePos * compositeM;

	return osg::Vec2f(scenePos.x(), scenePos.y());
}

// TODO move to helper class
osg::Vec3f OsgFrame::getMousePos(osg::Vec3f origPos, osg::Vec2f dragVector,
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
