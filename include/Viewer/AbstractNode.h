#ifndef ABSTRACTNODE_H_
#define ABSTRACTNODE_H_

#include <QObject>
#include <QString>
#include <QSet>
#include <osg/Vec3f>

/// Graphic representations of graph elements, visualization control and other related classes.
namespace Vwr {
class AbstractVisitor;

/// Abstract parent of OsgNode and OsgNodeGroup, base of the composite pattern.
class AbstractNode : public QObject {
Q_OBJECT
signals:
	void changedPosition(osg::Vec3f oldPos, osg::Vec3f newPos);
	void changedSize(osg::Vec3f oldSize, osg::Vec3f newSize);
	void changedVisibility(bool visible);
public:

	AbstractNode() {}
	~AbstractNode() {}

	virtual QSet<AbstractNode*> getIncidentNodes() = 0;

	virtual osg::Vec3f getPosition() const = 0;
	virtual void setPosition(osg::Vec3f pos) = 0;
	virtual void resize(float factor) = 0;
	virtual osg::Vec3f getSize() const = 0;

	virtual void setFixed(bool flag) = 0;
	virtual bool isFixed() const = 0;
	virtual void setFrozen(bool flag) = 0;
	virtual bool isFrozen() const = 0;
	virtual void setSelected(bool flag) = 0;
	virtual bool isSelected() const = 0;
	virtual void setExpanded(bool flag) = 0;
	virtual bool isExpanded() const = 0;

	virtual void setVisible(bool flag) = 0;
	virtual bool isVisible() const = 0;
	virtual void toggleContent(bool flag) = 0;
	virtual bool isClusterable() const = 0;
	virtual AbstractNode* clusterToParent() = 0;
	virtual AbstractNode* uncluster(bool returnResult = true) = 0;

	virtual void getProjRect(float &xMin, float &yMin, float &xMax, float &yMax) = 0;

	virtual void acceptVisitor(AbstractVisitor* visitor) = 0;

	virtual QString toString() const = 0;
	virtual bool equals(const AbstractNode* other) const = 0;
};

}

#endif /* ABSTRACTNODE_H_ */
