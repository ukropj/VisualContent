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
}

void Cluster::setIgnored(bool flag) {
	if (isIgnored() == flag)
		return;
	Node::setIgnored(flag);
	if (parent != NULL) {
		parent->expandedChClusters += flag ? +1 : -1;
//		qDebug() << parent->toString() << " " << parent->expandedChClusters;
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
	if (this->isIgnored())
		newParent->expandedChClusters++;
}

bool Cluster::canCluster() {
	if (!isIgnored())
		return false;
	if (expandedChClusters > 0) {
//		qDebug() << "Unable to cluster, ecc: " << parent->expandedChClusters;
		return false;
	}
	return true;
}

bool Cluster::clusterChildren() {
	if (expandedChClusters > 0)
		return false;

	setIgnored(false);
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
	if (children.isEmpty())
		return false;
	QSetIterator<Node*> nodeIt = getChildrenIterator();
	while (nodeIt.hasNext()) {
		Node* n = nodeIt.next();
		n->setPosition(getPosition());
		n->setIgnored(false);
	}
	setIgnored(true);
	graph->setFrozen(false);
	return true;
}
