/*
 * OsgContent.h
 *
 *  Created on: 29.11.2010
 *      Author: jakub
 */

#ifndef OSGCONTENT_H_
#define OSGCONTENT_H_

#include <osg/Geode>
#include <osg/PositionAttitudeTransform>

namespace Vwr {

class OsgContent : public osg::Geode {
public:
	OsgContent();
	~OsgContent();

	virtual bool load() = 0;

	void setTransform(osg::PositionAttitudeTransform* tr) {
		transform = tr;
	}
	osg::PositionAttitudeTransform* getTransform() const {
		return transform;
	}
	void setScale(osg::Vec3d scale) {
		if (transform != NULL)
			transform->setScale(scale);
	}
	osg::Vec3d getScale() const {
		if (transform == NULL)
			return osg::Vec3d(0, 0, 0);
		return transform->getScale();
	}
private:
	osg::ref_ptr<osg::PositionAttitudeTransform> transform;
};
}
#endif /* CONTENT_H_ */
