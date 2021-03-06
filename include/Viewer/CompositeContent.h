#ifndef COMPOSITECONTENT_H_
#define COMPOSITECONTENT_H_

#include "Viewer/WidgetContent.h"

namespace Vwr {
class OsgCluster;

/// Content implementation for visual clusters.
class CompositeContent : public OsgContent {
public:
	CompositeContent(OsgCluster* cluster);
	~CompositeContent();
	const osg::BoundingBox& getBoundingBox() const;
	bool load();

private:
	void loadInCircle(float radius, float scale);
	void loadInGrid(float margin, float rowHeight);
	osg::ref_ptr<osg::Geometry> createGeometry(osg::BoundingBox box);

	bool loaded;
	OsgCluster* cluster;
	osg::BoundingBox box;
};

}

#endif /* COMPOSITECONTENT_H_ */
