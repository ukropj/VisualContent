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

	qlonglong getId() const;
	QString getName() const;
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

	QString toString() const {
		QString str;
		osg::Vec3f pos = getPosition();
		QTextStream(&str) << "N" << getId() << " " << getName() << "["
				<< pos.x() << "," << pos.y() << "," << pos.z() << "]"
				<< (isFixed() ? "fixed" : "");
		return str;
	}

	void setColor(osg::Vec4 color);
	osg::Vec4 getColor() const {
		return color;
	}

	float getRadius() const;
	void showLabel(bool visible);

	bool isUsingInterpolation() const {
		return usingInterpolation;
	}

	void setUsingInterpolation(bool val) {
		usingInterpolation = val;
	}

	void reloadConfig();

	bool isObscuredBy(OsgNode* other);

private:
	Model::Node* node;

	SceneGraph* sceneGraph;

	bool selected;

	bool usingInterpolation;

	bool expanded;

	void setDrawableColor(osg::Vec4 color);

	osg::ref_ptr<osg::StateSet> createStateSet();

	osg::ref_ptr<osg::Geode> createTextureNode(
			osg::ref_ptr<osg::Texture2D> texture, const float scale = 1);
	osg::ref_ptr<osg::Geode> createLabel(QString text, const float scale = 1);
	osg::ref_ptr<osg::Geode> createSquare(const float scale = 1);
	osg::ref_ptr<osg::Geode> createCircle(const float scale = 1);

	osg::Vec4 color;

	osg::ref_ptr<osg::AutoTransform> nodeTransform;

	osg::ref_ptr<osg::Geode> label;
	osg::ref_ptr<osg::Geode> square;
	osg::ref_ptr<osg::Geode> circle;

	osg::ref_ptr<osg::Geode> nodeSmall;
	osg::ref_ptr<osg::Geode> nodeLarge;
};
}

#endif /* OSGNODE_H_ */
