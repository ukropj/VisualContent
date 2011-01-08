/*
 * OsgFrame.h
 *
 *  Created on: 4.1.2011
 *      Author: jakub
 */

#ifndef OSGFRAME_H_
#define OSGFRAME_H_

#include <QString>
#include <osg/AutoTransform>
#include <osg/PositionAttitudeTransform>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgViewer/Viewer>

namespace Vwr {
class OsgNode;

class OsgFrame : public osg::AutoTransform {
public:
	enum ButtonType {
		NONE = 0, MOVE = 1, RESIZE = 2, HIDE = 3, FIX = 4
	};
	OsgFrame();
	~OsgFrame();
	void show(OsgNode* node);
	void hide();
	bool isShowing() {
		return refNode != NULL;
	}
	bool isActivated() {
		return currentAction != NONE;
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
private:

	osg::ref_ptr<osg::Geode> createButton(ButtonType type, osg::Vec3f pos, QString imagePath);

	osg::Vec2f lastPos;
	osg::Vec2f originPos;
	OsgNode* refNode;
	ButtonType currentAction;
	osg::ref_ptr<osg::AutoTransform> mt;
	osg::ref_ptr<osg::AutoTransform> mt2;
};

}

#endif /* OSGFRAME_H_ */
