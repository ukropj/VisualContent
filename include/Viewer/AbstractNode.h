/*
 * AbstractNode.h
 *
 *  Created on: 11.1.2011
 *      Author: jakub
 */

#ifndef ABSTRACTNODE_H_
#define ABSTRACTNODE_H_

#include <QObject>
#include <QSet>
#include <osg/Vec3f>

namespace Vwr {
class AbstractVisitor;

class AbstractNode : public QObject {
Q_OBJECT
signals:
	void changedPosition(osg::Vec3f oldPos, osg::Vec3f newPos);
	void changedSize(osg::Vec3f oldSize, osg::Vec3f newSize);
public:

	AbstractNode() {}
	~AbstractNode() {}

	virtual QSet<AbstractNode*> getIncidentNodes() = 0;

	virtual osg::Vec3f getPosition() const = 0;
	virtual void setPosition(osg::Vec3f pos) = 0;
	virtual void resize(float factor) = 0;
	virtual osg::Vec3f getSize() const = 0;

	virtual bool setFixed(bool flag) = 0;
	virtual bool isFixed() const = 0;
	virtual void setFrozen(bool flag) = 0;
	virtual bool isFrozen() const = 0;
	virtual bool setSelected(bool flag) = 0;
	virtual bool isSelected() const = 0;
	virtual bool setExpanded(bool flag) = 0;
	virtual bool isExpanded() const = 0;

	virtual void getProjRect(float &xMin, float &yMin, float &xMax, float &yMax) = 0;

	virtual void acceptVisitor(AbstractVisitor* visitor) = 0;
};

}

#endif /* ABSTRACTNODE_H_ */
