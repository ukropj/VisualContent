/*
 * FrameButton.cpp
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#include "Viewer/FrameButton.h"
#include "Viewer/ControlFrame.h"
#include "Util/TextureWrapper.h"

#include <QDebug>
#include <osg/CullFace>
#include <osg/BlendFunc>

using namespace Vwr;

float FrameButton::BUTTON_SIZE = 40;
float FrameButton::BUTTON_MARGIN = 10;

FrameButton::FrameButton(ControlFrame* parentFrame) {
	frame = parentFrame;
}

FrameButton::FrameButton(ControlFrame* parentFrame, QString imagePath,
		osg::Vec2f relativePos) {
	frame = parentFrame;

	osg::Vec3f buttonPos(BUTTON_MARGIN / 2.0f, BUTTON_MARGIN / 2.0f + BUTTON_SIZE, 0);
	buttonPos.x() += relativePos.x() * (BUTTON_SIZE + BUTTON_MARGIN);
	buttonPos.y() += relativePos.y() * (BUTTON_SIZE + BUTTON_MARGIN);

	createGeometry(imagePath, buttonPos, osg::Vec2f(BUTTON_SIZE, BUTTON_SIZE));
}

FrameButton::FrameButton(ControlFrame* parentFrame, QString imagePath,
		osg::Vec3f pos, osg::Vec2f size) {
	frame = parentFrame;
	createGeometry(imagePath, pos, size);
}

FrameButton::~FrameButton() {
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
}
