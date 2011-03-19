/*
 * OsgNode.h
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#ifndef OSGNODE_H_
#define OSGNODE_H_

#include "Viewer/AbstractNode.h"
#include "Viewer/DataMapping.h"

#include <QMap>
#include <QString>
#include <QTextStream>

#include <osg/Vec3f>
#include <osg/Geode>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/AutoTransform>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>

namespace Model {
class Node;
}

namespace Vwr {
class OsgContent;
class AbstractVisitor;


class OsgNode: public osg::AutoTransform, public AbstractNode {
public:
	static uint MASK_ON;
	static uint MASK_OFF;

	OsgNode(Model::Node* node, DataMapping* dataMapping = NULL);
	~OsgNode();

	void setDataMapping(DataMapping* dataMapping = NULL);
	QString getMappingValue(DataMapping::ValueType prop);
	QSet<AbstractNode*> getIncidentNodes();

	osg::Vec3f getPosition() const;
	void updatePosition(float interpolationSpeed = 1);
	void setPosition(osg::Vec3f pos);

	void resize(float factor);
	osg::Vec3f getSize() const;
	float getRadius() const;

	void setFixed(bool flag);
	bool isFixed() const;
	void setFrozen(bool flag);
	bool isFrozen() const;
	void setSelected(bool flag);
	bool isSelected() const;
	void setExpanded(bool flag);
	bool isExpanded() const;

	void setVisible(bool flag);
	bool isVisible() const;

	bool isPickable(osg::Geode* geode) const;
	void setPickable(bool flag) {pickable = flag;}

	void acceptVisitor(AbstractVisitor* visitor);

	QString toString() const;

	void setColor(osg::Vec4 color);
	osg::Vec4 getColor() const {return color;}
	void showLabel(bool visible);

	bool isUsingInterpolation() const {return usingInterpolation;}
	void setUsingInterpolation(bool val) {usingInterpolation = val;}
	void reloadConfig();

	bool isOnScreen() const;
	static bool mayOverlap(OsgNode* u, OsgNode* v);
	float getDistanceToEdge(double angle) const;

	bool equals(OsgNode* other) const;

	void getProjRect(float &xMin, float &yMin, float &xMax, float &yMax);

	Model::Node* getNode() const {return node;}

private:

	void setSize(osg::BoundingBox box);
	void setSize(float width, float height, float depth = 0);

	osg::Vec3f size;

	Model::Node* node;

	bool selected;
	bool usingInterpolation;
	bool pickable;
	bool expanded;
	float maxScale;
	bool visible;

	void setDrawableColor(osg::ref_ptr<osg::Geode> geode, int drawablePos,
			osg::Vec4 color);

	osg::ref_ptr<osg::StateSet> createStateSet();

	osg::ref_ptr<osg::Geode> initFrame();
	void updateFrame(osg::ref_ptr<osg::Geode> frame, osg::BoundingBox box, float scale = 1, float margin = 0);

	osg::ref_ptr<osg::Geode> createTextureNode(osg::ref_ptr<osg::Texture2D> texture,
			float width, float height);
	osg::ref_ptr<osg::Drawable> createRect(float width, float height, osg::Vec4f color);
	osg::ref_ptr<osg::Geode> createFixed();
	osg::ref_ptr<osg::Geode> createLabel(QString text);

	osg::ref_ptr<osg::Geometry> createCustomGeometry(osg::Vec3 coords[], const int vertNum,
			GLenum mode, osg::Vec4 color = osg::Vec4(1.0, 1.0, 1.0, 1.0));

	osg::Vec4 color;

	osg::ref_ptr<osg::Geode> frameG;

	osg::ref_ptr<osg::Geode> labelG;
	osg::ref_ptr<osg::Geode> fixedG;

	osg::ref_ptr<osg::Switch> contentSwitch;
	osg::ref_ptr<osg::Geode> closedG;
	osg::ref_ptr<OsgContent> visualContent;

	DataMapping* mapping;
};
}

#endif /* OSGNODE_H_ */
