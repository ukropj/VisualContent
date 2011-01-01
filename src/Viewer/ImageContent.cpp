/*
 * ImageContent.cpp
 *
 *  Created on: 29.12.2010
 *      Author: jakub
 */

#include "Viewer/ImageContent.h"
#include "Util/TextureWrapper.h"

#include <QDebug>
#include <QString>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>

#include <osg/Geometry>
#include <osg/Texture2D>

using namespace Vwr;

ImageContent::ImageContent(QString imagePath) : imagePath(imagePath) {
	loaded = false;
}

ImageContent::~ImageContent() {
}

osg::ref_ptr<osg::Geometry> ImageContent::createGeometry(QString imagePath) {
	float scale = 0.2f;	// XXX magic number

	QImage image;
	image.load(imagePath);

	QSize size = image.size();
	float width = size.width()*150 / ((float)size.height());
	float height = 150;

	width *= scale;
	height *= scale;

	osg::Geometry* g = osg::createTexturedQuadGeometry(
			osg::Vec3(-width/2.0f, -height/2.0f, 0),
			osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
	g->setUseDisplayList(false);
	osg::ref_ptr<osg::Texture2D> texture =
			Util::TextureWrapper::readTextureFromFile(imagePath);
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

//		QLabel* l = new QLabel;
//		QImage image;
//		image.load(imagePath);
//		l->setPixmap(QPixmap::fromImage(image));
//		QSize size = image.size();
//		l->setGeometry(QRect(QPoint(1, 1), size));
//		setWidget(l, 0.2f);

		addDrawable(createGeometry(imagePath));
		return true;
	}
}
