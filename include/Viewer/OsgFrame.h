/*
 * OsgFrame.h
 *
 *  Created on: 4.1.2011
 *      Author: jakub
 */

#ifndef OSGFRAME_H_
#define OSGFRAME_H_

#include <osg/AutoTransform>
#include <osg/PositionAttitudeTransform>
#include <osgGA/GUIEventAdapter>

#include <string>

#include <QString>
#include <QMap>

namespace Vwr {
class OsgNode;
class FrameButton;

class OsgFrame : public osg::AutoTransform {
public:
	OsgFrame();
	~OsgFrame();
	void show(OsgNode* node);
	void hide();
	bool isShowing() {
		return refNode != NULL;
	}
	OsgNode* getNode() const {return refNode;}
	void updatePosition();
	bool activateAction(osg::Geode* button);
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
	osg::Vec2f originPos;
	osg::Vec2f currentPos;
	osg::Vec2f lastDragPos;

	OsgNode* refNode;

	QMap<std::string, FrameButton*> buttons;
	FrameButton* activeButton;
	FrameButton* nullButton;

	osg::ref_ptr<osg::AutoTransform> mt;
	osg::ref_ptr<osg::AutoTransform> mt2;
};

}

#endif /* OSGFRAME_H_ */
