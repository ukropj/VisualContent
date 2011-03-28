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

	AbstractNode* cluster();
	AbstractNode* uncluster();
	bool isClustering() const;
	void moveChildIn();

protected:

private:
	Model::Cluster* nodeCluster;
	int childrenMovingIn;

};

}

#endif  /*OSGCLUSTER_H_*/
