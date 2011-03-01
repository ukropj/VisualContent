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

OsgContent* ContentFactory::createContent(Model::Node* node) {
	if (node->containsData(Model::Type::IMAGE)) {
		QString imgPath = node->data(Model::Type::IMAGE);
		return new ImageContent(imgPath);
	} else if (node->containsData(Model::Type::TEXT)) {
		QString text = node->data(Model::Type::TEXT);
		return new TextContent(text);
	}
	// TODO other content, validation ?

	// generate random content
	int i = (node->getId() % 11) + 1;
//	if (i == 1) {
//		osg::Group* g = dynamic_cast<osg::Group*> (osgDB::readNodeFile("img/cow.osg"));
//		qDebug() << (g == NULL);
//		osg::Geode* ge = dynamic_cast<osg::Geode*> (g->getChild(0));
//		qDebug() << (ge == NULL);
//		return new GeodeContent(ge);
//	}
	QString path = QString("img/pic%1.jpg").arg(i);
	return new ImageContent(path);
}
