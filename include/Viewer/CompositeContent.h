/*
 * CompositeContent.h
 *
 *  Created on: 7.4.2011
 *      Author: jakub
 */

#ifndef COMPOSITECONTENT_H_
#define COMPOSITECONTENT_H_

#include "Viewer/WidgetContent.h"

namespace Vwr {
class OsgCluster;

class CompositeContent : public OsgContent {
public:
	CompositeContent(OsgCluster* cluster);
	~CompositeContent();
	const osg::BoundingBox& getBoundingBox() const;
	bool load();

private:
	osg::ref_ptr<osg::Geometry> createGeometry(osg::BoundingBox box);

	bool loaded;
	OsgCluster* cluster;
	osg::BoundingBox box;
};

}

#endif /* COMPOSITECONTENT_H_ */
