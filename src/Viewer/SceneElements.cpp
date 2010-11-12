#include "Viewer/SceneElements.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Util/Config.h"

#include <QDebug>
#include <iostream>

using namespace Vwr;
using namespace Model;

SceneElements::SceneElements(QMap<qlonglong, osg::ref_ptr<Node> > *nodes,
		SceneGraph* sceneGraph) {
	this->sceneGraph = sceneGraph;

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

	qDebug() << "scene on";

	nodeGroup->setName("nodes_group");
	this->group = nodeGroup;
}

SceneElements::~SceneElements(void) {
}

osg::ref_ptr<osg::Group> SceneElements::getNodeGroup(osg::ref_ptr<Node> node,
		Edge* parentEdge, float graphScale) {
	osg::ref_ptr<osg::Group> group = NULL;

	if (!nodes.contains(node->getId())) {
		group = new osg::Group;
		group->addChild(wrapNode(node, graphScale));

		QMap<qlonglong, Edge*>::iterator edgeI = node->getEdges()->begin();

		while (edgeI != node->getEdges()->end()) {
			if (*edgeI != parentEdge) {
				if (!edges.contains((*edgeI)->getId())) {
					group->addChild(wrapEdge(*edgeI));
				} else {
					//	qDebug() << "edge already in";
				}

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

osg::ref_ptr<osg::Group> SceneElements::getNodeGroup(osg::ref_ptr<Node> node,
		float graphScale) { // alternative SG construction
	osg::ref_ptr<osg::Group> group = NULL;

	QLinkedList<osg::ref_ptr<Node> > nodesAdded;

	QMap<qlonglong, Edge*>::iterator edgeI = node->getEdges()->begin();

	while (edgeI != node->getEdges()->end()) {
		if (edges.contains((*edgeI)->getId())) {
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

		if (!nodes.contains(otherNode->getId())) {
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

osg::ref_ptr<osg::AutoTransform> SceneElements::wrapNode(
		osg::ref_ptr<Node> node, float graphScale) {
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform();
	at->setPosition(node->getTargetPosition() * graphScale);
	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	nodes.insert(node->getId(), node);
	at->addChild(node);
	node->setNodeTansform(at);

	return at;
}

osg::ref_ptr<osg::Group> SceneElements::wrapEdge(Edge* edge) {
	osg::ref_ptr<OsgEdge> osgEdge = new OsgEdge(edge, sceneGraph);
	edges.insert(edge->getId(), osgEdge);
	osg::ref_ptr<osg::Group> edgeGroup = new osg::Group;

	edgeGroup->setName("edge_group");
	edgeGroup->setStateSet(OsgEdge::getStateSetInstance(edge->isOriented()));
	edgeGroup->addChild(osgEdge);

	return edgeGroup;
}

void SceneElements::updateNodeCoords(float interpolationSpeed) {
	QMap<qlonglong, osg::ref_ptr<Node> >::const_iterator i = nodes.constBegin();

	while (i != nodes.constEnd()) {
		i.value()->updatePosition(interpolationSpeed);
		++i;
	}
}

void SceneElements::updateEdgeCoords() {
	QMap<qlonglong, osg::ref_ptr<OsgEdge> >::const_iterator i =
			edges.constBegin();

	while (i != edges.constEnd()) {
		i.value()->updateGeometry();
		i++;
	}
}

void SceneElements::freezeNodePositions() {
	QMap<qlonglong, osg::ref_ptr<Node> >::const_iterator i = nodes.constBegin();

	while (i != nodes.constEnd()) {
		i.value()->updatePosition(1);
		++i;
	}
}
