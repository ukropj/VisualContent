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
#include <osgText/Text>

namespace Model {
class Node;
}

namespace Vwr {
class SceneGraph;
class OsgContent;

class OsgNode: public osg::Switch {
public:

	OsgNode(Model::Node* node, SceneGraph* sceneGraph, osg::ref_ptr<
			osg::AutoTransform> nodeTansform);

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

	bool isPickable(osg::Geode* geode) const;
	bool isResizable(osg::Geode* geode) const;

	QString toString() const;

	void setColor(osg::Vec4 color);
	osg::Vec4 getColor() const {
		return color;
	}

	float getRadius() const;

//	osg::Vec2f getSize() const {
//		return size;
//	}

	void showLabel(bool visible);

	bool isUsingInterpolation() const {
		return usingInterpolation;
	}

	void setUsingInterpolation(bool val) {
		usingInterpolation = val;
	}

	void reloadConfig();

	bool isOnScreen() const;

	osg::Vec3f getEye() const;
	osg::Vec3f getUpVector() const;

	static bool mayOverlap(OsgNode* u, OsgNode* v);

	float getDistanceToEdge(double angle);

	void setPickable(bool flag) {
		pickable = flag;
	}

	void resize(float factor);

	static osg::Vec4 selectedColor;

private:

	void setSize(osg::BoundingBox box);
	void setSize(float width, float height);

	osg::Vec2f size;

	Model::Node* node;

	SceneGraph* sceneGraph;

	bool selected;

	bool usingInterpolation;

	bool pickable;

	bool expanded;

	void setDrawableColor(osg::ref_ptr<osg::Geode> geode, int drawablePos,
			osg::Vec4 color);

	osg::ref_ptr<osg::StateSet> createStateSet();

	osg::ref_ptr<osg::Geode> createFrame(osg::BoundingBox box, float margin);
	void updateFrame(osg::ref_ptr<osg::Geode> frame, osg::BoundingBox box, float margin = 0);
	OsgContent* createContent();

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

	// constants

//	static const osg::Vec4 fixedColor;
};
}

//const osg::Vec4 OsgNode::fixedColor;

#endif /* OSGNODE_H_ */
