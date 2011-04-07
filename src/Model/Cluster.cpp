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
//		qDebug() << parent->toString() << " " << parent->expandedChClusters;
	}
}

QSet<Node*> Cluster::getIncidentNodes(bool noClusters) const {
	QSet<Node*> nodes;
	if (isExpanded())
		return nodes;
	QSetIterator<Node*> nodeIt = getChildrenIterator();
	while (nodeIt.hasNext()) {
		nodes.unite(nodeIt.next()->getIncidentNodes(noClusters));
	}
	nodes.subtract(children);

	Cluster* n = const_cast<Cluster*>(this);
	nodes.remove(n);
	nodes.remove(n);

//	if (noClusters) {
		return nodes;
//	} else {
//		QSet<Node*> visibleNodes;
//		QSetIterator<Node*> nodeIt(nodes);
//		while (nodeIt.hasNext()) {
//			Node* node = nodeIt.next();
//			Node* cluster = node->getTopCluster();
//			if (cluster != NULL && cluster != this) {
//				visibleNodes.insert(cluster);
//			} else {
//				visibleNodes.insert(node);
//			}
//		}
//		return visibleNodes;
//	}
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
