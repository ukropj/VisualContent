#include "Viewer/SceneElements.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Util/Config.h"

#include <QDebug>
#include <iostream>

using namespace Vwr;
using namespace Model;

SceneElements::SceneElements(QMap<qlonglong, Node*> *nodes,
		SceneGraph* sceneGraph) {
	this->sceneGraph = sceneGraph;

	osg::ref_ptr<osg::Group> nodeGroup = new osg::Group();

	if (Util::Config::getValue("Viewer.Display.NodesAlwaysOnTop").toInt())
		nodeGroup->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	float graphScale = Util::Config::getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	QMapIterator<qlonglong, Node* > i(*nodes);

	//	if (i.hasNext()) {
	//		i.next();
	//		nodeGroup->addChild(wrapNode(i.value()));
	//		nodeGroup->addChild(getNodeGroup(i.value()));
	//	}

	while (i.hasNext()) {
		i.next();

		osg::ref_ptr<osg::Group> g = getNodeGroup(i.value(), NULL);

		if (g != NULL) {
			nodeGroup->addChild(g);
		}
	}


	this->group = nodeGroup;
	group->setName("scene_elements");
	group->setStateSet(createStateSet());
}

SceneElements::~SceneElements(void) {
	nodes.clear();
	edges.clear();
}

osg::ref_ptr<osg::Group> SceneElements::getNodeGroup(Node* node,
		Edge* parentEdge) {
	osg::ref_ptr<osg::Group> group = NULL;

	if (!nodes.contains(node->getId())) {
		group = new osg::Group;
		group->addChild(wrapNode(node));

		QMap<qlonglong, Edge*>::iterator edgeI = node->getEdges()->begin();

		while (edgeI != node->getEdges()->end()) {
			if (*edgeI != parentEdge) {
				if (!edges.contains((*edgeI)->getId())) {
					group->addChild(wrapEdge(*edgeI));
				} else {
					//	qDebug() << "edge already in";
				}

				Node* otherNode = NULL;
				if (node->getId() == (*edgeI)->getSrcNode()->getId())
					otherNode = (*edgeI)->getDstNode();
				else
					otherNode = (*edgeI)->getSrcNode();

				osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup(otherNode,
						*edgeI);

				if (nodeGroup != NULL)
					group->addChild(nodeGroup);
			}

			edgeI++;
		}
	}
	return group;
}

osg::ref_ptr<osg::Group> SceneElements::getNodeGroup(Node* node) { // alternative SG construction
	osg::ref_ptr<osg::Group> group = NULL;

	QLinkedList<Node*> nodesAdded;

	QMap<qlonglong, Edge*>::iterator edgeI = node->getEdges()->begin();

	while (edgeI != node->getEdges()->end()) {
		if (edges.contains((*edgeI)->getId())) {
			edgeI++;
			continue;
		}
		if (group == NULL)
			group = new osg::Group;
		group->addChild(wrapEdge(*edgeI));

		Node* otherNode = NULL;
		if (node->getId() == (*edgeI)->getSrcNode()->getId())
			otherNode = (*edgeI)->getDstNode();
		else
			otherNode = (*edgeI)->getSrcNode();

		if (!nodes.contains(otherNode->getId())) {
			group->addChild(wrapNode(otherNode));
			nodesAdded.append(otherNode);
		}
		edgeI++;
	}

	QLinkedList<Node*>::iterator nodeI =
			nodesAdded.begin();

	while (nodeI != nodesAdded.end()) {
		osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup(*nodeI);
		if (nodeGroup != NULL)
			group->addChild(nodeGroup);
		nodeI++;
	}

	return group;
}

osg::ref_ptr<osg::AutoTransform> SceneElements::wrapNode(Node* node) {
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform();
	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	osg::ref_ptr<OsgNode> osgNode = new OsgNode(node, sceneGraph, at);
	nodes.insert(node->getId(), osgNode);

	at->setName("node_transform");
	at->addChild(osgNode);

	return at;
}

osg::ref_ptr<osg::Group> SceneElements::wrapEdge(Edge* edge) {
	osg::ref_ptr<OsgEdge> osgEdge = new OsgEdge(edge, sceneGraph);
	edges.insert(edge->getId(), osgEdge);
	osg::ref_ptr<osg::Group> edgeGroup = new osg::Group;

	edgeGroup->setName("edge_group");
	edgeGroup->addChild(osgEdge);

	return edgeGroup;
}

void SceneElements::updateNodeCoords(float interpolationSpeed) {
	QMap<qlonglong, osg::ref_ptr<OsgNode> >::const_iterator i =
			nodes.constBegin();

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

osg::ref_ptr<osg::StateSet> SceneElements::createStateSet() const {
	osg::ref_ptr<osg::StateSet> edgeStateSet = new osg::StateSet();

	edgeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	edgeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	edgeStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	edgeStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	edgeStateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

	return edgeStateSet;
}
