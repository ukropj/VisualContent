/*
 * Clusterer.cpp
 *
 *  Created on: 5.4.2011
 *      Author: jakub
 */

#include "Model/Clusterer.h"
#include "Model/Graph.h"
#include "Model/Node.h"
#include "Model/Type.h"
#include "Model/Cluster.h"

using namespace Model;

Clusterer::Clusterer() {
	graph = NULL;
	clusterType = NULL;
	alg = NEIGHBORS;
}

bool Clusterer::setClusteringAlg(int i) {
	ClusteringAlg newAlg = NONE;
	switch (i) {
		case 0: newAlg = NONE; break;
		case 1: newAlg = NEIGHBORS; break;
		case 2: newAlg = LEAFS; break;
		case 3: newAlg = MINCUT; break;
	}
	if (newAlg == alg) {
		return false;
	} else {
		alg = newAlg;
		return true;
	}
}

void Clusterer::cluster(Graph* graph) {
	this->graph = graph;
	clusters.clear();

	switch (alg) {
	default:
	case NONE:
		break;
	case NEIGHBORS:
		clusterNghbrs(*(graph->getNodes()), true, 2);
		break;
	case LEAFS:
		clusterLeafs(*(graph->getNodes()), true, -1);
		break;
	case MINCUT:
//		clusterMincut();
		break;
	}
}

void Clusterer::clusterNghbrs(QMap<qlonglong, Node* > someNodes, bool clustersVisible, int maxLevels) {
//	qDebug() << "clustering starts " << someNodes.size();
	for (NodeIt ui = someNodes.begin(); ui != someNodes.end(); ++ui) {
		Node* u = ui.value();
//		qDebug() << "u: " << u->getId();
		if (u->getParent() == NULL) {
			Cluster* c = NULL;
			QSet<Node*> in = u->getIncidentNodes();
			QSet<Node*>::const_iterator i = in.begin();
			while (i != in.end()) {
				Node* v = *i;
//				qDebug() << "v: " << v->getId();
				if (!clusters.contains(v->getId()) && v->getParent() == NULL) {
					if (c == NULL) {
						c = addCluster();
//						qDebug() << "new c: " << c->getId();
					}
					v->setParent(c);
					v->setIgnored(clustersVisible);
//					qDebug() << "v added to c";
				}
				++i;
			}
			if (c != NULL) {
				u->setParent(c);
				c->setIgnored(!clustersVisible);
				c->setExpanded(!clustersVisible);
				u->setIgnored(clustersVisible);
//				qDebug() << "u added to c";
			}
		}
	}
	for (NodeIt ci = clusters.begin(); ci != clusters.end(); ++ci) {
		Node* c = ci.value();
		graph->nodes.insert(c->getId(), c);
		graph->nodesByType.insert(c->getType()->getId(), c);
	}

	if (clusters.size() > 1 && maxLevels != 0) {
		QMap<qlonglong, Node*> newNodes(clusters);
		clusters.clear();
		clusterNghbrs(newNodes, clustersVisible, maxLevels - 1);
	}
//	qDebug() << "clustering ends " << clusters.size() << "/" <<  graph->nodes.size();
}

void Clusterer::clusterLeafs(QMap<qlonglong, Node* > someNodes, bool clustersVisible, int maxLevels) {
//	qDebug() << "clustering starts " << someNodes.size();
	for (NodeIt ui = someNodes.constBegin(); ui != someNodes.constEnd(); ++ui) {
		Node* u = ui.value();
//		qDebug() << "u: " << u->getId();
		if (u->getParent() == NULL) {
			QSet<Node*> in = u->getIncidentNodes();
//			qDebug() << "u nghbrs: " << in.size();
			if (in.size() == 1) {
				Node* v = *(in.constBegin());
//				qDebug() << "v: " << v->getId();

				Cluster* c = dynamic_cast<Cluster*>(clusters.value(v->getId()));
				if (c == NULL) {
					c = addCluster();
//					qDebug() << "new c: " << c->getId();
					v->setParent(c);
					v->setIgnored(clustersVisible);
//					qDebug() << "v added to c";

					c->setIgnored(!clustersVisible);
					c->setExpanded(!clustersVisible);
				}
				u->setParent(c);
				u->setIgnored(clustersVisible);
//				qDebug() << "u added to c";

			}
		}
	}
	for (NodeIt ci = clusters.begin(); ci != clusters.end(); ++ci) {
			Node* c = ci.value();
			graph->nodes.insert(c->getId(), c);
			graph->nodesByType.insert(c->getType()->getId(), c);
	}

	if (clusters.size() > 1 && maxLevels != 0) {
		QMap<qlonglong, Node*> newNodes(clusters);
		clusters.clear();
		clusterLeafs(newNodes, clustersVisible, maxLevels - 1);
	}
//	qDebug() << "clustering ends, clusters=" << clusters.size() << " level=" << maxLevels << " " << clusters.size() << "/" <<  graph->nodes.size();
}


Cluster* Clusterer::addCluster() { // TODO cluster type
	if (clusterType == NULL) {
		clusterType = graph->addType("cluster");
	}
	Cluster* node = new Cluster(graph->incEleIdCounter(), clusterType, graph);

	clusters.insert(node->getId(), node);
	return node;
}
