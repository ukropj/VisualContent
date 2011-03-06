/*
 * OsgFrame.cpp
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

OsgContent* ContentFactory::createContent(OsgProperty::ContentType type, QString data) {
	switch (type) {
	case OsgProperty::IMAGE :
		return new ImageContent(data);
	case OsgProperty::TEXT :
		return new TextContent(data);
		// TODO other content, validation ?
	case OsgProperty::RANDOM :
	case OsgProperty::NO_CONTENT :
	default :
		// generate random content
		QString path = QString("img/pic%1.jpg").arg((qrand() % 11) + 1);
		return new ImageContent(path);
	}
}
