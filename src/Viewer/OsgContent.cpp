/*
 * ControlFrame.cpp
 *
 *  Created on: 29.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgContent.h"
#include "Viewer/TextContent.h"
#include "Viewer/ImageContent.h"
#include "Viewer/GeodeContent.h"
#include "Model/Node.h"
#include "Model/Type.h"
#include "Model/Graph.h"
#include <osgDB/ReadFile>
#include <QDebug>

using namespace Vwr;

OsgContent::OsgContent() {
	contentGeode = NULL;
}

OsgContent::~OsgContent() {
}

std::string OsgContent::getGeodeName() const {
	if (contentGeode != NULL) {
		return contentGeode->getName();
	} else {
		return "?";
	}
}

const osg::BoundingBox& OsgContent::getBoundingBox() const {
	if (contentGeode != NULL)
		return contentGeode->getBoundingBox();
	else
		return (new osg::Geode)->getBoundingBox();
}

OsgContent* ContentFactory::createContent(DataMapping::ContentType type, QString data) {
	switch (type) {
	case DataMapping::IMAGE :
		return new ImageContent(data);
	case DataMapping::TEXT :
		return new TextContent(data);
		// TODO other content, validation ?
	case DataMapping::WEB :
		return new WebContent(data);
	case DataMapping::COMPOSITE :
	case DataMapping::NO_CONTENT :
		return new EmptyContent();
	case DataMapping::RANDOM :
	default :
		// generate random content
		QString path = QString("img/pic%1.jpg").arg((qrand() % 11) + 1);
		return new ImageContent(path);
	}
}

EmptyContent::EmptyContent() {
	loaded = false;
}

bool EmptyContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;
		contentGeode = new osg::Geode;
		float width = 10, height = 10;
		osg::Geometry* g = osg::createTexturedQuadGeometry(
				osg::Vec3(-width/2.0f, -height/2.0f, 0),
				osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
		g->setUseDisplayList(false);
		contentGeode->addDrawable(g);
		contentGeode->setName("node_content_empty");
		addChild(contentGeode);
		return true;
	}
}
