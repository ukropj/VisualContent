#ifndef NODECLUSTER_H_
#define NODECLUSTER_H_

#include "Model/Node.h"

namespace Model {
class Graph;
class Type;

class Cluster : public Node {
public:
	Cluster(qlonglong id, Type* type, Graph* graph);

	void setExpanded(bool flag);
	bool isExpanded() const {return expanded;}
	void setParent(Cluster* newParent);
	QSet<Node*> getIncidentNodes(bool ignoreClusters = false) const;
	QSetIterator<Node*> getChildrenIterator() const {return QSetIterator<Node*>(children);}


	bool clusterChildren();
	bool unclusterChildren();

	bool canCluster();
	virtual bool isCluster() const {return true;}

private:

	QSet<Node*> children;
	int expandedChClusters;		// number of children that are expanded (ignored) clusters
	bool expanded;

	friend class Node; // XXX
};

}

#endif  /*NODECLUSTER_H_*/
