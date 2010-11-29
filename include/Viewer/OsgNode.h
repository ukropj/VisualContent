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

	QString toString() const;

	void setColor(osg::Vec4 color);
	osg::Vec4 getColor() const {
		return color;
	}

	float getRadius() const;

	osg::Vec2f getSize() const {
		return size;
	}

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

	float getDistanceToEdge(double angle);

private:

	void setSize(osg::BoundingBox box);

	osg::Vec2f size;

	Model::Node* node;

	SceneGraph* sceneGraph;

	bool selected;

	bool usingInterpolation;

	bool expanded;

	void setDrawableColor(osg::ref_ptr<osg::Geode> geode, int drawablePos,
			osg::Vec4 color);

	osg::ref_ptr<osg::StateSet> createStateSet();

	osg::ref_ptr<osg::Geode> createFrame(osg::BoundingBox box, float margin);
	osg::ref_ptr<osg::Geode> createContent();

	osg::ref_ptr<osg::Geode> createTextureNode(
			osg::ref_ptr<osg::Texture2D> texture, const float scale = 1);
	osg::ref_ptr<osg::Drawable> createRect(float width, float height, osg::Vec4f color);
	osg::ref_ptr<osg::Geode> createFixed();
	osg::ref_ptr<osg::Geode> createText(const float scale = 1);
	osg::ref_ptr<osg::Geode> createLabel(QString text, const float scale = 1);

	osg::Vec4 color;

	osg::ref_ptr<osg::AutoTransform> nodeTransform;

	osg::ref_ptr<osg::Geode> frameG;

	osg::ref_ptr<osg::Geode> label;
	static osg::ref_ptr<osg::Geode> fixedG;

	osg::ref_ptr<osg::Geode> closedG;
	osg::ref_ptr<osg::Geode> contentG;

	// constants
	static osg::Vec4 selectedColor;
//	static const osg::Vec4 fixedColor;
};
}

//const osg::Vec4 OsgNode::fixedColor;

#endif /* OSGNODE_H_ */
