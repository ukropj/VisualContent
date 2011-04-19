/*
 * BasicButtons.h
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#ifndef BASICBUTTONS_H_
#define BASICBUTTONS_H_

#include "GUIControls/FrameButton.h"
#include <osg/Vec2f>
#include <osg/Vec3f>

namespace Controls {

class NullButton : public FrameButton {
public:
	NullButton(ControlFrame* parentFrame);
	void activate() {};
	void deactivate() {};
	void setEnabled(bool flag) {};
	static std::string name() {return "null_button";}
};

class MoveButton : public FrameButton {
public:
	MoveButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleDrag();
	void handleRelease();
	static std::string name() {return "move_button";}
};

class ResizeButton : public FrameButton {
public:
	ResizeButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleDrag();
	void handleRelease();
	static std::string name() {return "resize_button";}
};

class HideButton : public FrameButton {
public:
	HideButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "hide_button";}
};

class FixButton : public FrameButton {
public:
	FixButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "fix_button";}
};

class ExpandButton : public FrameButton {
public:
	ExpandButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "expand_button";}
};

class CompactButton : public FrameButton {
public:
	CompactButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "compact_button";}
};

class XRayButton : public FrameButton {
public:
	XRayButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleRelease();
	static std::string name() {return "xray_button";}
};

class UnclusterButton : public FrameButton {
public:
	UnclusterButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "uncluster_button";}
};

class ClusterButton : public FrameButton {
public:
	ClusterButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "cluster_button";}
};

}

#endif /* BASICBUTTONS_H_ */
