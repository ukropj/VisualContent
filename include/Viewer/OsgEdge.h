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
#include <QDebug>

#include <osg/PrimitiveSet>
#include <osg/Geode>
#include <osg/Stateset>
#include <osgText/Text>
#include <osgText/FadeText>

namespace Model {
class Edge;
}

namespace Vwr {
class SceneGraph;

class OsgEdge: public osg::Geode {
public:

	enum StateSetType {
		ORIENTED, NONORIENTED, ENDPOINT
	};

	OsgEdge(Model::Edge* edge, SceneGraph* sceneGraph);
	~OsgEdge();

	void updateGeometry();

	QString toString() const;

	osg::Vec4 getEdgeColor() const {
		if (selected)
			return osg::Vec4f(1.0f, 0.0f, 0.0f, 1.0f);
		else
			return edgeColor;
	}

	void setColor(osg::Vec4 color) {
		edgeColor = color;
//		qDebug() << color.x() << ", " << color.y() << ", "<< color.z()<< ", " << color.w();
		if (geometry != NULL) {
			osg::Vec4Array * colorArray =
					dynamic_cast<osg::Vec4Array *> (geometry->getColorArray());
			colorArray->pop_back();
			colorArray->push_back(color);
		} else {
			qDebug() << "edge geometry was null";
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

	osg::ref_ptr<osg::StateSet> getStateSetInstance(StateSetType type);

private:

	Model::Edge* edge;
	Vwr::SceneGraph* sceneGraph;

	bool selected;

	osg::ref_ptr<osg::Geometry> geometry;
	osg::ref_ptr<osg::Geometry> endPoints;
	osg::ref_ptr<osgText::FadeText> label;
	osg::Vec4 edgeColor;

	osg::ref_ptr<osg::Vec3Array> edgeCoords;
	osg::ref_ptr<osg::Vec2Array> edgeTexCoords;
	osg::ref_ptr<osg::Vec3Array> endPointCoords;

	osg::ref_ptr<osg::Geometry> createGeometry();
	osg::ref_ptr<osg::Geometry> createEndpointGeometry();
	osg::ref_ptr<osgText::FadeText> createLabel(QString text);

	static osg::ref_ptr<osg::StateSet> stateSet;
	static osg::ref_ptr<osg::StateSet> stateSetOriented;
	static osg::ref_ptr<osg::StateSet> stateSetEndpoint;
	osg::ref_ptr<osg::StateSet> createStateSet(StateSetType type) const;
};
}

#endif /* OSGEDGE_H_ */