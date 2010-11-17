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

SceneElements::SceneElements(QMap<qlonglong, Node*>* nodes, QMap<qlonglong,
		Edge*>* edges, SceneGraph* sceneGraph) {
	this->sceneGraph = sceneGraph;

	group = new osg::Group();
	group->setName("scene_elements");

	if (!nodes->isEmpty())
		group->addChild(initNodes(nodes));
	if (!edges->isEmpty())
		group->addChild(initEdges(edges));

	group->setStateSet(createStateSet());
}

SceneElements::~SceneElements(void) {
	nodes.clear();
	edges.clear();
}

osg::ref_ptr<osg::Group> SceneElements::initNodes(
		QMap<qlonglong, Node*>* inNodes) {
	osg::ref_ptr<osg::Group> nodeGroup = new osg::Group();

	if (Util::Config::getValue("Viewer.Display.NodesAlwaysOnTop").toInt())
		nodeGroup->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	QMapIterator<qlonglong, Node*> i(*inNodes);

	while (i.hasNext()) {
		i.next();
		osg::ref_ptr<osg::Group> g;

		//		if (!nodes.contains(i.value()->getId())) {
		//			nodeGroup->addChild(wrapNode(i.value()));
		//			g = getNodeGroup2(i.value()));
		//		}
		g = getNodeGroup1(i.value(), NULL);
		if (g != NULL) {
			nodeGroup->addChild(g);
		}
	}
	return nodeGroup;
}

osg::ref_ptr<osg::Group> SceneElements::initEdges(
		QMap<qlonglong, Edge*>* inEdges) {
	osg::ref_ptr<osg::Group> allEdges = new osg::Group;

	edgesGeometry = new osg::Geometry();
	edgesOGeometry = new osg::Geometry();
	edgesGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	edgesOGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	QMapIterator<qlonglong, Edge*> i(*inEdges);
	int j = 0;
	while (i.hasNext()) {
		i.next();
		OsgEdge* osgEdge = new OsgEdge(i.value(), sceneGraph);
		edges.insert(i.value()->getId(), osgEdge);
		if (!osgEdge->isOriented())
			edgesGeometry->addPrimitiveSet(new osg::DrawArrays(
					osg::PrimitiveSet::QUADS, j, 4));
		else
			edgesOGeometry->addPrimitiveSet(new osg::DrawArrays(
					osg::PrimitiveSet::QUADS, j, 4));
		j += 4;
	}

	updateEdgeCoords(); // updates edgeGeometry

	osg::ref_ptr<osg::Geode> edgeGeode = new osg::Geode();
	edgesGeometry->setStateSet(OsgEdge::getStateSetInstance(
			OsgEdge::NONORIENTED));
	edgeGeode->addDrawable(edgesGeometry);
	edgesOGeometry->setStateSet(OsgEdge::getStateSetInstance(OsgEdge::ORIENTED));
	edgeGeode->addDrawable(edgesOGeometry);

	//	osg::ref_ptr<osg::Geode> g2 = new osg::Geode;
	//	g2->addDrawable(orientedGeometry);
	//	g2->setStateSet(OsgEdge::getStatesetInstance(OsgEdge::ORIENTED));

	allEdges->addChild(edgeGeode);

	return allEdges;
}

osg::ref_ptr<osg::Group> SceneElements::getNodeGroup1(Node* node,
		Edge* parentEdge) {
	osg::ref_ptr<osg::Group> group = NULL;

	if (!nodes.contains(node->getId())) {
		group = new osg::Group;
		group->addChild(wrapNode(node));

		QMap<qlonglong, Edge*>::iterator edgeI = node->getEdges()->begin();

		while (edgeI != node->getEdges()->end()) {
			if (*edgeI != parentEdge) {
				//				if (!edges.contains((*edgeI)->getId())) {
				//					group->addChild(wrapEdge(*edgeI));
				//				} else {
				//					//	qDebug() << "edge already in";
				//				}

				Node* otherNode = NULL;
				if (node->getId() == (*edgeI)->getSrcNode()->getId())
					otherNode = (*edgeI)->getDstNode();
				else
					otherNode = (*edgeI)->getSrcNode();

				osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup1(otherNode,
						*edgeI);

				if (nodeGroup != NULL)
					group->addChild(nodeGroup);
			}

			edgeI++;
		}
	}
	return group;
}

osg::ref_ptr<osg::Group> SceneElements::getNodeGroup2(Node* node) { // alternative SG construction
	osg::ref_ptr<osg::Group> group = NULL;

	QLinkedList<Node*> nodesAdded;

	QMap<qlonglong, Edge*>::iterator edgeI = node->getEdges()->begin();

	while (edgeI != node->getEdges()->end()) {
		Node* otherNode = (*edgeI)->getOtherNode(node);
		if (nodes.contains(otherNode->getId()))
			continue;
		if (group == NULL)
			group = new osg::Group;
		//		group->addChild(wrapEdge(*edgeI));
		group->addChild(wrapNode(otherNode));
		nodesAdded.append(otherNode);
		edgeI++;
	}

	QLinkedList<Node*>::iterator nodeI = nodesAdded.begin();

	while (nodeI != nodesAdded.end()) {
		osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup2(*nodeI);
		if (nodeGroup != NULL)
			group->addChild(nodeGroup);
		nodeI++;
	}

	return group;
}

osg::ref_ptr<osg::AutoTransform> SceneElements::wrapNode(Node* node) {
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform();
	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_CAMERA);

	osg::ref_ptr<OsgNode> osgNode = new OsgNode(node, sceneGraph, at);
	nodes.insert(node->getId(), osgNode);

	at->setName("node_transform");
	at->addChild(osgNode);

	return at;
}

//osg::ref_ptr<osg::Group> SceneElements::wrapEdge(Edge* edge) {
//	osg::ref_ptr<OsgEdge> osgEdge = new OsgEdge(edge, sceneGraph);
//	edges.insert(edge->getId(), osgEdge);
//	osg::ref_ptr<osg::Group> edgeGroup = new osg::Group;
//
//	edgeGroup->setName("edge_group");
//	edgeGroup->addChild(osgEdge);
//
//	return edgeGroup;
//}

void SceneElements::updateNodeCoords(float interpolationSpeed) {
	QMap<qlonglong, osg::ref_ptr<OsgNode> >::const_iterator i =
			nodes.constBegin();

	while (i != nodes.constEnd()) {
		i.value()->updatePosition(interpolationSpeed);
		++i;
	}
}

void SceneElements::updateEdgeCoords() {
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec4Array> orientedColors = new osg::Vec4Array;

	QMap<qlonglong, OsgEdge*>::const_iterator i = edges.constBegin();

	while (i != edges.constEnd()) {
		if (i.value()->isOriented())
			i.value()->getEdgeData(coords, texCoords, orientedColors);
		else
			i.value()->getEdgeData(coords, texCoords, colors);
		//		i.value()->updateGeometry();
		i++;
	}

	edgesGeometry->setVertexArray(coords);
	edgesGeometry->setTexCoordArray(0, texCoords);
	edgesGeometry->setColorArray(colors);

	edgesOGeometry->setVertexArray(coords);
	edgesOGeometry->setTexCoordArray(0, texCoords);
	edgesOGeometry->setColorArray(orientedColors);
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
