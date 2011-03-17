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
		QMap<QString, QString>* data, Graph* graph) {
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
	this->edgeData = data ? data : new QMap<QString, QString>();
	ignore = false;
	weight = 1;
}

Edge::~Edge(void) {
	// don't delete edge directly, use Graph::removeEdge()
	graph = NULL;
	srcNode->removeEdge(this);
	srcNode = NULL;
	dstNode->removeEdge(this);
	dstNode = NULL;
	type = NULL;
//	qDebug() << "edge removed";
}

Node* Edge::getOtherNode(const Model::Node* node) const {
	if (node == NULL)
		return NULL;
	if (node->getId() == srcNode->getId())
		return dstNode;
	if (node->getId() == dstNode->getId())
		return srcNode;
//	qWarning() << "Node not incident to this edge";
	return NULL;
}

QString Edge::data(QString key) const {
	return edgeData->value(key);
}
