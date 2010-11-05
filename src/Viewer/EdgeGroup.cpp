#include "Viewer/EdgeGroup.h"
#include "Util/TextureWrapper.h"
#include <QDebug>

using namespace Vwr;
using namespace Model;

EdgeGroup::EdgeGroup(QMap<qlonglong, osg::ref_ptr<Edge> > *edges, float scale) {
	this->edges = edges;
	this->scale = scale;

	edgeGroup = new osg::Group;
	edgeGroup->setName("edges_group");
	edgeGroup->setStateSet(Edge::createStateSet(false));
	// NOTE: all edges are shown as unoriented for now!

	QMap<qlonglong, osg::ref_ptr<Edge> >::iterator i = edges->begin();
	while (i != edges->end()) {
		edgeGroup->addChild(i.value());
		i++;
	}

}

EdgeGroup::~EdgeGroup() {
}

void EdgeGroup::updateEdgeCoords(osg::Vec3d viewVector) {
	QMap<qlonglong, osg::ref_ptr<Edge> >::iterator i = edges->begin();

	while (i != edges->end()) {
		i.value()->updateGeometry(viewVector);
		i++;
	}
}
