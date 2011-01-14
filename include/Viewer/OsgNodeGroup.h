/*
 * OsgNodeGroup.h
 *
 *  Created on: 8.1.2011
 *      Author: jakub
 */

#ifndef OSGNODEGROUP_H_
#define OSGNODEGROUP_H_

#include "Viewer/AbstractNode.h"
#include <QSet>
#include <osg/Vec3f>

namespace Vwr {

class OsgNodeGroup : public AbstractNode {
	Q_OBJECT

signals:
	void nodeAdded(AbstractNode* node);
	void nodeRemoved(AbstractNode* node);
public slots:
	void childPosChanged(osg::Vec3f oldPos, osg::Vec3f newPos);
	void childSizeChanged(osg::Vec3f oldSize, osg::Vec3f newSize);
public:
	OsgNodeGroup();
	~OsgNodeGroup();

	void addNode(AbstractNode* node, bool removeIfPresent = false, bool recalc = true);
	void removeNode(AbstractNode* node, bool recalc = true);
	void removeAll();
	bool isEmpty();

	osg::Vec3f getPosition() const;
	void setPosition(osg::Vec3f pos);
	void updatePosition();
	void resize(float factor);
	osg::Vec3f getSize() const;
	void updateSize();

	bool setFixed(bool flag);
	bool isFixed() const;
	void setFrozen(bool flag);
	bool isFrozen() const;
	bool setSelected(bool flag);
	bool isSelected() const;
	bool setExpanded(bool flag);
	bool isExpanded() const;

//	bool isResizable(osg::Geode* geode) const;

	static AbstractNode* merge(AbstractNode* n1, AbstractNode* n2);

private:
	void addToNodes(AbstractNode* node);
	void removeFromNodes(AbstractNode* node);


	NodeSet nodes;
	osg::Vec3f massCenter;
	osg::Vec3f size;

	bool selected;
	bool expanded;
	bool fixed;
	bool frozen;
};

}

#endif /* COMPOSITENODE_H_ */
