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

	bool handlePush(const osgGA::GUIEventAdapter& event,
			osgViewer::Viewer* viewer);
	bool handleDoubleclick(const osgGA::GUIEventAdapter& event,
			osgViewer::Viewer* viewer);
	bool handleDrag(const osgGA::GUIEventAdapter& event,
			osgViewer::Viewer* viewer);
	bool handleMove(const osgGA::GUIEventAdapter& event,
			osgViewer::Viewer* viewer);
	bool handleRelease(const osgGA::GUIEventAdapter& event,
			osgViewer::Viewer* viewer);
	bool handleKeyDown(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);
	bool handleKeyUp(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);
private:
	// Store mouse xy location for button press & move events.

	osg::ref_ptr<osg::Geode> createButton(ButtonType type, osg::Vec3f pos, osg::Vec4f color);
	osg::Vec2f toScreenCoordinates(osg::Vec3f scenePos, osgViewer::Viewer* viewer);
	osg::Vec3f getMousePos(osg::Vec3f origPos, osg::Vec2f dragVector,
			osgViewer::Viewer* viewer);

	osg::Vec2f lastPos;
	osg::Vec2f originPos;
	OsgNode* refNode;
	ButtonType currentAction;
	osg::ref_ptr<osg::PositionAttitudeTransform> mt;
};

}

#endif /* OSGFRAME_H_ */
