/*
 * OsgNodeGroup.cpp
 *
 *  Created on: 8.1.2011
 *      Author: jakub
 */

#include "Viewer/OsgNodeGroup.h"
#include "Util/CameraHelper.h"

using namespace Vwr;

OsgNodeGroup::OsgNodeGroup() {
	selected = false;
	expanded = false;
	fixed = false;
	frozen = false;

	massCenter = osg::Vec3f(0,0,0);
	size = osg::Vec3f(0,0,0);
	nodes.clear();
}

OsgNodeGroup::~OsgNodeGroup() {
	removeAll();
}

void OsgNodeGroup::addNode(AbstractNode* node, bool removeIfPresent, bool recalc) {
	if (node == NULL)
		return;
	OsgNodeGroup* group = dynamic_cast<OsgNodeGroup*>(node);
	if (group != NULL) {
		NodeSet::const_iterator i = group->nodes.constBegin();
		while (i != group->nodes.constEnd()) {
			if (removeIfPresent && nodes.contains(*i)) {
				removeNode(*i, false);
			} else {
				addToNodes(*i);
			}
			++i;
		}
	} else {
		if (removeIfPresent && nodes.contains(node)) {
			removeNode(node, false);
		} else {
			addToNodes(node);
		}
	}

	if (recalc) {
		updateSizeAndPos();
	}

	if (selected && !node->isSelected()) selected = false;
	if (expanded && !node->isExpanded()) expanded = false;
	if (frozen && !node->isFrozen()) frozen = false;
	if (fixed && !node->isFixed()) fixed = false;
}

void OsgNodeGroup::addToNodes(AbstractNode* node) {
	nodes.insert(node);
	connect(node, SIGNAL(changedPosition(osg::Vec3f, osg::Vec3f)),
			this, SLOT(childPosChanged(osg::Vec3f, osg::Vec3f)));
	connect(node, SIGNAL(changedSize(osg::Vec3f, osg::Vec3f)),
			this, SLOT(childSizeChanged(osg::Vec3f, osg::Vec3f)));
	emit nodeAdded(node);
}

void OsgNodeGroup::removeNode(AbstractNode* node, bool recalc) {
	if (node == NULL)
		return;
	OsgNodeGroup* group = dynamic_cast<OsgNodeGroup*>(node);
	if (group != NULL) {
		NodeSet::const_iterator i = group->nodes.constBegin();
		while (i != group->nodes.constEnd()) {
			removeFromNodes(*i);
			++i;
		}
	} else {
		removeFromNodes(node);
	}

	if (recalc) {
		updateSizeAndPos();
	}
}

void OsgNodeGroup::removeAll() {
	NodeSet::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		disconnect(*i, 0, this, 0);// TODO removeFromNodes
		emit nodeRemoved(*i);
		++i;
	}
	nodes.clear();
	updateSizeAndPos();
}

void OsgNodeGroup::removeFromNodes(AbstractNode* node) {
	nodes.remove(node);
	disconnect(node, 0, this, 0);
	emit nodeRemoved(node);
}

bool OsgNodeGroup::isEmpty() {
	return nodes.isEmpty();
}

osg::Vec3f OsgNodeGroup::getPosition() const {
	return massCenter;
}

void OsgNodeGroup::setPosition(osg::Vec3f pos) {
	if (pos == getPosition())
		return;
	osg::Vec3f oldPos = getPosition();
	osg::Vec3f diff = pos - oldPos;
	NodeSet::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		qDebug() << "Node pos set in group";
		(*i)->setPosition((*i)->getPosition() + diff);
		++i;
	}
	massCenter = pos;
	qDebug() << "Group pos set";
	emit changedPosition(oldPos, getPosition());
}

void OsgNodeGroup::resize(float factor) {
	NodeSet::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		(*i)->resize(factor);
		++i;
	}
	updateSizeAndPos();
}

osg::Vec3f OsgNodeGroup::getSize() const {
	return size;
}

void OsgNodeGroup::updateSizeAndPos() {
	osg::Vec3f oldPos = getPosition();
	osg::Vec3f oldSize = getSize();
	float xMin = 10000, yMin = 10000, xMax = -10000, yMax = -10000,
			zMin = 10000, zMax = -10000;//TODO
	NodeSet::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		osg::Vec3f pos = Util::CameraHelper::byView((*i)->getPosition());
		osg::Vec3f size = (*i)->getSize() / 2.0f;
		xMin = qMin(xMin, pos.x()-size.x());
		yMin = qMin(yMin, pos.y()-size.y());
		zMin = qMin(zMin, pos.z()-size.z());
		xMax = qMax(xMax, pos.x()+size.x());
		yMax = qMax(yMax, pos.y()+size.y());
		zMax = qMax(zMax, pos.z()+size.z());
		++i;
	}
	size.set(xMax-xMin, yMax-yMin, zMax-zMin);
	massCenter.set(xMin+size.x()/2.0f, yMin+size.y()/2.0f, zMin+size.z()/2.0f);
	massCenter = Util::CameraHelper::byViewInv(massCenter);

//	qDebug() << "Group size updated";
	if (oldPos != getPosition())
		emit changedPosition(oldPos, getPosition());
	if (oldSize != getSize())
		emit changedSize(oldSize, getSize());
}

bool OsgNodeGroup::setFixed(bool flag) {
	NodeSet::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		(*i)->setFixed(flag);
		++i;
	}
	fixed = flag;
}

bool OsgNodeGroup::isFixed() const {
	return fixed;
}

void OsgNodeGroup::setFrozen(bool flag) {
	NodeSet::const_iterator i = nodes.constBegin();
//	qDebug() << "Composite frozen: " << flag;
	while (i != nodes.constEnd()) {
		(*i)->setFrozen(flag);
		++i;
	}
	frozen = flag;
}

bool OsgNodeGroup::isFrozen() const {
	return fixed;
}

bool OsgNodeGroup::setSelected(bool flag) {
	NodeSet::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		(*i)->setSelected(flag);
		++i;
	}
	selected = flag;
}

bool OsgNodeGroup::isSelected() const {
	return fixed;
}

bool OsgNodeGroup::setExpanded(bool flag) {
	NodeSet::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		(*i)->setExpanded(flag);
		++i;
	}
	expanded = flag;
}

bool OsgNodeGroup::isExpanded() const {
	return fixed;
}

void OsgNodeGroup::childPosChanged(osg::Vec3f oldPos, osg::Vec3f newPos) {
	updateSizeAndPos();
}
void OsgNodeGroup::childSizeChanged(osg::Vec3f oldSize, osg::Vec3f newSize) {
	updateSizeAndPos();
}

void OsgNodeGroup::getProjRect(float &xMin, float &yMin, float &xMax, float &yMax) {
	xMin = 10000, yMin = 10000, xMax = -10000, yMax = -10000; // TODO

	NodeSet::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		osg::Vec3f pos = Util::CameraHelper::byView((*i)->getPosition());
		osg::Vec3f size = (*i)->getSize() / 2.0f;
		size.z() = 0;

		osg::Vec3f pos1 = Util::CameraHelper::byWindow(Util::CameraHelper::byProjection(
				pos + size));
		xMax = qMax(xMax, pos1.x());
		yMax = qMax(yMax, pos1.y());

		osg::Vec3f pos2 = Util::CameraHelper::byWindow(Util::CameraHelper::byProjection(
				pos - size));
		xMin = qMin(xMin, pos2.x());
		yMin = qMin(yMin, pos2.y());

		++i;
	}
}

AbstractNode* OsgNodeGroup::merge(AbstractNode* n1, AbstractNode* n2) {
	if (n1 == NULL) return n2;
	if (n2 == NULL) return n1;
	OsgNodeGroup* group1;
	OsgNodeGroup* group2;
	group1 = dynamic_cast<OsgNodeGroup*>(n1);
	group2 = dynamic_cast<OsgNodeGroup*>(n2);
	if (group1 != NULL) {
		group1->addNode(n2, true);
		return group1;
	} else if (group2 != NULL) {
		group2->addNode(n1, true);
		return group2;
	} else {
		OsgNodeGroup* group = new OsgNodeGroup;
		group->addNode(n1);
		group->addNode(n2, true);
		return group;
	}
}













