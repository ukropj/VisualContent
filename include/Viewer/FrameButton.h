/*
 * FrameButton.h
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#ifndef FRAMEBUTTON_H_
#define FRAMEBUTTON_H_

#include <osg/Geode>
#include <osg/Geometry>

#include <QString>

namespace Vwr {
class ControlFrame;

class FrameButton : public osg::Geode {
public:
	FrameButton(ControlFrame* parentFrame);
	FrameButton(ControlFrame* parentFrame, osg::Vec3f pos, QString imagePath);
	~FrameButton();

	virtual void handlePush() {}
	virtual void handleDoubleclick() {}
	virtual void handleDrag() {}
	virtual void handleMove() {}
	virtual void handleRelease() {}
	virtual void handleKeyDown() {}
	virtual void handleKeyUp() {}

	virtual void activate() {}
	virtual void deactivate() {}
protected:
	ControlFrame* frame;
};

}

#endif /* FRAMEBUTTON_H_ */
