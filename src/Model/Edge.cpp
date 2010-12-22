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
	dstNode->addEdge(this);
	srcNode->addEdge(this);
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
//	osgEdge = NULL;
}

Node* Edge::getOtherNode(Model::Node* node) const {
	if (node->getId() == srcNode->getId())
		return dstNode;
	else
		return srcNode; // returns source node even if node is not a node of this edge
}

PseudoEdge::PseudoEdge(Node* srcNode, Node* dstNode) {
	this->srcNode = srcNode;
	this->dstNode = dstNode;
	this->real = false;
	QString name;
	id = computeId(srcNode, dstNode);
}

uint PseudoEdge::computeId(Node* srcNode, Node* dstNode) {
	QString name;
	if (srcNode->getId() < dstNode->getId())
		name = QString::number(srcNode->getId()) + "_" + QString::number(dstNode->getId());
	else
		name = QString::number(dstNode->getId()) + "_" + QString::number(srcNode->getId());
	return qHash(name);
}
