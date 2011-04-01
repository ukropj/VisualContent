/*
 * Cluster.cpp
 *
 *  Created on: 9.3.2011
 *      Author: jakub
*/

#include "Model/Cluster.h"
#include "Model/Type.h"
#include "Model/Edge.h"
#include "Model/Graph.h"

#include "Viewer/OsgNode.h"

using namespace Model;

Cluster::Cluster(qlonglong id, Type* type, Graph* graph)
	: Node(id, type, NULL, graph) {
	weight = 0;
	expandedChClusters = 0;
	expanded = true;
}

void Cluster::setExpanded(bool flag) {
	if (expanded == flag)
		return;
	expanded = flag;
	if (parent != NULL) {
		parent->expandedChClusters += expanded ? +1 : -1;
		qDebug() << parent->toString() << " " << parent->expandedChClusters;
	}
}

QSet<Node*> Cluster::getIncidentNodes(bool getClusters) const {
	QSet<Node*> nodes;
	QSetIterator<Node*> nodeIt = getChildrenIterator();
	while (nodeIt.hasNext()) {
		nodes.unite(nodeIt.next()->getIncidentNodes());
	}
	nodes.subtract(children);
	if (getClusters) {
		QSet<Node*> clusters;
		QSetIterator<Node*> nodeIt(nodes);
		while (nodeIt.hasNext()) {
			Node* parentN = nodeIt.next()->getParent();
			if (parentN != NULL && parentN != this && parentN != parent)
				clusters.insert(parentN);
		}
		nodes.unite(clusters);
	}
	return nodes;
}

void Cluster::setParent(Cluster* newParent) {
	Node::setParent(newParent);
	if (isExpanded())
		newParent->expandedChClusters++;
}

bool Cluster::canCluster() {
	if (!isIgnored() || !isExpanded())
		return false;
	if (expandedChClusters > 0) {
//		qDebug() << "Unable to cluster, ecc: " << parent->expandedChClusters;
		return false;
	}
	return true;
}

bool Cluster::clusterChildren() {
	if (!canCluster())
		return false;

	setIgnored(false);
	// NOTE: don't call setExpanded(false) here - wait for all children to reach this cluster
	// see OsgCluster::moveChildIn()
	osg::Vec3f pos(0, 0, 0);
	QSetIterator<Node*> nodeIt = getChildrenIterator();
	while (nodeIt.hasNext()) {
		Node* n = nodeIt.next();
		n->setIgnored(true);
		pos += n->getPosition();
	}
	setPosition(pos / children.size());
	graph->setFrozen(false);
	return true;
}

bool Cluster::unclusterChildren() {
	if (isIgnored() || isExpanded() || children.isEmpty())
		return false;
	QSetIterator<Node*> nodeIt = getChildrenIterator();
	while (nodeIt.hasNext()) {
		Node* n = nodeIt.next();
		n->setPosition(getPosition());
		n->setIgnored(false);
	}
	setIgnored(true);
	setExpanded(true);
	graph->setFrozen(false);
	return true;
}
