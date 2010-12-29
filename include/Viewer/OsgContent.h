/*
 * OsgFrame.h
 *
 *  Created on: 29.11.2010
 *      Author: jakub
 */

#ifndef OSGFRAME_H_
#define OSGFRAME_H_

#include <osg/Geode>

namespace Vwr {

class OsgContent : public osg::Geode {
public:
	OsgContent() {};
	~OsgContent() {};

	virtual bool load() = 0;
};
}
#endif /* OSGFRAME_H_ */
