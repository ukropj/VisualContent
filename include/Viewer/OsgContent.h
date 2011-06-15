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

/// Abstract visual content of OsgNode.
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

/// Empty content implementation.
class EmptyContent : public OsgContent {
public:
	EmptyContent();
	~EmptyContent() {}
	bool load();
private:
	bool loaded;

};

/// Creates OsgContent implementations.
class ContentFactory {
public:
	static OsgContent* createContent(DataMapping::ContentType type, int id, QString data);
	static OsgContent* createRandomContent(int id);
	static OsgContent* createAlphabetContent(int id);
private:
};
}
#endif /* CONTENT_H_ */
