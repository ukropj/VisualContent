/*!
 * Graph.cpp
 * Projekt 3DVisual
 */
#include "Model/Graph.h"
#include "Model/Type.h"
#include "Model/Edge.h"
#include "Model/Node.h"

using namespace Model;

Graph::Graph(QString name, qlonglong ele_id_counter) {
	this->name = name;
	this->ele_id_counter = ele_id_counter;
	this->frozen = false;
	nodes.clear();
	edges.clear();
	types.clear();
	newTypes.clear();
	newNodes.clear();
	newEdges.clear();
	nodesByType.clear();
	edgesByType.clear();
	typesByName.clear();
}

Graph::~Graph() {
	qDebug() << "deleting nodes";
//	qDebug() << nodes.size();
	qDeleteAll(nodes); // NOTE: deleting nodes will also delete edges
	qDebug() << "deleting types";

	qDeleteAll(types);

	newTypes.clear();
	newNodes.clear();
	newEdges.clear();

	nodesByType.clear();
	edgesByType.clear();
	typesByName.clear();
}

QString Graph::setName(QString newName) {
	if (newName != NULL) {
		this->name = newName;
	}
	return name;
}

Node* Graph::addNode(QString name, Type* type) {
	Node* node = new Node(incEleIdCounter(), name, type, this);

	newNodes.insert(node->getId(), node);
	nodes.insert(node->getId(), node);
	nodesByType.insert(type->getId(), node);

	return node;
}

Edge* Graph::addEdge(QString name, Node* srcNode, Node* dstNode, Type* type,
		bool isOriented) {
	Edge* edge = new Edge(incEleIdCounter(), name, this, srcNode,
			dstNode, type, isOriented);

	//	edge->linkNodes(&newEdges);
	//	edge->linkNodes(edges);
	newEdges.insert(edge->getId(), edge);
	edges.insert(edge->getId(), edge);
	edgesByType.insert(type->getId(), edge);

	return edge;
}

Type* Graph::addType(QString name, QMap<QString, QString> *settings) {
	Type* type = new Type(incEleIdCounter(), name, this, settings);

	newTypes.insert(type->getId(), type);
	types.insert(type->getId(), type);
	typesByName.insert(type->getName(), type);

	return type;
}

qlonglong Graph::getMaxEleIdFromElements() {
	qlonglong max = 0;

	if (!nodes.isEmpty()) {
		QMap<qlonglong, Node*>::iterator iNodes = nodes.end();
		iNodes--;

		if (iNodes.key() > max)
			max = iNodes.key();
	}

	if (!types.isEmpty()) {
		QMap<qlonglong, Type*>::iterator iTypes = types.end();
		iTypes--;

		if (iTypes.key() > max)
			max = iTypes.key();
	}

	if (!edges.isEmpty()) {
		QMap<qlonglong, Edge*>::iterator iEdges = edges.end();
		iEdges--;

		if (iEdges.key() > max)
			max = iEdges.key();
	}

	return max;
}

QString Graph::toString() const {
	QString str;
	QTextStream(&str) << name << " (" << ele_id_counter << " elements)";
	return str;
}

QList<Type*> Graph::getTypesByName(QString name) {
	return typesByName.values(name);
}

void Graph::removeAllEdgesOfType(Type* type) {
	QList<Edge*> edgesToKill;
	edgesToKill = edgesByType.values(type->getId());

	if (!edgesToKill.isEmpty()) {
		for (qlonglong i = 0; i < edgesToKill.size(); i++) {
			removeEdge(edgesToKill.at(i));
		}
		edgesToKill.clear();
	}
}

void Graph::removeAllNodesOfType(Type* type) {
	QList<Node*> nodesToKill;
	nodesToKill = nodesByType.values(type->getId());

	if (!nodesToKill.isEmpty()) {
		for (qlonglong i = 0; i < nodesToKill.size(); i++) { //prejdeme kazdy jeden uzol
			this->removeNode(nodesToKill.at(i));
		}
		nodesToKill.clear();
	}
}

void Graph::removeNode(Node* node) {
	if (node != NULL && node->getGraph() == this) {
		nodes.remove(node->getId());
		newNodes.remove(node->getId());
		nodesByType.remove(node->getType()->getId(), node);
		//		if (this->types->contains(node->getId())) {
		//			this->removeType(this->types->value(node->getId()));
		//		}
		delete node;
	}
}

void Graph::removeEdge(Edge* edge) {
	if (edge != NULL && edge->getGraph() == this) {
		edges.remove(edge->getId());
		newEdges.remove(edge->getId());
		edgesByType.remove(edge->getType()->getId(), edge);
		//		edge->unlinkNodes();
		delete edge;
	}
}

void Graph::removeType(Type* type) {
	if (type != NULL && type->getGraph() == this) {
		types.remove(type->getId());
		newTypes.remove(type->getId());
		typesByName.remove(type->getName());

		//vymazeme vsetky hrany resp. metahrany daneho typu
		removeAllEdgesOfType(type);
		//vymazeme vsetky uzly daneho typu
		removeAllNodesOfType(type);
	}
}
