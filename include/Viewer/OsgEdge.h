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

#include "Viewer/DataMapping.h"

namespace Model {
class Edge;
}

namespace Vwr {

class OsgEdge {
	// OsgEdge is not Geode (for performance reasons),
	// just an empty shell that provides all data by getEdgeData and renders nothing
public:

	enum StateSetType {
		ORIENTED, UNORIENTED, ENDPOINT
	};

	OsgEdge(Model::Edge* modelNode, DataMapping* dataMapping = NULL);
	~OsgEdge();

	void setDataMapping(DataMapping* dataMapping = NULL);
	QString getMappingValue(DataMapping::ValueType prop);
	void updateGeometry();
	void getEdgeData(osg::ref_ptr<osg::Vec3Array> coords, osg::ref_ptr<
			osg::Vec2Array> textureCoords, osg::ref_ptr<
					osg::Vec4Array> colors);

	Model::Edge* getEdge() const {return edge;}
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

	void setVisible(bool flag) {
		visible = flag;
	}

	bool isVisible() const {
		return visible;
	}

	osg::ref_ptr<osg::Drawable> getLabel() {
		return label;
	}

	static osg::ref_ptr<osg::StateSet> createStateSet(StateSetType type);

private:

	Model::Edge* edge;
	DataMapping* mapping;

	bool selected;
	bool oriented;
	bool visible;

	osg::ref_ptr<osgText::FadeText> label;
	osg::Vec4 edgeColor;
	osg::Vec4 selectedColor;

	osg::ref_ptr<osg::Vec3Array> edgeCoords;
	osg::ref_ptr<osg::Vec2Array> edgeTexCoords;
	osg::ref_ptr<osg::Vec3Array> endPointCoords;

	osg::ref_ptr<osg::Geometry> createGeometry();
	osg::ref_ptr<osg::Geometry> createEndpointGeometry();
	osg::ref_ptr<osgText::FadeText> createLabel(QString text);

	static float EDGE_VOLUME;
};
}

#endif /* OSGEDGE_H_ */
