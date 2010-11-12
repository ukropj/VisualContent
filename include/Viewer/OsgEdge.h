/*
 * OsgEdge.h
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#ifndef OSGEDGE_H_
#define OSGEDGE_H_

#include <QString>
#include <QTextStream>
#include <QtCore/QMap>

#include <osg/PrimitiveSet>
#include <osg/Geode>
#include <osg/Stateset>
#include <osgText/Text>
#include <osgText/FadeText>

#include "Model/Edge.h"

namespace Vwr {
class SceneGraph;

class OsgEdge: public osg::Geode {
public:

	OsgEdge(Model::Edge* edge, SceneGraph* sceneGraph);
	~OsgEdge();

	qlonglong getId() const {
		return edge->getId();
	}

	QString getName() const {
		return edge->getName();
	}

//	bool isOriented() const {
//		return oriented;
//	}

//	void linkNodes(QMap<qlonglong, osg::ref_ptr<Edge> > *edges);
//	void unlinkNodes();
//	void unlinkNodesAndRemoveFromGraph();

	QString toString() const {
		QString str;
		QTextStream(&str) << "edge id:" << getId() << " name:" << getName();
		return str;
	}

	void updateGeometry();

	osg::Vec4 getEdgeColor() const {
		if (selected)
			return osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		else
			return edgeColor;
	}

	void setColor(osg::Vec4 color) {
		edgeColor = color;
		if (geometry != NULL) {
			osg::Vec4Array * colorArray =
					dynamic_cast<osg::Vec4Array *> (geometry->getColorArray());
			colorArray->pop_back();
			colorArray->push_back(color);
		}
	}

	bool isSelected() const {
		return selected;
	}

	void setSelected(bool val) {
		selected = val;
	}

	void showLabel(bool visible) {
		if (this->containsDrawable(label) && !visible)
			removeDrawable(label);
		if (!this->containsDrawable(label) && visible)
			addDrawable(label);
	}

	static osg::ref_ptr<osg::StateSet> getStateSetInstance(bool oriented);

private:

	Model::Edge* edge;
	Vwr::SceneGraph* sceneGraph;

	bool selected;

	osg::ref_ptr<osg::Geometry> geometry;
	osg::ref_ptr<osgText::FadeText> label;
	osg::Vec4 edgeColor;

	osg::ref_ptr<osg::Geometry> createGeometry();
	osg::ref_ptr<osgText::FadeText> createLabel(QString text);

	static osg::ref_ptr<osg::StateSet> stateSet;
	static osg::ref_ptr<osg::StateSet> stateSetOriented;
	static osg::ref_ptr<osg::StateSet> createStateSet(bool oriented);
};
}

#endif /* OSGEDGE_H_ */
