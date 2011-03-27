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
	FrameButton(ControlFrame* parentFrame, QString imagePath, osg::Vec2f relativePos);
	FrameButton(ControlFrame* parentFrame, QString imagePath, osg::Vec3f pos, osg::Vec2f size);
	~FrameButton();

	bool isEnabled() const;
	virtual void setEnabled(bool enabled);

	virtual void handlePush() {}
	virtual void handleDoubleclick() {}
	virtual void handleDrag() {}
	virtual void handleMove() {}
	virtual void handleRelease() {}
	virtual void handleKeyDown() {}
	virtual void handleKeyUp() {}

	virtual void activate();
	virtual void deactivate();
protected:
	ControlFrame* frame;

private:
	bool enabled;

	static float BUTTON_SIZE;
	static float BUTTON_MARGIN;
	static osg::Vec4f ENABLED_COLOR;
	static osg::Vec4f DISABLED_COLOR;
	static osg::Vec4f SELECTED_COLOR;

	void createGeometry(QString imagePath, osg::Vec3f pos, osg::Vec2f size);
	void setColor(osg::Vec4f color);
};

}

#endif /* FRAMEBUTTON_H_ */
