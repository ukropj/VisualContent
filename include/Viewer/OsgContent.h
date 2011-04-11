/*
 * OsgContent.h
 *
 *  Created on: 29.11.2010
 *      Author: jakub
 */

#ifndef OSGCONTENT_H_
#define OSGCONTENT_H_

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/BoundingBox>
#include <osg/PositionAttitudeTransform>
#include <QString>
#include <QDebug>
#include "Viewer/DataMapping.h"

namespace Model {
	class Node;
}

namespace Vwr {

class OsgContent : public osg::PositionAttitudeTransform {
public:
	OsgContent();
	~OsgContent();

	virtual bool load() = 0;
	virtual std::string getGeodeName() const;
	virtual const osg::BoundingBox& getBoundingBox() const;

	osg::Geode* getGeode() const {return contentGeode;}
protected:
	osg::ref_ptr<osg::Geode> contentGeode;
};

class EmptyContent : public OsgContent {
public:
	EmptyContent();
	~EmptyContent() {}
	bool load();
private:
	bool loaded;

};

class ObjectContent : public OsgContent {
public:
	ObjectContent(QString osgPath);
	~ObjectContent() {}
	bool load();
private:
	bool loaded;
	QString osgPath;

};

class ContentFactory {
public:
	static OsgContent* createContent(DataMapping::ContentType type, QString data);
private:
};
}
#endif /* CONTENT_H_ */
