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

// XXX temporary method (until content info is read from file)
OsgContent* ContentFactory::createContent(Model::Node* node) {
	if (node->getGraph()->getName() == "Philosophers") {
		QString label = node->data(Model::Type::LABEL);
		int i = label.indexOf(':') + 1;
		label = label.mid(i, -1);
//		qDebug() << label;
		QString path = QString("input/philosophers/%1.jpg").arg(label);
		return new ImageContent(path);
	}

	int i = node->getId() % 11;
	if (i == 0) {
		QString text("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce ut eros id augue ullamcorper fringilla at id est. Donec egestas congue pretium. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. ");
		return new TextContent(text);
	}
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
