#include "Viewer/NodeGroup.h"
#include "Util/Config.h"
#include "Viewer/EdgeGroup.h"
#include <QDebug>
#include <iostream>

using namespace Vwr;
using namespace Model;

NodeGroup::NodeGroup(QMap<qlonglong, osg::ref_ptr<Node> > *nodes) {
	this->nodes = nodes;
	this->nodeTransforms
			= new QMap<qlonglong, osg::ref_ptr<osg::AutoTransform> > ;
	this->addedEdgeIds = new QSet<qlonglong> ;

	initNodes();
}

NodeGroup::~NodeGroup(void) {
}

void NodeGroup::initNodes() {
	osg::ref_ptr<osg::Group> nodeGroup = new osg::Group();

	if (Util::Config::getValue("Viewer.Display.NodesAlwaysOnTop").toInt())
		nodeGroup->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	float graphScale = Util::Config::getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	QMapIterator<qlonglong, osg::ref_ptr<Node> > i(*nodes);

	//	if (i.hasNext()) {
	//		i.next();
	//		nodeGroup->addChild(wrapNode(i.value(), graphScale));
	//		nodeGroup->addChild(getNodeGroup(i.value(), graphScale));
	//	}



	while (i.hasNext()) {
		i.next();

		osg::ref_ptr<osg::Group> g = getNodeGroup(i.value(), NULL, graphScale);

		if (g != NULL) {
			nodeGroup->addChild(g);
		}
	}

	nodeGroup->setName("nodes_group");
	this->group = nodeGroup;
}

osg::ref_ptr<osg::Group> NodeGroup::getNodeGroup(osg::ref_ptr<Node> node,
		osg::ref_ptr<Edge> parentEdge, float graphScale) {
	osg::ref_ptr<osg::Group> group = NULL;

	if (!nodeTransforms->contains(node->getId())) {
		group = new osg::Group;
		group->addChild(wrapNode(node, graphScale));

		QMap<qlonglong, osg::ref_ptr<Edge> >::iterator edgeI =
				node->getEdges()->begin();

		while (edgeI != node->getEdges()->end()) {
			if (*edgeI != parentEdge) {
//				if (!addedEdgeIds->contains((*edgeI)->getId())) {
//					group->addChild(wrapEdge(*edgeI));
//				} else {
//					//	qDebug() << "edge already in";
//				}

				osg::ref_ptr<Node> otherNode = NULL;
				if (node->getId() == (*edgeI)->getSrcNode()->getId())
					otherNode = (*edgeI)->getDstNode();
				else
					otherNode = (*edgeI)->getSrcNode();

				osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup(otherNode,
						*edgeI, graphScale);

				if (nodeGroup != NULL)
					group->addChild(nodeGroup);
			}

			edgeI++;
		}
	}
	return group;
}

osg::ref_ptr<osg::Group> NodeGroup::getNodeGroup(osg::ref_ptr<Node> node,
		float graphScale) { // alternative SG construction
	osg::ref_ptr<osg::Group> group = NULL;

	QLinkedList<osg::ref_ptr<Node> > nodesAdded;

	QMap<qlonglong, osg::ref_ptr<Edge> >::iterator edgeI =
			node->getEdges()->begin();

	while (edgeI != node->getEdges()->end()) {
		if (addedEdgeIds->contains((*edgeI)->getId())) {
			edgeI++;
			continue;
		}
		if (group == NULL)
			group = new osg::Group;
		group->addChild(wrapEdge(*edgeI));

		osg::ref_ptr<Node> otherNode = NULL;
		if (node->getId() == (*edgeI)->getSrcNode()->getId())
			otherNode = (*edgeI)->getDstNode();
		else
			otherNode = (*edgeI)->getSrcNode();

		if (!nodeTransforms->contains(otherNode->getId())) {
			group->addChild(wrapNode(otherNode, graphScale));
			nodesAdded.append(otherNode);
		}
		edgeI++;
	}

	QLinkedList<osg::ref_ptr<Model::Node> >::iterator nodeI =
			nodesAdded.begin();

	while (nodeI != nodesAdded.end()) {
		osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup(*nodeI, graphScale);
		if (nodeGroup != NULL)
			group->addChild(nodeGroup);
		nodeI++;
	}

	return group;
}

osg::ref_ptr<osg::AutoTransform> NodeGroup::wrapNode(osg::ref_ptr<Node> node,
		float graphScale) {
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform();
	at->setPosition(node->getTargetPosition() * graphScale);
	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	nodeTransforms->insert(node->getId(), at);
	at->addChild(node);

	//	qDebug() << node->getName() << "wrapped";
	return at;
}

osg::ref_ptr<osg::Group> NodeGroup::wrapEdge(osg::ref_ptr<Edge> edge) {
	addedEdgeIds->insert(edge->getId());
	osg::ref_ptr<osg::Group> edgeGroup = new osg::Group;

	edgeGroup->setName("edge_group");
	edgeGroup->setStateSet(Edge::getStateSetInstance(edge->isOriented()));
	edgeGroup->addChild(edge);

	//	edgeGroup->addChild(edge->getLabel());

	//	qDebug() << edge->getName() << "wrapped";

	return edgeGroup;
}

void NodeGroup::synchronizeNodes() { // is never called, was used for metanodes...
	QList<qlonglong> nodeKeys = nodes->keys();

	QList<qlonglong> nodeTransformsKeys = nodeTransforms->keys();
	QSet<qlonglong> result = nodeTransformsKeys.toSet().subtract(
			nodeKeys.toSet());

	QSet<qlonglong>::const_iterator i = result.constBegin();

	while (i != result.constEnd()) {
		group->removeChild(nodeTransforms->value(*i));

		nodeTransforms->remove(*i);
		++i;
	}

	result = nodeKeys.toSet().subtract(nodeTransformsKeys.toSet());
	i = result.constBegin();

	float graphScale = Util::Config::getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	while (i != result.constEnd()) {
		group->addChild(wrapNode(nodes->value(*i), graphScale));
		++i;
	}
}

void NodeGroup::updateNodeCoordinates(float interpolationSpeed) {
	QMap<qlonglong, osg::ref_ptr<Node> >::const_iterator i =
			nodes->constBegin();

	while (i != nodes->constEnd()) {
		nodeTransforms->value(i.key())->setPosition((*i)->getCurrentPosition(
				true, interpolationSpeed));
		++i;
	}
}

void NodeGroup::freezeNodePositions() {
	float graphScale = Util::Config::getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	QMap<qlonglong, osg::ref_ptr<Node> >::const_iterator i =
			nodes->constBegin();

	while (i != nodes->constEnd()) {
		(*i)->setTargetPosition((*i)->getCurrentPosition() / graphScale);
		++i;
	}
}
