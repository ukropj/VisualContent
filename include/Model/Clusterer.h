/*
 * Clusterer.h
 *
 *  Created on: 5.4.2011
 *      Author: jakub
 */

#ifndef CLUSTERER_H_
#define CLUSTERER_H_

#include <QMap>

namespace Model {
class Graph;
class Node;
class Cluster;
class Type;

class Clusterer {
public:
	enum ClusteringAlg {
		NONE, NEIGHBORS, LEAFS, MINCUT
	};

	Clusterer();
	bool setClusteringAlg(int);

	void cluster(Graph* graph);
private:
	Graph* graph;
	Type* clusterType;
	ClusteringAlg alg;

	QMap<qlonglong, Node* > clusters;
	void clusterNghbrs(QMap<qlonglong, Node* > someNodes, bool clustersVisible, int maxLevels);
	void clusterLeafs(QMap<qlonglong, Node* > someNodes, bool clustersVisible, int maxLevels);
	Cluster* addCluster();
};

}

#endif /* CLUSTERER_H_ */
