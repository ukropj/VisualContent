/*
 * CompositeContent.cpp
 *
 *  Created on: 7.4.2011
 *      Author: jakub
 */

#include "Viewer/CompositeContent.h"
#include "Viewer/OsgCluster.h"
#include "Model/Cluster.h"

using namespace Vwr;

CompositeContent::CompositeContent(OsgCluster* cluster) : cluster(cluster){
	loaded = false;
}

CompositeContent::~CompositeContent() {

}

bool CompositeContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;

		QSize size(200, 200); // XXX magic

		QWidget* mosaic = new QWidget();
		mosaic->setGeometry(QRect(QPoint(1, 1), size));

		QSetIterator<Model::Node*> nodeIt = cluster->nodeCluster->getChildrenIterator();
		while (nodeIt.hasNext()) {
			OsgNode* child = nodeIt.next()->getOsgNode();

		}

		setWidget(mosaic, 0.2f); 	// XXX magic number
		contentGeode->setName("node_content_cluster");
		return true;
	}
}
