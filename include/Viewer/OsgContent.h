/*
 * OsgFrame.h
 *
 *  Created on: 29.11.2010
 *      Author: jakub
 */

#ifndef OSGFRAME_H_
#define OSGFRAME_H_

#include <osg/Vec3f>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <QString>


namespace Vwr {

class OsgContent : public osg::Geode {
public:
	OsgContent(QString imagePath);
	~OsgContent();

	void load();

private:
	osg::ref_ptr<osg::Geometry> createGeometry(float width, float height);
	QString imagePath;
	osg::ref_ptr<osg::Texture2D> texture;
};
}
#endif /* OSGFRAME_H_ */
