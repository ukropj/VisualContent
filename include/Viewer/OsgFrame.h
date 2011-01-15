/*
 * OsgFrame.h
 *
 *  Created on: 4.1.2011
 *      Author: jakub
 */

#ifndef OSGFRAME_H_
#define OSGFRAME_H_

#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>
#include <osg/Geode>
#include <osgGA/GUIEventAdapter>

#include <string>

#include <QObject>
#include <QString>
#include <QMap>

namespace Vwr {
class AbstractNode;
class FrameButton;

class OsgFrame: public QObject, public osg::PositionAttitudeTransform {
Q_OBJECT
public slots:
	void nodeAdded(AbstractNode* node);
	void nodeRemoved(AbstractNode* node);
public:
	OsgFrame();
	~OsgFrame();
	void show();
	void hide();
	void setNode(AbstractNode* node);
	AbstractNode* getNode() {
		return myNode;
	}
	bool isNodeSet();
	bool isActive() {
		return activeButton != nullButton;
	}
	void updateGeometry();
	void updateProjection();
	void activateAction(osg::Geode* button);
	void deactivateAction();

	bool handlePush(const osgGA::GUIEventAdapter& event);
	bool handleDoubleclick(const osgGA::GUIEventAdapter& event);
	bool handleDrag(const osgGA::GUIEventAdapter& event);
	bool handleMove(const osgGA::GUIEventAdapter& event);
	bool handleRelease(const osgGA::GUIEventAdapter& event);
	bool handleKeyDown(const osgGA::GUIEventAdapter& event);
	bool handleKeyUp(const osgGA::GUIEventAdapter& event);

	osg::Vec2f getPushPos() {
		return originPos;
	}
	osg::Vec2f getCurrentPos() {
		return currentPos;
	}
	osg::Vec2f getLastDragPos() {
		return lastDragPos;
	}
private:
	void createButtons();
	void createBorder();
	void insertButton(FrameButton* button, osg::Transform* transform);

	osg::Vec2f originPos;
	osg::Vec2f currentPos;
	osg::Vec2f lastDragPos;

	AbstractNode* myNode;

	QMap<std::string, FrameButton*> buttons;
	FrameButton* activeButton;
	FrameButton* nullButton;

	osg::ref_ptr<osg::AutoTransform> mt;
	osg::ref_ptr<osg::AutoTransform> mt2;
	osg::ref_ptr<osg::Geode> rect;
};

}

#endif /* OSGFRAME_H_ */
