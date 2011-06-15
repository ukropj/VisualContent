#ifndef OSGCLUSTER_H_
#define OSGCLUSTER_H_

#include "Viewer/OsgNode.h"

namespace Model {
class Cluster;
}

namespace Vwr {
class CompositeContent;

/// Graphic representation of Cluster.
class OsgCluster : public OsgNode {
public:
	OsgCluster(Model::Cluster* nodeCluster, DataMapping* dataMapping = NULL);
	~OsgCluster();

	void setDataMapping(DataMapping* dataMapping = NULL);
	bool updateClusterState(float maxClusterSize = -1);
	bool isClustering() const;
	AbstractNode* cluster();
	AbstractNode* uncluster(bool returnResult = true);
	void moveChildIn(OsgNode* child);
	void allowAutocluster(bool flag) {autocluster = flag;}
	bool canAutocluster() const {return autocluster;}

	QString debugInfo() const;

protected:

private:
	Model::Cluster* nodeCluster;
	int childrenMovingIn;
	bool autocluster;

	friend class CompositeContent;
};

}

#endif  /*OSGCLUSTER_H_*/
