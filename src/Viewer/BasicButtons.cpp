/*
 * BasicButtons.cpp
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#include "Viewer/BasicButtons.h"
#include "Viewer/OsgFrame.h"
#include "Viewer/AbstractNode.h"
#include "Util/CameraHelper.h"
#include <QApplication>

using namespace Vwr;

NullButton::NullButton(OsgFrame* parentFrame) : FrameButton(parentFrame) {
	setName("null_button");
}


MoveButton::MoveButton(OsgFrame* parentFrame, osg::Vec3f pos)
	: FrameButton(parentFrame, pos, "img/texture/b_move.png") {
	setName("move_button");
}
void MoveButton::handlePush() {
	frame->getNode()->setFrozen(true);
	QApplication::setOverrideCursor(Qt::SizeAllCursor);
}
void MoveButton::handleDrag() {
	osg::Vec2f dragVec = frame->getCurrentPos() - frame->getLastDragPos();
	frame->getNode()->setPosition(
			Util::CameraHelper::moveByScreenVector(frame->getNode()->getPosition(), dragVec));
}
void MoveButton::handleRelease() {
	frame->getNode()->setFrozen(false);
	QApplication::restoreOverrideCursor();
}
void MoveButton::activate() {
}
void MoveButton::deactivate() {
}


ResizeButton::ResizeButton(OsgFrame* parentFrame, osg::Vec3f pos)
	: FrameButton(parentFrame, pos, "img/texture/b_resize.png") {
	setName("resize_button");
}
void ResizeButton::handlePush() {
	frame->getNode()->setFrozen(true);
	QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
}
void ResizeButton::handleDrag() {
	osg::Vec2f nodePos = Util::CameraHelper::worldToScreen(frame->getNode()->getPosition());
	osg::Vec2f newVec = frame->getCurrentPos() - nodePos;
	osg::Vec2f oldVec = frame->getLastDragPos() - nodePos;
	frame->getNode()->resize(newVec.length() / oldVec.length());
}
void ResizeButton::handleRelease() {
	frame->getNode()->setFrozen(false);
	QApplication::restoreOverrideCursor();
}
void ResizeButton::activate() {
}
void ResizeButton::deactivate() {
}


HideButton::HideButton(OsgFrame* parentFrame, osg::Vec3f pos)
	: FrameButton(parentFrame, pos, "img/texture/b_hide.png") {
	setName("hide_button");
}
void HideButton::handlePush() {
	frame->hide();
	frame->setNode(NULL);
}


FixButton::FixButton(OsgFrame* parentFrame, osg::Vec3f pos)
	: FrameButton(parentFrame, pos, "img/texture/b_lock.png") {
	setName("fix_button");
}

void FixButton::handlePush() {
	frame->getNode()->setFixed(!frame->getNode()->isFixed());
}


ExpandButton::ExpandButton(OsgFrame* parentFrame, osg::Vec3f pos)
	: FrameButton(parentFrame, pos, "img/texture/b_expand.png") {
	setName("expand_button");
}

void ExpandButton::handlePush() {
	frame->getNode()->setExpanded(true);
}


CompactButton::CompactButton(OsgFrame* parentFrame, osg::Vec3f pos)
	: FrameButton(parentFrame, pos, "img/texture/b_compact.png") {
	setName("compact_button");
}

void CompactButton::handlePush() {
	frame->getNode()->setExpanded(false);
}
