#include "Viewer/OsgCluster.h"
#include "Viewer/OsgNodeGroup.h"
#include "Model/Cluster.h"

#include <QDebug>

using namespace Vwr;

OsgCluster::OsgCluster(Model::Cluster* nodeCluster, DataMapping* dataMapping)
		: OsgNode(nodeCluster, dataMapping){ // TODO data mapping
	this->nodeCluster = nodeCluster;

	childrenMovingIn = 0;
	setColor(osg::Vec4f(0,0,1,1)); // XXX
	setScale(sqrt(nodeCluster->getWeight()));
}

OsgCluster::~OsgCluster() {
//	qDebug() << "OsgCluster deleted";
}

void OsgCluster::moveChildIn() {
	childrenMovingIn--;
	setScale(sqrt(nodeCluster->getWeight() - childrenMovingIn));
	if (!isVisible()) {
		updatePosition();
		setVisible(true);
		emit changedVisibility(this, true);
	}
}

bool OsgCluster::isClustering() const {
	if (isMovingToCluster())
		return true;
	if (childrenMovingIn > 0)
		return true;
	return false;
}

AbstractNode* OsgCluster::cluster() {
	if (isClustering())
		return NULL;

	if (nodeCluster->clusterChildren()) {
		QSetIterator<Model::Node*> nodeIt = nodeCluster->getChildrenIterator();
		while (nodeIt.hasNext()) {
			OsgNode* child = nodeIt.next()->getOsgNode();
			child->setMovingToCluster(true);
			// TODO temporarily change color of moving-to-cluster child
			childrenMovingIn++;
		}
		return this;
	} else {
		return NULL;
	}
}

AbstractNode* OsgCluster::uncluster() {
	if (isClustering())
		return this;

	if (nodeCluster->unclusterChildren()) {
//	qDebug() << "unclustering" << this->toString();
		this->setVisible(false);
		OsgNodeGroup* unclusterGroup = new OsgNodeGroup();
		QSetIterator<Model::Node*> nodeIt = nodeCluster->getChildrenIterator();
		while (nodeIt.hasNext()) {
			OsgNode* child = nodeIt.next()->getOsgNode();
			child->setVisible(true);
			child->updatePosition();
			unclusterGroup->addNode(child, false, false);
		}
		unclusterGroup->updateSizeAndPos();
		return unclusterGroup;
	} else {
		return this;
	}
}
