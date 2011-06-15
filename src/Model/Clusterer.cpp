/*
 * Clusterer.cpp
 *
 *  Created on: 5.4.2011
 *      Author: jakub
 */

#include "Model/Clusterer.h"
#include "Model/Graph.h"
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Model/Type.h"
#include "Model/Cluster.h"
#include <math.h>
#include <QProgressDialog>

using namespace Model;

Clusterer::Clusterer() {
	graph = NULL;
	clusterType = NULL;
	alg = NONE;
	step = level = 0;
}

bool Clusterer::setClusteringAlg(int i) {
	ClusteringAlg newAlg = NONE;
	switch (i) {
	case 0: newAlg = NONE; break;
	case 1: newAlg = ADJACENCY; break;
	case 2: newAlg = LEAFS; break;
	case 3: newAlg = NEIGHBORS; break;
	}
	if (newAlg == alg) {
		return false;
	} else {
		alg = newAlg;
		return true;
	}
}

void Clusterer::cluster(Graph* graph, QProgressDialog* pd) {
	this->pd = pd;
	level = 0;
	this->graph = graph;
	clusterType = NULL;
	clusters.clear();

	switch (alg) {
	default:
	case NONE:
		break;
	case NEIGHBORS:
		clusterNghbrs(*(graph->getNodes()), true, 3);
		break;
	case LEAFS:
		clusterLeafs(*(graph->getNodes()), true, -1);
		break;
	case ADJACENCY:
		clusterAdjacency(*(graph->getNodes()), true, 6);
		break;
	}
}

void Clusterer::clusterNghbrs(QMap<qlonglong, Node* > someNodes, bool clustersVisible, int maxLevels) {
	pd->reset();
	pd->setLabelText(QString("Clustering graph ... (%1)").arg(++level));
	pd->setMaximum(someNodes.size()); // approximate
	step = 0;

	//	qDebug() << "clustering starts " << someNodes.size();
	for (NodeIt ui = someNodes.begin(); ui != someNodes.end(); ++ui) {
		if (pd->wasCanceled()) return;
		Node* u = ui.value();
		//		qDebug() << "u: " << u->getId();
		if (u->getParent() == NULL) {
			Cluster* c = NULL;
			QSet<Node*> in = u->getIncidentNodes();
			NIt i = in.constBegin();
			while (i != in.constEnd()) {
				Node* v = *i;
				//				qDebug() << "v: " << v->getId();
				if (!clusters.contains(v->getId()) && v->getParent() == NULL) {
					if (c == NULL) {
						c = addCluster();
						c->setIgnored(!clustersVisible);
						c->setExpanded(!clustersVisible);
						//						qDebug() << "new c: " << c->getId();
					}
					v->setParent(c);
					v->setIgnored(clustersVisible);
					pd->setValue(step++);
					//					qDebug() << "v added to c";
				}
				++i;
			}
			if (c != NULL) {
				u->setParent(c);
				u->setIgnored(clustersVisible);
				pd->setValue(step++);
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
	pd->reset();
	pd->setLabelText(QString("Clustering graph ... (%1)").arg(++level));
	pd->setMaximum(someNodes.size());
	step = 0;

	//	qDebug() << "clustering starts " << someNodes.size();
	for (NodeIt ui = someNodes.constBegin(); ui != someNodes.constEnd(); ++ui) {
		if (pd->wasCanceled()) return;
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
					pd->setValue(step++);
				}
				u->setParent(c);
				u->setIgnored(clustersVisible);
				pd->setValue(step++);
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

void Clusterer::clusterAdjacency(QMap<qlonglong, Node* > someNodes, bool clustersVisible, int maxLevels) {
	pd->reset();
	pd->setLabelText(QString("Clustering graph ... (%1)").arg(++level));
	pd->setMaximum(someNodes.size() * 3);
	step = 0;

	int n = someNodes.size();
	std::vector<bool> p(7);
	std::vector<std::vector<bool> > matrix(n, std::vector<bool>(n, false));
	std::vector<std::vector<unsigned char> > w(n, std::vector<unsigned char>(n, 0));
	// we don't use float, floats are multiplied by K and stored as unsigned char;
	unsigned char K = 100;
	int i = 0, j = 0;
	
	// prepare adjacency matrix 
	for (NodeIt ui = someNodes.constBegin(); ui != someNodes.constEnd(); ++ui, i++) {
		pd->setValue(step++);
		Node* u = ui.value();
		matrix[i][i] = true;
		QSet<Node*> nghbrs = u->getIncidentNodes();
		j = i+1;
		for (NodeIt vi = ui+1; vi != someNodes.constEnd(); ++vi, j++) {
			Node* v = vi.value();
			if (nghbrs.contains(v)) {
				matrix[i][j] = true;
				matrix[j][i] = true;
			} else {
				matrix[i][j] = false;
				matrix[j][i] = false;
			}
		}
	}

	i = 0;
	float maxW = -1;
	QString str = "\n     ";
	// prepare weight matrix w, using Pearson correlation
	for (NodeIt ui = someNodes.constBegin(); ui != someNodes.constEnd(); ++ui, i++) {
		pd->setValue(step++);
		Node* u = ui.value();
		str += QString("%1").arg(u->getId(), 5) + " ";
		w[i][i] = 0;
		int degU = u->getIncidentNodes().size();
		j = i+1;
		for (NodeIt vi = ui+1; vi != someNodes.constEnd(); ++vi, j++) {
			if (pd->wasCanceled()) return;
			Node* v = vi.value();
			int degV = v->getIncidentNodes().size();

			float sum = 0;
			for (int k = 0; k < n; k++) {
				sum += matrix[i][k] && matrix[j][k] ? 1 : 0;
			}
			// apply Pearson
			float wij = ((float)((n * sum) - (degU * degV))) /
					sqrt(degU * degV * (n - degU) * (n - degV));
			// ignore negative values
			w[j][i] = w[i][j] = qMax(0.0f, wij * K); // K is used to store 0-1 floats in uchar matrix
			if (w[j][i] > maxW) // remember largest weight
				maxW = w[j][i];
		}
	}

	str += "\n";
	NodeIt qi = someNodes.constBegin();
	for (i=0; i < n; i++) {
		float s = 0;
		Node* q = qi.value();
		str += QString("%1").arg(q->getId(), 5) + " ";
		for (j=0; j < n; j++) {
			str += QString("%1").arg(w[i][j], 5) + " ";
			s += w[i][j];
		}
		str += "\n";
		++qi;
	}
//	qDebug() << str;

	float t = qMin(1.0f * K, maxW); // set correlation threashold for clustering
	
	// start clustering
//	while (t > 0.8f * K && someNodes.size() > 2)
	//	{
	// prepare threshold
	t *= 0.9f;
	//		qDebug() << "t: " << t;
	i = 0;

	// set of clusters
	QSet<qlonglong> clustered;
	for (NodeIt ui = someNodes.constBegin(); ui != someNodes.constEnd(); ++ui, i++) {
		Node* u = ui.value();
		//											qDebug() << "u: " << u->getId();
		j = i+1;
		Cluster* c = u->getParent();
		// set of nodes about to cluster
		QSet<Node*> toCluster;
		for (NodeIt vi = ui+1; vi != someNodes.constEnd(); ++vi, j++) {
			if (pd->wasCanceled()) return;
			Node* v = vi.value();
			if (w[i][j] >= t) {
				//											qDebug() << "v: " << v->getId() << " w=" << w[i][j];
				if (c == NULL) {
					c = v->getParent();
					//								qDebug() << "c = v->getParent()";
					//								qDebug() << "c: " << (c == NULL ? "NULL" : QString("%1").arg(c->getId()));
				} else {
					if (v->getParent() != NULL) {
						//								qDebug() << "v has other parent!";
						continue;
					}
				}
				toCluster.insert(v);
				clustered.insert(v->getId());
				pd->setValue(step++);

				int link = -1;
				for (int k = 0; k < n; k++) {
					if(matrix[i][k] && matrix[j][k]) {
						if (link < 0 && link != i && link != j) {
							link = k;
						} else if (link >= 0) {
							link = -1;
							break;
						}
					}
				}
				if (link >= 0) {
					//							qDebug() << "link = " << link;
					Node* x = someNodes.value(someNodes.keys().at(link));
					if (!clustered.contains(x->getId())) {
						//								qDebug() << "x: " << x->getId();
						if (c = NULL) {
							c = x->getParent();
						} else if (x->getParent() != NULL) {
							continue;
						}
						toCluster.insert(x);
						clustered.insert(x->getId());
						pd->setValue(step++);
					}
				}

				//											qDebug() << "is clusterable";
			}
		}
		if (!toCluster.isEmpty()) {
			if (c == NULL) {
				c = addCluster();
				//											qDebug() << "new c: " << c->getId();
				c->setIgnored(!clustersVisible);
				c->setExpanded(!clustersVisible);
			}
			NIt i = toCluster.constBegin();
			while (i != toCluster.constEnd()) {
				Node* v = *i;
				//											qDebug() << "v': " << v->getId();
				if (v->getParent() == NULL) {
					v->setParent(c);
					v->setIgnored(clustersVisible);
					//											qDebug() << "v' added to c";
				}
				i++;
			}
			if (u->getParent() == NULL) {
				u->setParent(c);
				u->setIgnored(clustersVisible);
				//											qDebug() << "u added to c";
				clustered.insert(u->getId());
				pd->setValue(step++);
			}
		}
	}
	for (QSet<qlonglong>::const_iterator i = clustered.constBegin();
			i != clustered.constEnd(); ++i) {
		someNodes.remove(*i);
	}
//	}
//	qDebug() << "nodes: " << someNodes.size() << " clusters: " << clusters.size();

	for (NodeIt ci = clusters.begin(); ci != clusters.end(); ++ci) {
		Node* c = ci.value();
		graph->nodes.insert(c->getId(), c);
		graph->nodesByType.insert(c->getType()->getId(), c);
	}

	someNodes.unite(clusters);
	clusters.clear();
	if (someNodes.size() > 2 && maxLevels != 0) {
		clusterAdjacency(someNodes, clustersVisible, maxLevels - 1);
	}

}

Cluster* Clusterer::addCluster() {
	if (clusterType == NULL) {
		clusterType = graph->addType("cluster");
		clusterType->addKey("id", "");
	}
	Cluster* node = new Cluster(graph->incEleIdCounter(), clusterType, graph);

	clusters.insert(node->getId(), node);
	return node;
}
