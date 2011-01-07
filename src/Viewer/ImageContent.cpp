/*
 * ImageContent.cpp
 *
 *  Created on: 29.12.2010
 *      Author: jakub
 */

#include "Viewer/ImageContent.h"
#include "Util/TextureWrapper.h"

#include <QDebug>

#include <osg/Image>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

using namespace Vwr;

ImageContent::ImageContent(QString imagePath) : imagePath(imagePath) {
	loaded = false;
}

ImageContent::~ImageContent() {
}

osg::ref_ptr<osg::Geometry> ImageContent::createGeometry(QString imagePath) {
	float scale = 0.2f;	// XXX magic number

	osg::Image* image = osgDB::readImageFile(imagePath.toStdString());

	float width = image->s()*150 / ((float)image->t());
	float height = 150;

	width *= scale;
	height *= scale;

	osg::Geometry* g = osg::createTexturedQuadGeometry(
			osg::Vec3(-width/2.0f, -height/2.0f, 0),
			osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
	g->setUseDisplayList(false);

	osg::ref_ptr<osg::Texture2D> texture =
			Util::TextureWrapper::createTexture(image, false);
	osg::ref_ptr<osg::StateSet> stateSet = getOrCreateStateSet();
	stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	g->setStateSet(stateSet);

	return g;
}

bool ImageContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;
		contentGeode = new osg::Geode;
		contentGeode->addDrawable(createGeometry(imagePath));
		contentGeode->setName("node_content_image");
		addChild(contentGeode);
		return true;
	}
}
