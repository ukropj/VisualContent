#include "Viewer/SceneElements.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Util/Config.h"

#include <QDebug>
#include <iostream>
#include <osgManipulator/TabBoxDragger>

using namespace Vwr;
using namespace Model;

SceneElements::SceneElements(QMap<qlonglong, Node*>* nodes, QMap<qlonglong,
		Edge*>* edges, SceneGraph* sceneGraph, QProgressDialog* progressBar) {
	this->sceneGraph = sceneGraph;

	pd = progressBar;
	if (pd != NULL) {
		pd->setLabelText("Drawing graph...");
		step = pd->value();
	}

	elementsGroup = new osg::Group();
	elementsGroup->setName("scene_elements");

	if (!nodes->isEmpty())
		elementsGroup->addChild(initNodes(nodes));
	if (!edges->isEmpty())
		elementsGroup->addChild(initEdges(edges));

	elementsGroup->setStateSet(createStateSet());
}

SceneElements::~SceneElements(void) {
	nodes.clear();
	edges.clear();
}

osg::ref_ptr<osg::Group> SceneElements::initNodes(
		QMap<qlonglong, Node*>* inNodes) {
	osg::ref_ptr<osg::Group> nodeGroup = new osg::Group();
	nodeGroup->setName("all_nodes");
	nodeGroup->getOrCreateStateSet()->setRenderBinDetails(1, "DepthSortedBin");

	//	if (Util::Config::getValue("Viewer.Display.NodesAlwaysOnTop").toInt())
	//		nodeGroup->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	QMapIterator<qlonglong, Node*> i(*inNodes);

	while (i.hasNext()) {
		i.next();
		osg::ref_ptr<osg::Group> g;

		g = getNodeGroup2(i.value());
//		g = getNodeGroup1(i.value(), NULL);
		if (g != NULL) {
			nodeGroup->addChild(g);
		}
	}
	return nodeGroup;
}

osg::ref_ptr<osg::Group> SceneElements::initEdges(
		QMap<qlonglong, Edge*>* inEdges) {
	osg::ref_ptr<osg::Group> allEdges = new osg::Group;
	allEdges->setName("all_edges");
	osg::ref_ptr<osg::Geode> edgeGeode = new osg::Geode();
	allEdges->addChild(edgeGeode);
	//	allEdges->getOrCreateStateSet()->setRenderBinDetails(1, "DepthSortedBin");

	edgesGeometry = new osg::Geometry();
	edgesOGeometry = new osg::Geometry();

	osg::Vec4Array* colors = new osg::Vec4Array;
	osg::Vec4Array* colorsO = new osg::Vec4Array;

	QMapIterator<qlonglong, Edge*> i(*inEdges);
	int index = 0;
	int indexO = 0;
	while (i.hasNext()) {
		if (pd != NULL)
			pd->setValue(step++);
		i.next();
		OsgEdge* osgEdge = new OsgEdge(i.value(), sceneGraph);
		edges.insert(i.value()->getId(), osgEdge);
		if (!osgEdge->isOriented()) {
			edgesGeometry->addPrimitiveSet(new osg::DrawArrays(
					osg::PrimitiveSet::QUADS, index, 4));
			for (int i = 0; i < 4; i++)
				colors->push_back(osg::Vec4f(0,0,0,1));
			index += 4;
		} else {
			edgesOGeometry->addPrimitiveSet(new osg::DrawArrays(
					osg::PrimitiveSet::QUADS, indexO, 4));
			for (int i = 0; i < 4; i++)
				colorsO->push_back(osg::Vec4f(0,0,0,1));
			indexO += 4;
		}
	}

	edgesGeometry->setStateSet(OsgEdge::createStateSet(OsgEdge::UNORIENTED));
	edgesOGeometry->setStateSet(OsgEdge::createStateSet(OsgEdge::ORIENTED));

	edgesGeometry->setColorArray(colors);
	edgesOGeometry->setColorArray(colorsO);

	// important: set color binding only after setting color arrays!
	edgesGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	edgesOGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	edgeGeode->addDrawable(edgesGeometry);
	edgeGeode->addDrawable(edgesOGeometry);
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
				Node* otherNode = (*edgeI)->getOtherNode(node);
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

osg::ref_ptr<osg::Group> SceneElements::getNodeGroup2(Node* firstNode) { // alternative SG construction
	QList<Node*> nodeList;
	osg::ref_ptr<osg::Group> nodeGroup = new osg::Group;
	int index = 0;

	if (nodes.contains(firstNode->getId()))
		return NULL;

	nodeGroup->addChild(wrapNode(firstNode));
	nodeList << firstNode;

	while (nodeList.size() > index) {
		int size = nodeList.size();
		osg::ref_ptr<osg::Group> uberGroup = NULL;

		for (int i = index; i < size; i++) {
			osg::ref_ptr<osg::Group> group = NULL;
			Node* node = nodeList[i];

			QMap<qlonglong, Edge*>::iterator edgeI = node->getEdges()->begin();
			while (edgeI != node->getEdges()->end()) {
				Node* otherNode = (*edgeI)->getOtherNode(node);
				if (!nodes.contains(otherNode->getId())) {
					if (group == NULL)
						group = new osg::Group;
					group->addChild(wrapNode(otherNode));
					nodeList << otherNode;
				}
				edgeI++;
			}
			if (group != NULL) {
				if (uberGroup == NULL)
					uberGroup = new osg::Group;
				uberGroup->addChild(group);
			}
		}
		nodeGroup->addChild(uberGroup);
		index++;
	}
	return nodeGroup;
}

osg::ref_ptr<osg::AutoTransform> SceneElements::wrapNode(Node* node) {
	if (pd != NULL) {
		pd->setValue(step++);
	}
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform();
	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	osg::ref_ptr<OsgNode> osgNode = new OsgNode(node, sceneGraph, at);
	nodes.insert(node->getId(), osgNode);

	at->setName("node_transform");
	at->addChild(osgNode);

	return at;
}

void SceneElements::updateNodes(float interpolationSpeed) {
	QMap<qlonglong, osg::ref_ptr<OsgNode> >::const_iterator i =
			nodes.constBegin();

	while (i != nodes.constEnd()) {
		i.value()->updatePosition(interpolationSpeed);
		++i;
	}
}

void SceneElements::updateEdges() {
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	osg::ref_ptr<osg::Vec3Array> coordsO = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texCoordsO = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec4Array> colorsO = new osg::Vec4Array;

	QMap<qlonglong, OsgEdge*>::const_iterator i = edges.constBegin();

	while (i != edges.constEnd()) {
		if (!i.value()->isOriented())
			i.value()->getEdgeData(coords, texCoords, colors);
		else
			i.value()->getEdgeData(coordsO, texCoordsO, colorsO);
		i++;
	}

	edgesGeometry->setVertexArray(coords);
	edgesGeometry->setTexCoordArray(0, texCoords);
	edgesGeometry->setColorArray(colors);

	edgesOGeometry->setVertexArray(coordsO);
	edgesOGeometry->setTexCoordArray(0, texCoordsO);
	edgesOGeometry->setColorArray(colorsO);
}

osg::ref_ptr<osg::StateSet> SceneElements::createStateSet() const {
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();

	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	stateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

	return stateSet;
}
