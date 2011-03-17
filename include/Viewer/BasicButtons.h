/*
 * BasicButtons.h
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#ifndef BASICBUTTONS_H_
#define BASICBUTTONS_H_

#include "Viewer/FrameButton.h"

namespace Vwr {

class NullButton : public FrameButton {
public:
	NullButton(ControlFrame* parentFrame);
};

class MoveButton : public FrameButton {
public:
	MoveButton(ControlFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
	void handleDrag();
	void handleRelease();

	void activate();
	void deactivate();
};

class ResizeButton : public FrameButton {
public:
	ResizeButton(ControlFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
	void handleDrag();
	void handleRelease();

	void activate();
	void deactivate();
};

class HideButton : public FrameButton {
public:
	HideButton(ControlFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
};

class FixButton : public FrameButton {
public:
	FixButton(ControlFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
};

class ExpandButton : public FrameButton {
public:
	ExpandButton(ControlFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
};

class CompactButton : public FrameButton {
public:
	CompactButton(ControlFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
};

}

#endif /* BASICBUTTONS_H_ */
