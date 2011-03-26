/*
 * BasicButtons.cpp
 *
 *  Created on: 7.1.2011
 *      Author: jakub
 */

#include "Viewer/BasicButtons.h"
#include "Viewer/ControlFrame.h"
#include "Viewer/AbstractNode.h"
#include "Viewer/SceneGraph.h"
#include "Viewer/Visitors.h"
#include "Viewer/OsgCluster.h"
#include "Util/CameraHelper.h"
#include <QApplication>

using namespace Vwr;

NullButton::NullButton(ControlFrame* parentFrame) : FrameButton(parentFrame) {
	setName("null_button");
}


MoveButton::MoveButton(ControlFrame* parentFrame, int x, int y)
	: FrameButton(parentFrame, "img/texture/b_move.png", osg::Vec2f(x, y)) {
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


ResizeButton::ResizeButton(ControlFrame* parentFrame, int x, int y)
: FrameButton(parentFrame, "img/texture/b_resize.png", osg::Vec2f(x, y)) {
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


HideButton::HideButton(ControlFrame* parentFrame, int x, int y)
	: FrameButton(parentFrame, "img/texture/b_hide.png", osg::Vec2f(x, y)) {
	setName("hide_button");
}
void HideButton::handlePush() {
	frame->hide();
	frame->setNode(NULL);
}


FixButton::FixButton(ControlFrame* parentFrame, int x, int y)
	: FrameButton(parentFrame, "img/texture/b_lock.png", osg::Vec2f(x, y)) {
	setName("fix_button");
}
void FixButton::handlePush() {
	frame->getNode()->setFixed(!frame->getNode()->isFixed());
}


ExpandButton::ExpandButton(ControlFrame* parentFrame, int x, int y)
	: FrameButton(parentFrame, "img/texture/b_expand.png", osg::Vec2f(x, y)) {
	setName("expand_button");
}
void ExpandButton::handlePush() {
	AbstractVisitor* v = new ExpanderVisitor(true);
	frame->getNode()->acceptVisitor(v);
	delete v;
}


CompactButton::CompactButton(ControlFrame* parentFrame, int x, int y)
	: FrameButton(parentFrame, "img/texture/b_compact.png", osg::Vec2f(x, y)) {
	setName("compact_button");
}
void CompactButton::handlePush() {
	AbstractVisitor* v = new ExpanderVisitor(false);
	frame->getNode()->acceptVisitor(v);
	delete v;
}


XRayButton::XRayButton(ControlFrame* parentFrame, int x, int y)
	: FrameButton(parentFrame, "img/texture/b_xray.png", osg::Vec2f(x, y)) {
	setName("xray_button");
}
void XRayButton::handlePush() {
	frame->getNode()->toggleContent(false);
}
void XRayButton::handleRelease() {
	frame->getNode()->toggleContent(true);
}


UnclusterButton::UnclusterButton(ControlFrame* parentFrame, int x, int y)
	: FrameButton(parentFrame, "img/texture/b_expand.png", osg::Vec2f(x, y)) {
	setName("uncluster_button");
}
void UnclusterButton::handlePush() {
	AbstractNode* nodes = frame->getNode()->uncluster();
	frame->setNode(nodes);
}


ClusterButton::ClusterButton(ControlFrame* parentFrame, int x, int y)
	: FrameButton(parentFrame, "img/texture/b_compact.png", osg::Vec2f(x, y)) {
	setName("cluster_button");
}
void ClusterButton::handlePush() {
	AbstractNode* topCluster = frame->getNode()->cluster();
	frame->addNode(topCluster);
}
