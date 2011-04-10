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
	alg = NEIGHBORS;
	//	alg = ADJACENCY;
	step = level = 0;
}

bool Clusterer::setClusteringAlg(int i) {
	ClusteringAlg newAlg = NONE;
	switch (i) {
	case 0: newAlg = NONE; break;
	case 1: newAlg = NEIGHBORS; break;
	case 2: newAlg = LEAFS; break;
	case 3: newAlg = ADJACENCY; break;
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
		clusterAdjacency(QSet<Node*>::fromList(graph->getNodes()->values()), true, 3);
		break;
	}
}

void Clusterer::clusterNghbrs(QMap<qlonglong, Node* > someNodes, bool clustersVisible, int maxLevels) {
	pd->reset();
	pd->setLabelText(QString("Clustering graph ... (%1)").arg(++level));
	pd->setMaximum(someNodes.size()); //XXX
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
	pd->setMaximum(someNodes.size()); //XXX
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

void Clusterer::clusterAdjacency(QSet<Node*> someNodes, bool clustersVisible, int maxLevels) {
	pd->reset();
	pd->setLabelText(QString("Clustering graph ... (%1)").arg(++level));
	pd->setMaximum(someNodes.size() * 3); //XXX
	step = 0;

	int n = someNodes.size();
	std::vector<bool> p(7);
	std::vector<std::vector<bool> > matrix(n, std::vector<bool>(n, false));
	std::vector<std::vector<unsigned char> > w(n, std::vector<unsigned char>(n, 0));
	// we don't use float, floats are multiplied by K and stored as unsigned char;
	unsigned char K = 100;
	int i = 0, j = 0;
	for (NIt ui = someNodes.constBegin(); ui != someNodes.constEnd(); ++ui, i++) {
		pd->setValue(step++);
		Node* u = *ui;
		matrix[i][i] = true;
		QSet<Node*> nghbrs = u->getIncidentNodes();
		j = i+1;
		for (NIt vi = ui+1; vi != someNodes.constEnd(); ++vi, j++) {
			Node* v = *vi;
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
//	QString str = "\n";
	for (NIt ui = someNodes.constBegin(); ui != someNodes.constEnd(); ++ui, i++) {
		pd->setValue(step++);
		Node* u = *ui;
//		str += QString("%1").arg(u->getId(), 5) + " ";
		w[i][i] = 0;
		int degU = u->getIncidentNodes().size();
		j = i+1;
		for (NIt vi = ui+1; vi != someNodes.constEnd(); ++vi, j++) {
			if (pd->wasCanceled()) return;
			Node* v = *vi;
			int degV = v->getIncidentNodes().size();

			float sum = 0;
			for (int k = 0; k < n; k++) {
				sum += matrix[i][k] && matrix[j][k] ? 1 : 0;
			}
			float wij = ((float)((n * sum) - (degU * degV))) /
					sqrt(degU * degV * (n - degU) * (n - degV));
			w[j][i] = w[i][j] = qMax(0.0f, wij * K);
			if (wij > maxW)
				maxW = wij;
		}
	}
//	qDebug() << "maxW: " << maxW;

//	str += "\n";
//	for (i=0; i < n; i++) {
//		float s = 0;
//		for (j=0; j < n; j++) {
//			str += QString("%1").arg(w[i][j], 5) + " ";
//			s += w[i][j];
//		}
//		str += "\n";
//	}
//	qDebug() << str;

	float t = qMin(1.0f, maxW) * K;

	while (t > 0.6f * K && someNodes.size() > 2) {
		t *= 0.8f;
		//	qDebug() << "t: " << t;
		i = 0;

		QSet<Node*> clustered;
		for (NIt ui = someNodes.constBegin(); ui != someNodes.constEnd(); ++ui, i++) {
			Node* u = *ui;

			if (u->getParent() == NULL) {
				//							qDebug() << "u: " << u->getId();
				j = i+1;
				Cluster* c = u->getParent();
				QSet<Node*> toCluster;
				for (NIt vi = ui+1; vi != someNodes.constEnd(); ++vi, j++) {
					if (pd->wasCanceled()) return;
					Node* v = *vi;
					if (w[i][j] >= t) {
						//					qDebug() << "v: " << v->getId() << " w=" << w[i][j];
						if (c == NULL) {
							c = v->getParent();
						} else {
							if (v->getParent() != NULL)
								continue;
						}
						toCluster.insert(v);
						clustered.insert(v);
						pd->setValue(step++);
						//					qDebug() << "is clusterable";
					}
				}
				if (!toCluster.isEmpty()) {
					if (c == NULL) {
						c = addCluster();
						//					qDebug() << "c: " << c->getId();
						c->setIgnored(!clustersVisible);
						c->setExpanded(!clustersVisible);
					}
					NIt i = toCluster.constBegin();
					while (i != toCluster.constEnd()) {
						Node* v = *i;
						//					qDebug() << "v': " << v->getId();
						if (v->getParent() == NULL) {
							v->setParent(c);
							v->setIgnored(clustersVisible);
							//				qDebug() << "v' added to c";
						}
						i++;
					}
					if (u->getParent() == NULL) {
						u->setParent(c);
						u->setIgnored(clustersVisible);
						//					qDebug() << "u added to c";
						clustered.insert(u);
						pd->setValue(step++);
					}
				}
			}
		}
		someNodes.subtract(clustered);
	}

	for (NodeIt ci = clusters.begin(); ci != clusters.end(); ++ci) {
		Node* c = ci.value();
		graph->nodes.insert(c->getId(), c);
		graph->nodesByType.insert(c->getType()->getId(), c);
	}

	if (clusters.size() > 1 && maxLevels != 0) {
		someNodes.unite(QSet<Node*>::fromList(clusters.values()));
		clusters.clear();
		clusterAdjacency(someNodes, clustersVisible, maxLevels - 1);
	}
}

Cluster* Clusterer::addCluster() { // TODO cluster type
	if (clusterType == NULL) {
		clusterType = graph->addType("cluster");
		clusterType->addKey("id", "");
	}
	Cluster* node = new Cluster(graph->incEleIdCounter(), clusterType, graph);

	clusters.insert(node->getId(), node);
	return node;
}
