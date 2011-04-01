#ifndef OSGCLUSTER_H_
#define OSGCLUSTER_H_

#include "Viewer/OsgNode.h"

namespace Model {
class Cluster;
}

namespace Vwr {

class OsgCluster : public OsgNode {
public:
	OsgCluster(Model::Cluster* nodeCluster, DataMapping* dataMapping = NULL);
	~OsgCluster();

	bool updateClusterState(float maxClusterSize = -1);
	bool isClustering() const;
	AbstractNode* cluster();
	AbstractNode* uncluster(bool returnResult = true);
	void moveChildIn();
	void allowAutocluster(bool flag) {autocluster = flag;}
	bool canAutocluster() const {return autocluster;}

	QString debugInfo() const;

protected:

private:
	Model::Cluster* nodeCluster;
	int childrenMovingIn;
	bool autocluster;

};

}

#endif  /*OSGCLUSTER_H_*/
