#include "Viewer/SceneElements.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Viewer/OsgCluster.h"
#include "Model/Type.h"
#include "Model/Node.h"
#include "Model/Cluster.h"
#include "Model/Edge.h"
#include "Util/Config.h"

#include <QDebug>
#include <iostream>

using namespace Vwr;
using namespace Model;

SceneElements::SceneElements(QMap<qlonglong, Node*>* nodes, QMap<qlonglong,
		Edge*>* edges, QMap<qlonglong, DataMapping*>* dataMappings, QProgressDialog* progressBar) {

	this->dataMappings = dataMappings;
	elementsGroup = new osg::Group();
	elementsGroup->setName("scene_elements");

	pd = progressBar;
	if (pd != NULL) {
		if (pd->wasCanceled())
			return;
		pd->reset();
		pd->setLabelText("Drawing graph ...");
		pd->setMaximum(nodes->size() + edges->size());
		step = 0;
	}

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

QList<OsgNode*> SceneElements::getNodes() {
	return nodes;
}

QList<OsgEdge*> SceneElements::getEdges() {
	return edges;
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
		if (pd != NULL && pd->wasCanceled()) return nodeGroup;
		i.next();
		osg::ref_ptr<osg::Group> g;

		g = getNodeGroup2(i.value());
//		g = getNodeGroup1(i.value(), NULL);
		if (g != NULL) {
			nodeGroup->addChild(g);
		}
	}
	nodeIds.clear();
	QListIterator<OsgNode*> j(nodes);
	while (j.hasNext()) {
		j.next()->resolveParent();
		// cannot be done in constructor, as parent might not exist yet
	}
	return nodeGroup;
}

osg::ref_ptr<osg::Group> SceneElements::initEdges(
		QMap<qlonglong, Edge*>* inEdges) {
	osg::ref_ptr<osg::Group> allEdges = new osg::Group;
	edgesGeometry = new osg::Geometry();
	edgesOGeometry = new osg::Geometry();
	edgeLabels = new osg::Geode();

	osg::Vec4Array* colors = new osg::Vec4Array;
	osg::Vec4Array* colorsO = new osg::Vec4Array;

	QListIterator<Edge*> i(inEdges->values());
	int index = 0;
	int indexO = 0;
	while (i.hasNext()) {
		if (pd != NULL) {
			if (pd->wasCanceled()) return allEdges;
			pd->setValue(step++);
		}
		Model::Edge* edge = i.next();
		DataMapping* mapping = dataMappings->value(edge->getType()->getId());
		OsgEdge* osgEdge = new OsgEdge(edge, mapping);
		edges.append(osgEdge);
		if (!osgEdge->isDirected()) {
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
		edgeLabels->addDrawable(osgEdge->getLabel());
	}

	edgesGeometry->setStateSet(OsgEdge::createStateSet(OsgEdge::UNDIRECTED));
	edgesOGeometry->setStateSet(OsgEdge::createStateSet(OsgEdge::DIRECTED));

	edgesGeometry->setColorArray(colors);
	edgesOGeometry->setColorArray(colorsO);

	// important: set color binding only after setting color arrays!
	edgesGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	edgesOGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	osg::ref_ptr<osg::Geode> edgeGeode = new osg::Geode();
	edgeGeode->setName("edges");

	edgeGeode->addDrawable(edgesGeometry);
	edgeGeode->addDrawable(edgesOGeometry);

	allEdges->addChild(edgeGeode);
	allEdges->addChild(edgeLabels);
	//	allEdges->getOrCreateStateSet()->setRenderBinDetails(1, "DepthSortedBin");
	edgeLabels->setNodeMask(false);
	return allEdges;
}

osg::ref_ptr<osg::Group> SceneElements::getNodeGroup1(Node* node,
		Edge* parentEdge) {
	osg::ref_ptr<osg::Group> group = NULL;

	if (!nodeIds.contains(node->getId())) {
		group = new osg::Group;
		group->addChild(wrapNode(node));

		QMap<qlonglong, Edge*>::const_iterator i = node->getEdges()->begin();
		while (i != node->getEdges()->end()) {
			if (pd != NULL && pd->wasCanceled()) return NULL;
			Edge* e = *i;
			if (e != parentEdge) {
				Node* otherNode = e->getOtherNode(node);
				osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup1(otherNode, e);

				if (nodeGroup != NULL)
					group->addChild(nodeGroup);
			}
			++i;
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
			if (pd != NULL && pd->wasCanceled()) return NULL;
			osg::ref_ptr<osg::Group> group = NULL;
			Node* node = nodeList[i];

			QMap<qlonglong, Edge*>::iterator ei = node->getEdges()->begin();
			while (ei != node->getEdges()->end()) {
				Node* otherNode = (*ei)->getOtherNode(node);
				if (!nodeIds.contains(otherNode->getId())) {
					if (group == NULL)
						group = new osg::Group;
					group->addChild(wrapNode(otherNode));
					nodeList << otherNode;
				}
				++ei;
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

osg::ref_ptr<OsgNode> SceneElements::wrapNode(Node* node) {
	if (pd != NULL) {
		pd->setValue(step++);
	}
	OsgNode* osgNode;
	Cluster* cluster = dynamic_cast<Cluster*>(node);
	DataMapping* mapping = dataMappings->value(node->getType()->getId());
	if (cluster != NULL) {
		osgNode = new OsgCluster(cluster, mapping);
	} else {
		osgNode = new OsgNode(node, mapping);
	}
	nodes.append(osgNode);
	nodeIds.insert(node->getId());

	return osgNode;
}

void SceneElements::updateNodes(float interpolationSpeed, float maxClusterSize) {
	QList<OsgNode* >::const_iterator i = nodes.constBegin();

	while (i != nodes.constEnd()) {
		OsgNode* n = *i;
		if (n->isVisible()) {
			n->updatePosition(interpolationSpeed);
			n->updateClusterState(maxClusterSize);
		}
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

	QList<OsgEdge*>::const_iterator i = edges.constBegin();
	while (i != edges.constEnd()) {
		if (!(*i)->isDirected())
			(*i)->getEdgeData(coords, texCoords, colors);
		else
			(*i)->getEdgeData(coordsO, texCoordsO, colorsO);
		++i;
	}

	edgesGeometry->setVertexArray(coords);
	edgesGeometry->setTexCoordArray(0, texCoords);
	edgesGeometry->setColorArray(colors);

	edgesOGeometry->setVertexArray(coordsO);
	edgesOGeometry->setTexCoordArray(0, texCoordsO);
	edgesOGeometry->setColorArray(colorsO);
}

void SceneElements::setEdgeLabelsVisible(bool visible) {
	edgeLabels->setNodeMask(visible);
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
