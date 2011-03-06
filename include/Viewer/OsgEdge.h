/*
 * OsgEdge.h
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#ifndef OSGEDGE_H_
#define OSGEDGE_H_

#include <QString>
#include <QtCore/QMap>
#include <QDebug>

#include <osg/Stateset>
#include <osgText/Text>
#include <osgText/FadeText>

#include "Viewer/OsgProperty.h"

namespace Model {
class Edge;
}

namespace Vwr {

class OsgEdge {
	// OsgEdge is not Geode any more (for performance reasons),
	// just an empty shell that provides all data by getEdgeData and renders nothing
public:

	enum StateSetType {
		ORIENTED, UNORIENTED, ENDPOINT
	};

	OsgEdge(Model::Edge* edge, OsgProperty* property);
	~OsgEdge();

	QString getPropertyValue(OsgProperty::ValueType prop);
	void updateGeometry();
	void getEdgeData(osg::ref_ptr<osg::Vec3Array> coords, osg::ref_ptr<
			osg::Vec2Array> textureCoords, osg::ref_ptr<
					osg::Vec4Array> colors);

	bool isOriented();
	QString toString() const;

	osg::Vec4 getEdgeColor() const {
		return edgeColor;
	}

	void setColor(osg::Vec4 color) {
		edgeColor = color;
	}

	bool isSelected() const {
		return selected;
	}

	void setSelected(bool val) {
		selected = val;
	}

	void showLabel(bool visible);

	static osg::ref_ptr<osg::StateSet> createStateSet(StateSetType type);

private:

	Model::Edge* edge;
	OsgProperty* property;

	bool selected;
	bool oriented;

	osg::ref_ptr<osgText::FadeText> label;
	osg::Vec4 edgeColor;

	osg::ref_ptr<osg::Vec3Array> edgeCoords;
	osg::ref_ptr<osg::Vec2Array> edgeTexCoords;
	osg::ref_ptr<osg::Vec3Array> endPointCoords;

	osg::ref_ptr<osg::Geometry> createGeometry();
	osg::ref_ptr<osg::Geometry> createEndpointGeometry();
	osg::ref_ptr<osgText::FadeText> createLabel(QString text);
};
}

#endif /* OSGEDGE_H_ */
