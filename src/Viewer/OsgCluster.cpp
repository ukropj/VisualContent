#include "Viewer/OsgCluster.h"
#include "Viewer/OsgNodeGroup.h"
#include "Viewer/CompositeContent.h"
#include "Model/Cluster.h"

#include <QDebug>

using namespace Vwr;

OsgCluster::OsgCluster(Model::Cluster* nodeCluster)
		: OsgNode(nodeCluster, NULL){ // TODO data mapping
	this->nodeCluster = nodeCluster;
	DataMapping* dm = new DataMapping();
	dm->setContentType(DataMapping::COMPOSITE);
	setDataMapping(dm);

	childrenMovingIn = 0;
	autocluster = true;
	setScale(sqrt(nodeCluster->getWeight()));
}

OsgCluster::~OsgCluster() {
//	qDebug() << "OsgCluster deleted";
}

void OsgCluster::setDataMapping(DataMapping* dataMapping) {
	OsgNode::setDataMapping(dataMapping);
	mapping->setContentType(DataMapping::COMPOSITE);

	bool f = isExpanded();
	setExpanded(false);
	contentSwitch->removeChild(visualG);
	visualG = new CompositeContent(this); // this is diferent from parent!
	contentSwitch->addChild(visualG);
	setExpanded(f);
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
	bool expand = false;
	if (nodeCluster->clusterChildren()) {
		QSetIterator<Model::Node*> nodeIt = nodeCluster->getChildrenIterator();
		while (nodeIt.hasNext()) {
			OsgNode* child = nodeIt.next()->getOsgNode();
			child->setMovingToCluster(true);
			child->setPickable(false);
			expand = expand || child->isExpanded();
			// TODO temporarily change color of moving-to-cluster child
			childrenMovingIn++;
		}
		setExpanded(expand);
		return this;
	} else {
		return NULL;
	}
}

AbstractNode* OsgCluster::uncluster(bool returnResult) {
	if (isClustering())
		return this;
	bool expand = isExpanded();
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
			child->setExpanded(expand);
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
