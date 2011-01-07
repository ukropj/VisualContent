/*
 * GeodeContent.cpp
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#include "Viewer/GeodeContent.h"
#include <osg/MatrixTransform>

using namespace Vwr;

GeodeContent::GeodeContent(osg::Geode* geode) {
	contentGeode = geode;
	ParentList parents = contentGeode->getParents();
	ParentList::const_iterator i;
	i = parents.begin();
	while (i != parents.end()) {
		(*i)->removeChild(contentGeode);
		i++;
	}
	contentGeode->setName("node_content_geode");
	innerScale = 1;
	loaded = false;
}

GeodeContent::~GeodeContent() {
}

bool GeodeContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;
		float size = contentGeode->getBoundingBox().radius();
		innerScale = (150/size) * 0.2f;
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		mt->setMatrix(osg::Matrix::rotate(osg::Vec3(0, 1, 0), osg::Vec3(0, 0, -1)) *
				osg::Matrix::scale(1, 1, 1)); // TODO to scale or not to scale?
		mt->addChild(contentGeode);
		addChild(mt);
		return true;
	}
}

