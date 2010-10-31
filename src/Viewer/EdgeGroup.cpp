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
	edgeGroup->setStateSet(createStateSet(false));
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

osg::ref_ptr<osg::StateSet> EdgeGroup::createStateSet(bool oriented) {
	osg::ref_ptr<osg::StateSet> edgeStateSet = new osg::StateSet;

	if (!oriented) {
		edgeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		edgeStateSet->setTextureAttributeAndModes(0,
				Util::TextureWrapper::getEdgeTexture(), osg::StateAttribute::ON);
		edgeStateSet->setAttributeAndModes(new osg::BlendFunc,
				osg::StateAttribute::ON);
	} else {
		edgeStateSet->setTextureAttributeAndModes(0,
				Util::TextureWrapper::getOrientedEdgeTexture(),
				osg::StateAttribute::ON);
		edgeStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	}

	edgeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	edgeStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	edgeStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	edgeStateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

	return edgeStateSet;
}
