#include "Viewer/SceneElements.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"

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
		pd->setValue(0);
		pd->setMaximum(nodes->size() + edges->size());
		step = 0;
	}

	elementsGroup = new osg::Group();
	elementsGroup->setName("scene_elements");

	if (!nodes->isEmpty())
		elementsGroup->addChild(initNodes(nodes));
	if (!edges->isEmpty())
		elementsGroup->addChild(initEdges(edges));

	elementsGroup->setStateSet(createStateSet());
}

SceneElements::~SceneElements() {
	qDeleteAll(edges);	// nodes are deleted automatically (osg::Referenced)
}

osg::Group* SceneElements::getElementsGroup() {
	return elementsGroup;
}

QList<OsgNode* > SceneElements::getNodes() {
	return nodes;
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

/*osg::ref_ptr<osg::Group> SceneElements::initNodes(QMap<qlonglong, Node*>* inNodes) {
	osg::ref_ptr<osg::Group> allNodes = new osg::Group;
	osg::ref_ptr<osg::Geode> nodeGeode = new osg::Geode();
	nodeGeode->setName("nodes");
	allNodes->addChild(nodeGeode);
	//	allEdges->getOrCreateStateSet()->setRenderBinDetails(1, "DepthSortedBin");

	nodesGeometry = new osg::Geometry();
	osg::Vec4Array* colors = new osg::Vec4Array;

	QMapIterator<qlonglong, Node*> i(*inNodes);
	int index = 0;
	while (i.hasNext()) {
		if (pd != NULL)
			pd->setValue(step++);
		i.next();
		OsgNode* osgNode = new OsgNode(i.value(), NULL);
		nodes.append(osgNode);
		nodesGeometry->addPrimitiveSet(new osg::DrawArrays(
				osg::PrimitiveSet::QUADS, index, 4));
		for (int i = 0; i < 4; i++)
			colors->push_back(osg::Vec4f(0,0,0,1));
		index += 4;
	}

	// TODO move to static method OsgNode::createStateSet();
	osg::StateSet* ss = nodesGeometry->getOrCreateStateSet();
	ss->setTextureAttributeAndModes(0,
			Util::TextureWrapper::readTextureFromFile(Util::Config::getValue(
			"Viewer.Textures.Node")), osg::StateAttribute::ON);
//	nodesGeometry->setStateSet(ss);
	nodesGeometry->setColorArray(colors);
	// important: set color binding only after setting color arrays!
	nodesGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	nodeGeode->addDrawable(nodesGeometry);
	return allNodes;
}*/

osg::ref_ptr<osg::Group> SceneElements::initEdges(
		QMap<qlonglong, Edge*>* inEdges) {
	osg::ref_ptr<osg::Group> allEdges = new osg::Group;
	osg::ref_ptr<osg::Geode> edgeGeode = new osg::Geode();
	edgeGeode->setName("edges");
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
		OsgEdge* osgEdge = new OsgEdge(i.value());
		edges.append(osgEdge);
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

	if (!nodeIds.contains(node->getId())) {
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

	if (nodeIds.contains(firstNode->getId()))
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
				if (!nodeIds.contains(otherNode->getId())) {
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

	OsgNode* osgNode = new OsgNode(node, at);

	nodes.append(osgNode);
	nodeIds.insert(node->getId());

	at->setName("node_transform");
	at->addChild(osgNode);
	return at;
}

void SceneElements::updateNodes(float interpolationSpeed) {
	QList<OsgNode* >::const_iterator i = nodes.constBegin();

	while (i != nodes.constEnd()) {
		(*i)->updatePosition(interpolationSpeed);
		++i;
	}
}

/*void SceneElements::updateNodes(float interpolationSpeed) {
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	QList<OsgNode*>::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		(*i)->getNodeData(interpolationSpeed, coords, texCoords, colors);
		i++;
	}

	nodesGeometry->setVertexArray(coords);
	nodesGeometry->setTexCoordArray(0, texCoords);
	nodesGeometry->setColorArray(colors);
}*/

void SceneElements::updateEdges() {
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	osg::ref_ptr<osg::Vec3Array> coordsO = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texCoordsO = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec4Array> colorsO = new osg::Vec4Array;

	QList<OsgEdge*>::const_iterator i = edges.constBegin();
	while (i != edges.constEnd()) {
		if (!(*i)->isOriented())
			(*i)->getEdgeData(coords, texCoords, colors);
		else
			(*i)->getEdgeData(coordsO, texCoordsO, colorsO);
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
