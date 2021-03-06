/*
 * Clusterer.h
 *
 *  Created on: 5.4.2011
 *      Author: jakub
 */

#ifndef CLUSTERER_H_
#define CLUSTERER_H_

#include <QMap>
#include <QSet>
#include <QProgressDialog>

namespace Model {
class Graph;
class Node;
class Cluster;
class Type;

/// Class containing clustering algorithms.
/**
 * \author Jakub Ukrop
 */
class Clusterer {
public:
	enum ClusteringAlg {
		NONE, NEIGHBORS, LEAFS, ADJACENCY
	};

	Clusterer();
	/// Sets clustering alrogithm.
	/*
	 * 0 = no clustering, 1 = adjacency clustering, 2 = leaf clustering, 3 = neighbor psedo-clustering
	 */
	bool setClusteringAlg(int);

	void cluster(Graph* graph, QProgressDialog* pd);

private:
	typedef QSet<Node*>::const_iterator NIt;
	Graph* graph;
	Type* clusterType;
	ClusteringAlg alg;

	QMap<qlonglong, Node* > clusters;
	void clusterNghbrs(QMap<qlonglong, Node* > someNodes, bool clustersVisible = true, int maxLevels = 1);
	void clusterLeafs(QMap<qlonglong, Node* > someNodes, bool clustersVisible = true, int maxLevels = 1);
	void clusterAdjacency(QMap<qlonglong, Node* > someNodes, bool clustersVisible = true, int maxLevels = 1);

	Cluster* addCluster();

	int step;
	int level;
	QProgressDialog* pd;

};

}

#endif /* CLUSTERER_H_ */
