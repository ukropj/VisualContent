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

FrameButton::FrameButton(ControlFrame* parentFrame) {
	frame = parentFrame;
}

FrameButton::FrameButton(ControlFrame* parentFrame, osg::Vec3f pos, QString imagePath) {
	frame = parentFrame;

	float width = 40;
	float height = 40;

	osg::Geometry* g = osg::createTexturedQuadGeometry(
			osg::Vec3(pos.x()-width/2.0f, pos.y()-height/2.0f, 0),
			osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
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

FrameButton::~FrameButton() {
}

