#ifndef BASICBUTTONS_H_
#define BASICBUTTONS_H_

#include "GUIControls/FrameButton.h"
#include <osg/Vec2f>
#include <osg/Vec3f>

namespace Controls {

/// FrameButton implementation used as NULL object.
class NullButton : public FrameButton {
public:
	NullButton(ControlFrame* parentFrame);
	void activate() {};
	void deactivate() {};
	void setEnabled(bool flag) {};
	static std::string name() {return "null_button";}
};
/// FrameButton implementation used to move selected node.
class MoveButton : public FrameButton {
public:
	MoveButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleDrag();
	void handleRelease();
	static std::string name() {return "move_button";}
};
/// FrameButton implementation used to resize selected node.
class ResizeButton : public FrameButton {
public:
	ResizeButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleDrag();
	void handleRelease();
	static std::string name() {return "resize_button";}
};
/// FrameButton implementation used to unselect selected node.
class HideButton : public FrameButton {
public:
	HideButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "hide_button";}
};
/// FrameButton implementation used to fix position of selected node.
class FixButton : public FrameButton {
public:
	FixButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "fix_button";}
};
/// FrameButton implementation used to show contents of selected node.
class ExpandButton : public FrameButton {
public:
	ExpandButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "expand_button";}
};
/// FrameButton implementation used to hide contents of selected node.
class CompactButton : public FrameButton {
public:
	CompactButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "compact_button";}
};
/// FrameButton implementation used to make selected node transparent.
class XRayButton : public FrameButton {
public:
	XRayButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	void handleRelease();
	static std::string name() {return "xray_button";}
};
/// FrameButton implementation used to explode selected cluster.
class UnclusterButton : public FrameButton {
public:
	UnclusterButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "uncluster_button";}
};
/// FrameButton implementation used to cluster selected node.
class ClusterButton : public FrameButton {
public:
	ClusterButton(ControlFrame* parentFrame, int x = 0, int y = 0);
	void handlePush();
	static std::string name() {return "cluster_button";}
};

}

#endif /* BASICBUTTONS_H_ */
