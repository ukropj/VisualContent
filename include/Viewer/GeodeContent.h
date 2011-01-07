/*
 * GeodeContent.h
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#ifndef GEODECONTENT_H_
#define GEODECONTENT_H_

#include "OsgContent.h"

namespace Vwr {

class GeodeContent: public Vwr::OsgContent {
public:
	GeodeContent(osg::Geode* geode);
	~GeodeContent();
	bool load();

private:
	bool loaded;
	float innerScale;
};

}

#endif /* GEODECONTENT_H_ */
