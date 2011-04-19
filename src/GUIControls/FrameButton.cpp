/*
 * FrameButton.cpp
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#include "GUIControls/FrameButton.h"
#include "GUIControls/ControlFrame.h"
#include "Util/TextureWrapper.h"

#include <QDebug>
#include <osg/CullFace>
#include <osg/BlendFunc>

using namespace Controls;

float FrameButton::BUTTON_SIZE = 40;
float FrameButton::BUTTON_MARGIN = 10;
osg::Vec4f FrameButton::ENABLED_COLOR = osg::Vec4f(1, 1, 1, 1);
osg::Vec4f FrameButton::DISABLED_COLOR = osg::Vec4f(1, 1, 1, 0.5f);
osg::Vec4f FrameButton::SELECTED_COLOR = osg::Vec4f(0.7, 0.7, 1, 1);

FrameButton::FrameButton(ControlFrame* parentFrame) {
	frame = parentFrame;
	enabled = false;
}

FrameButton::FrameButton(ControlFrame* parentFrame, QString imagePath,
		osg::Vec2f relativePos) {
	frame = parentFrame;

	osg::Vec3f buttonPos(BUTTON_MARGIN / 2.0f, BUTTON_MARGIN / 2.0f + BUTTON_SIZE, 0);
	buttonPos.x() += relativePos.x() * (BUTTON_SIZE + BUTTON_MARGIN);
	buttonPos.y() += relativePos.y() * (BUTTON_SIZE + BUTTON_MARGIN);

	createGeometry(imagePath, buttonPos, osg::Vec2f(BUTTON_SIZE, BUTTON_SIZE));
	enabled = true;
}

FrameButton::FrameButton(ControlFrame* parentFrame, QString imagePath,
		osg::Vec3f pos, osg::Vec2f size) {
	frame = parentFrame;
	createGeometry(imagePath, pos, size);
	enabled = true;
}

FrameButton::~FrameButton() {
//	qDebug() << "Button deleted";
}

bool FrameButton::isEnabled() const {
	return enabled;
}

void FrameButton::setEnabled(bool flag) {
	if (enabled == flag)
		return;
	enabled = flag;
	qDebug() << getName().c_str() << "enabled " << enabled;
	if (enabled) {
		setColor(ENABLED_COLOR);
	} else {
		setColor(DISABLED_COLOR);
	}
}

void FrameButton::activate() {
	setColor(SELECTED_COLOR);
}

void FrameButton::deactivate() {
	if (isEnabled()) {
		setColor(ENABLED_COLOR);
	} else {
		setColor(DISABLED_COLOR);
	}
}

void FrameButton::setColor(osg::Vec4f color) {
	osg::Geometry* g = dynamic_cast<osg::Geometry*> (getDrawable(0));
	if (g != NULL) {
		osg::Vec4Array* colorArray =
				dynamic_cast<osg::Vec4Array*> (g->getColorArray());
		colorArray->pop_back();
		colorArray->push_back(color);
	}
}

void FrameButton::createGeometry(QString imagePath, osg::Vec3f pos, osg::Vec2f size) {
	osg::Geometry* g = osg::createTexturedQuadGeometry(
			osg::Vec3(pos.x(), pos.y() - size.y(), pos.z()),
			osg::Vec3(size.x(), 0, 0), osg::Vec3(0, size.y(), 0), 1, 1);
	g->setUseDisplayList(false);

	osg::ref_ptr<osg::Texture2D> texture =
			Util::TextureWrapper::readTextureFromFile(imagePath, false);
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;
	stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF); // OFF
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//	stateSet->setRenderBinDetails(11, "RenderBin");

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	stateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

	addDrawable(g);
	setStateSet(stateSet);
	setColor(ENABLED_COLOR);
}
