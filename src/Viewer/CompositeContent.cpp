/*
 * CompositeContent.cpp
 *
 *  Created on: 7.4.2011
 *      Author: jakub
 */

#include "Viewer/CompositeContent.h"
#include "Viewer/OsgCluster.h"
#include "Model/Cluster.h"
#include "Util/Config.h"
#include <math.h>

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

		bool circle = Util::Config::getValue("Viewer.Cluster.ContentInCircle") != "0";
		if (circle)
			loadInCircle(10, 0.3);
		else
			loadInGrid(1, 10);

		contentGeode = new osg::Geode();
		contentGeode->addDrawable(createGeometry(box));
		contentGeode->setName("node_content_cluster");
		addChild(contentGeode);
		return true;
	}
}

void CompositeContent::loadInCircle(float radius, float scale) {
	float angle = 2 * osg::PI / cluster->nodeCluster->getChildrenCount();
	int i = 0;
	osg::Vec3f minVec(0, 0, 0);
	osg::Vec3f maxVec(0, 0, 0);

	QSetIterator<Model::Node*> nodeIt = cluster->nodeCluster->getChildrenIterator();
	while (nodeIt.hasNext()) {
		OsgNode* node = nodeIt.next()->getOsgNode();
		OsgContent* chContent = node->getVisualContent();
		chContent->load();
		node->updateFrames();
		osg::Geode* border = node->getVisualBorder();

		osg::Vec3f pos(radius * sin(i * angle), radius * cos(i * angle), 1);
		i++;

		osg::PositionAttitudeTransform* tr = new osg::PositionAttitudeTransform();
		tr->setPosition(pos);
		tr->setScale(osg::Vec3f(scale, scale, scale));
		tr->addChild(chContent);
		tr->addChild(border);
		addChild(tr);

		osg::BoundingBox chBox = border->getBoundingBox();
		float width = qAbs(chBox.xMax() - chBox.xMin()) * scale;
		float height = qAbs(chBox.yMax() - chBox.yMin()) * scale;
		minVec.x() = qMin(minVec.x(), pos.x() - width / 2.0f);
		maxVec.x() = qMax(maxVec.x(), pos.x() + width / 2.0f);
		minVec.y() = qMin(minVec.y(), pos.y() - height / 2.0f);
		maxVec.y() = qMax(maxVec.y(), pos.y() + height / 2.0f);
	}
	box = osg::BoundingBoxImpl<osg::Vec3f>(minVec, maxVec);
}

void CompositeContent::loadInGrid(float margin, float rowHeight) {
	osg::PositionAttitudeTransform* gridTr = new osg::PositionAttitudeTransform();
	int columns = ceil(sqrt(cluster->nodeCluster->getChildrenCount()));
	float col = 0, x = margin, y = -margin;
	float width;
	float maxX = 0, minY = 0;

	QSetIterator<Model::Node*> nodeIt = cluster->nodeCluster->getChildrenIterator();
	while (nodeIt.hasNext()) {
		OsgNode* node = nodeIt.next()->getOsgNode();
		OsgContent* chContent = node->getVisualContent();
		chContent->load();
		node->updateFrames();
		osg::Geode* border = node->getVisualBorder();

		if (col >= columns) {
			col = 0;
			y -= (rowHeight + margin);
			x = margin;
		}
		col++;

		osg::BoundingBox chBox = border->getBoundingBox();
		float scale = rowHeight / qAbs(chBox.yMax() - chBox.yMin());
		width = qAbs(chBox.xMax() - chBox.xMin()) * scale;

		x += width / 2.0f;
		osg::Vec3f pos(x, y - rowHeight / 2.0f, 1);
		x += width / 2.0f + margin;

		osg::PositionAttitudeTransform* tr = new osg::PositionAttitudeTransform();
		tr->setPosition(pos);
		tr->setScale(osg::Vec3f(scale, scale, scale));
		tr->addChild(chContent);
		tr->addChild(border);
		gridTr->addChild(tr);

		maxX = qMax(maxX, x);
		minY = qMin(minY, y);
	}
	minY -= (rowHeight + margin);

	box = osg::BoundingBoxImpl<osg::Vec3f>(osg::Vec3f(-maxX/2.0f, minY/2, 0), osg::Vec3f(maxX/2.0f, -minY/2.0f, 0));
	gridTr->setPosition(-osg::Vec3f(maxX/2.0, minY/2.0, 0));
	addChild(gridTr);
}
