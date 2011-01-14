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
	NullButton(OsgFrame* parentFrame);
};

class MoveButton : public FrameButton {
public:
	MoveButton(OsgFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
	void handleDrag();
	void handleRelease();

	void activate();
	void deactivate();
};

class ResizeButton : public FrameButton {
public:
	ResizeButton(OsgFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
	void handleDrag();
	void handleRelease();

	void activate();
	void deactivate();
};

class HideButton : public FrameButton {
public:
	HideButton(OsgFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
};

class FixButton : public FrameButton {
public:
	FixButton(OsgFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
};

class ExpandButton : public FrameButton {
public:
	ExpandButton(OsgFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
};

class CompactButton : public FrameButton {
public:
	CompactButton(OsgFrame* parentFrame, osg::Vec3f pos);
	void handlePush();
};

}

#endif /* BASICBUTTONS_H_ */
