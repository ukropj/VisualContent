#include "Viewer/OsgCluster.h"
#include "Viewer/OsgNodeGroup.h"
#include "Model/Cluster.h"

#include <QDebug>

using namespace Vwr;

OsgCluster::OsgCluster(Model::Cluster* nodeCluster, DataMapping* dataMapping)
		: OsgNode(nodeCluster, dataMapping){ // TODO data mapping
	this->nodeCluster = nodeCluster;

	childrenMovingIn = 0;
	autocluster = true;
	setScale(sqrt(nodeCluster->getWeight()));
}

OsgCluster::~OsgCluster() {
//	qDebug() << "OsgCluster deleted";
}

bool OsgCluster::updateClusterState(float maxClusterSize) {
	if (!canAutocluster())
		return true;
	if (OsgNode::updateClusterState(maxClusterSize))
		return true;
	if (nodeCluster->getWeight() > maxClusterSize) {
		uncluster(false);
		return true;
	}
	return false;
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
			child->setPickable(false);
			// TODO temporarily change color of moving-to-cluster child
			childrenMovingIn++;
		}
		return this;
	} else {
		return NULL;
	}
}

AbstractNode* OsgCluster::uncluster(bool returnResult) {
	if (isClustering())
		return this;

	if (nodeCluster->unclusterChildren()) {
		setVisible(false);
		OsgNodeGroup* unclusterGroup = NULL;
		if (returnResult)
			unclusterGroup = new OsgNodeGroup();
		QSetIterator<Model::Node*> nodeIt = nodeCluster->getChildrenIterator();
		while (nodeIt.hasNext()) {
			OsgNode* child = nodeIt.next()->getOsgNode();
			child->setVisible(true);
			child->updatePosition();
			if (returnResult)
				unclusterGroup->addNode(child, false, false);
		}
		if (returnResult)
			unclusterGroup->updateSizeAndPos();
		return unclusterGroup;
	} else {
		return this;
	}
}

void OsgCluster::moveChildIn() {
	childrenMovingIn--;
	setScale(sqrt(nodeCluster->getWeight() - childrenMovingIn));

	if (childrenMovingIn == 0)
		nodeCluster->setExpanded(false);
	if (!isVisible()) {
		updatePosition();
		setVisible(true);
		emit changedVisibility(this, true);
	}
}

QString OsgCluster::debugInfo() const {
	QString str;
	QTextStream(&str) << toString()
			<< " childrenMovingIn: " << childrenMovingIn
			<< " movingToCluster: " << isMovingToCluster()
			<< " isClustering: " << isClustering()
			<< " canAutocluster: " << canAutocluster();
	return str ;
}
