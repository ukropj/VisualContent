/*
 * OsgNode.h
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#ifndef OSGNODE_H_
#define OSGNODE_H_

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
class OsgFrame;

class OsgNode: public osg::Switch {
public:

	OsgNode(Model::Node* node, osg::ref_ptr<osg::AutoTransform> nodeTansform);

	~OsgNode();

	osg::Vec3f getPosition() const;
	void updatePosition(float interpolationSpeed = 1);
	void setPosition(osg::Vec3f pos);

	bool setFixed(bool flag);
	bool isFixed() const;
	void setFrozen(bool flag);
	bool isFrozen() const;
	bool setSelected(bool flag);
	bool isSelected() const {
		return selected;
	}

	bool isExpanded() const {
		return expanded;
	}
	void toggleExpanded() {
		setExpanded(!expanded);
	}
	bool setExpanded(bool flag);

	void setFrame(OsgFrame* frame) {
		myFrame = frame;
	}

	bool isPickable(osg::Geode* geode) const;
	bool isResizable(osg::Geode* geode) const;

	QString toString() const;

	void setColor(osg::Vec4 color);
	osg::Vec4 getColor() const {
		return color;
	}

	float getRadius() const;

	osg::Vec3f getSize() const;

	void showLabel(bool visible);

	bool isUsingInterpolation() const {
		return usingInterpolation;
	}

	void setUsingInterpolation(bool val) {
		usingInterpolation = val;
	}

	void reloadConfig();

	bool isOnScreen() const;

	static bool mayOverlap(OsgNode* u, OsgNode* v);

	float getDistanceToEdge(double angle) const;

	void setPickable(bool flag) {
		pickable = flag;
	}

	void resize(float factor);

	static osg::Vec4 selectedColor;

	bool equals(OsgNode* other) const;

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

	void setDrawableColor(osg::ref_ptr<osg::Geode> geode, int drawablePos,
			osg::Vec4 color);

	osg::ref_ptr<osg::StateSet> createStateSet();

	osg::ref_ptr<osg::Geode> initFrame();
	void updateFrame(osg::ref_ptr<osg::Geode> frame, osg::BoundingBox box, float scale = 1, float margin = 0);

	osg::ref_ptr<osg::Geode> createTextureNode(osg::ref_ptr<osg::Texture2D> texture,
			float width, float height);
	osg::ref_ptr<osg::Drawable> createRect(float width, float height, osg::Vec4f color);
	osg::ref_ptr<osg::Geode> createFixed();
	osg::ref_ptr<osg::Geode> createLabel(QString text, const float scale = 1);

	osg::ref_ptr<osg::Geometry> createCustomGeometry(osg::Vec3 coords[], const int vertNum,
			GLenum mode, osg::Vec4 color = osg::Vec4(1.0, 1.0, 1.0, 1.0));

	osg::Vec4 color;

	osg::ref_ptr<osg::AutoTransform> nodeTransform;

	osg::ref_ptr<osg::Geode> frameG;

	osg::ref_ptr<osg::Geode> label;
	static osg::ref_ptr<osg::Geode> fixedG;

	osg::ref_ptr<osg::Geode> closedG;
	OsgContent* contentG;

	OsgFrame* myFrame;

	// constants

//	static const osg::Vec4 fixedColor;
};
}

//const osg::Vec4 OsgNode::fixedColor;

#endif /* OSGNODE_H_ */
