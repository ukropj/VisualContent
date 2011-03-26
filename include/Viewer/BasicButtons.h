/*
 * BasicButtons.h
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#ifndef BASICBUTTONS_H_
#define BASICBUTTONS_H_

#include "Viewer/FrameButton.h"
#include <osg/Vec2f>
#include <osg/Vec3f>

namespace Vwr {

class NullButton : public FrameButton {
public:
	NullButton(ControlFrame* parentFrame);
};

class MoveButton : public FrameButton {
public:
	MoveButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleDrag();
	void handleRelease();
};

class ResizeButton : public FrameButton {
public:
	ResizeButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleDrag();
	void handleRelease();
};

class HideButton : public FrameButton {
public:
	HideButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
};

class FixButton : public FrameButton {
public:
	FixButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
};

class ExpandButton : public FrameButton {
public:
	ExpandButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
};

class CompactButton : public FrameButton {
public:
	CompactButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
};

class XRayButton : public FrameButton {
public:
	XRayButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleRelease();
};

class UnclusterButton : public FrameButton {
public:
	UnclusterButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
};

class ClusterButton : public FrameButton {
public:
	ClusterButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
};

}

#endif /* BASICBUTTONS_H_ */
