#include "Viewer/NodeGroup.h"
#include "Util/Config.h"
#include "Viewer/EdgeGroup.h"
#include <QDebug>

using namespace Vwr;
using namespace Model;

NodeGroup::NodeGroup(QMap<qlonglong, osg::ref_ptr<Node> > *nodes) {
	this->nodes = nodes;
	this->nodeTransforms
			= new QMap<qlonglong, osg::ref_ptr<osg::AutoTransform> > ;
	this->addedEdgeIds.clear();

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

	while (i.hasNext()) {
		i.next();

		osg::ref_ptr<osg::Group> g = getNodeGroup(i.value(), NULL, graphScale);

		if (g != NULL)
			nodeGroup->addChild(g);
	}

	nodeGroup->setName("nodes_group");
	this->group = nodeGroup;
}

osg::ref_ptr<osg::Group> NodeGroup::getNodeGroup(osg::ref_ptr<Node> node,
		osg::ref_ptr<Edge> parentEdge, float graphScale) {
	osg::ref_ptr<osg::Group> group = NULL;

	if (!nodeTransforms->contains(node->getId())) {
		group = new osg::Group;

		group->addChild(wrapChild(node, graphScale));

		QMap<qlonglong, osg::ref_ptr<Edge> >::iterator edgeI =
				node->getEdges()->begin();

		while (edgeI != node->getEdges()->end()) {
			if (*edgeI != parentEdge) {
				osg::ref_ptr<Node> otherNode = NULL;

//				if (!addedEdgeIds->contains((*edgeI)->getId())) {
//					addedEdgeIds->insert((*edgeI)->getId());
//					osg::ref_ptr<osg::Group> edgeGroup = new osg::Group;
//					edgeGroup->setName("edges_group");
//					edgeGroup->setStateSet(Edge::createStateSet((*edgeI)->isOriented()));
//					edgeGroup->addChild(edgeI.value());
//					group->addChild(edgeGroup);
//				} else {
// //				qDebug() << "edge already in";
//				}

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

osg::ref_ptr<osg::AutoTransform> NodeGroup::wrapChild(osg::ref_ptr<Node> node,
		float graphScale) {
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform();
	at->setPosition(node->getTargetPosition() * graphScale);
	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	nodeTransforms->insert(node->getId(), at);
	at->addChild(node);

	return at;
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
		group->addChild(wrapChild(nodes->value(*i), graphScale));
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
