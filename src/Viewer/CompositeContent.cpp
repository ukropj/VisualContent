/*
 * CompositeContent.cpp
 *
 *  Created on: 7.4.2011
 *      Author: jakub
 */

#include "Viewer/CompositeContent.h"
#include "Viewer/OsgCluster.h"
#include "Model/Cluster.h"
#include <math.h>
#include <osg/BoundingBox>
#include "Util/CameraHelper.h"

using namespace Vwr;

CompositeContent::CompositeContent(OsgCluster* cluster) : cluster(cluster){
	loaded = false;
}

CompositeContent::~CompositeContent() {
}

const osg::BoundingBox& CompositeContent::getBoundingBox() const {
	return box;
}

osg::ref_ptr<osg::Geometry> CompositeContent::createGeometry(osg::BoundingBox box) {
	osg::Vec3 coords[] = {
			osg::Vec3(box.xMin(), box.yMin(), 0),
			osg::Vec3(box.xMax(), box.yMin(), 0),
			osg::Vec3(box.xMax(), box.yMax(), 0),
			osg::Vec3(box.xMin(), box.yMax(), 0) };

	osg::ref_ptr<osg::Geometry> g = new osg::Geometry;

	g->setVertexArray(new osg::Vec3Array(4, coords));
	g->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	g->setUseDisplayList(false);

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4f(0.5f,0.5f,0.5f,0.5f));
	g->setColorArray(colorArray);
	g->setColorBinding(osg::Geometry::BIND_OVERALL);
	g->setName("blank drawable");
	return g;
}

bool CompositeContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;

		QSetIterator<Model::Node*> nodeIt = cluster->nodeCluster->getChildrenIterator();
		float angle = 2 * osg::PI / cluster->nodeCluster->getChildrenCount();
		float l = 10;
		int i = 0;
		float scale = 0.3f;
		osg::Vec3f minVec(0,0,0);
		osg::Vec3f maxVec(0,0,0);
		while (nodeIt.hasNext()) {
			OsgContent* chContent = nodeIt.next()->getOsgNode()->getVisualContent();
			chContent->load();

			osg::PositionAttitudeTransform* tr = new osg::PositionAttitudeTransform();
			osg::Vec3f pos(l * sin(i * angle), l * cos(i * angle), 1);
			tr->setPosition(pos);
			tr->setScale(osg::Vec3f(scale, scale, scale));
			tr->addChild(chContent);
			addChild(tr);

			osg::BoundingBox chBox = chContent->getBoundingBox();
			minVec.x() = qMin(minVec.x(), pos.x() + chBox.xMin() * scale);
			maxVec.x() = qMax(maxVec.x(), pos.x() + chBox.xMax() * scale);
			minVec.y() = qMin(minVec.y(), pos.y() + chBox.yMin() * scale);
			maxVec.y() = qMax(maxVec.y(), pos.y() + chBox.yMax() * scale);

			i++;
		}

		Util::CameraHelper::printVec(minVec);
		Util::CameraHelper::printVec(maxVec);

		box = osg::BoundingBoxImpl<osg::Vec3f>(minVec, maxVec);
		contentGeode = new osg::Geode();
		contentGeode->addDrawable(createGeometry(box));
		contentGeode->setName("node_content_cluster");
		addChild(contentGeode);
		return true;
	}
}
