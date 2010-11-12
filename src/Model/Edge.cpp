/*!
 * Edge.cpp
 * Projekt 3DVisual
 */
#include "Model/Edge.h"
#include "Model/Graph.h"
#include "Model/Node.h"
#include "Model/Type.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"

using namespace Model;

Edge::Edge(qlonglong id, QString name, Graph* graph, Node* srcNode,
		Node* dstNode, Type* type, bool isOriented) {

	this->id = id;
	this->name = name;
	this->graph = graph;
	this->srcNode = srcNode;
	this->dstNode = dstNode;
	this->type = type;
	this->oriented = isOriented;
}

Edge::~Edge(void) {
	graph = NULL;
	if (srcNode != NULL) {
		srcNode->removeEdge(this);
		srcNode = NULL;
	}
	if (dstNode != NULL) {
		dstNode->removeEdge(this);
		dstNode = NULL;
	}
	type = NULL;
}

void Edge::linkNodes(QMap<qlonglong, Edge*> *edges) {
	edges->insert(this->id, this);
	dstNode->addEdge(this);
	srcNode->addEdge(this);
}

void Edge::unlinkNodes() {
	dstNode->removeEdge(this);
	srcNode->removeEdge(this);
	srcNode = NULL;
	dstNode = NULL;
}

void Edge::unlinkNodesAndRemoveFromGraph() {
	//unlinkNodes will be called from graph->removeEdge !!
	graph->removeEdge(this);
}

Node* Edge::getOtherNode(Model::Node* node) const {
	if (node->getId() == srcNode->getId())
		return dstNode;
	else
		return srcNode; // returns source node even if node is not a node of this edge
}
