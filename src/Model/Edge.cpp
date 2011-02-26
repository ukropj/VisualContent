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

Edge::Edge(qlonglong id, Node* srcNode, Node* dstNode, Type* type,
		Data* data, Graph* graph) {
	Q_ASSERT(graph != NULL);
	Q_ASSERT(srcNode != NULL && dstNode != NULL);
	Q_ASSERT(type != NULL);

	this->id = id;
	this->graph = graph;
	this->type = type;
	this->srcNode = srcNode;
	this->dstNode = dstNode;
	dstNode->addEdge(this);
	srcNode->addEdge(this);
	this->edgeData = data ? data : new Data();
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

QString Edge::data(Type::DataType key) const {
	return edgeData->value(type->getMapping(key));
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
