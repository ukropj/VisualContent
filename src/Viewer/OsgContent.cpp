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
#include <osg/AutoTransform>
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
	case DataMapping::OSG :
		return new ObjectContent(data);
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

ObjectContent::ObjectContent(QString osgPath) : osgPath(osgPath) {
	loaded = false;
}

bool ObjectContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;
		osg::Node* node = osgDB::readNodeFile(osgPath.toStdString());
		if (node != NULL) {
			if (node->asGeode() != NULL) {
				contentGeode = node->asGeode();
			} else {
				osg::Group* group = node->asGroup();
				if (group != NULL) {
					int ch = group->getNumChildren();
					int i = 0;
					while (contentGeode == NULL && i < ch) {
						contentGeode = group->getChild(i)->asGeode();
					}
				}
			}
		}

		if (contentGeode == NULL) {
			qWarning() << "Cannot read from " << osgPath;
 			contentGeode = new osg::Geode;
			float width = 10, height = 10;
			osg::Geometry* g = osg::createTexturedQuadGeometry(
					osg::Vec3(-width/2.0f, -height/2.0f, 0),
					osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
			g->setUseDisplayList(false);
			contentGeode->addDrawable(g);
			contentGeode->setName("node_content_osg");
			addChild(contentGeode);
		} else {
			contentGeode->setName("node_content_osg");
			osg::BoundingBox box = contentGeode->getBoundingBox();
			osg::PositionAttitudeTransform* tr = new osg::PositionAttitudeTransform();
			osg::Quat rot;
			rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(0, 1, 0));
			tr->setAttitude(rot);
//			tr->setScale(osg::Vec3d(3, 3, 3));
			tr->setPosition(box.center());
			tr->addChild(node);
			osg::AutoTransform* tr2 = new osg::AutoTransform();
			tr2->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_CAMERA);
			tr2->addChild(tr);
			addChild(tr2);
//			setScale(osg::Vec3d(3, 3, 3));
		}
		return true;
	}
}
