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

	//pre Misa
	this->nodes = new QMap<qlonglong, Node* > ();
	this->edges = new QMap<qlonglong, Edge* > ();
	this->types = new QMap<qlonglong, Type*> ();
	this->frozen = false;
	this->typesByName = new QMultiMap<QString, Type*> ();
}

Graph::~Graph(void) {
	//uvolnime vsetky Nodes, Edges... su cez osg::ref_ptr takze staci clearnut
	// FIXME uz nie su :D
	this->nodes->clear();
	delete this->nodes;
	this->nodes = NULL;

	this->edges->clear();
	delete this->edges;
	this->edges = NULL;

	this->newTypes.clear();
	this->newNodes.clear();
	this->newEdges.clear();
	this->nodesByType.clear();
	this->edgesByType.clear();

	this->typesByName->clear();
	delete this->typesByName;
	this->typesByName = NULL;

	//uvolnime types - treba iterovat a kazde jedno deletnut samostatne
	QMap<qlonglong, Type*>::iterator it = this->types->begin();

	Type* type;
	while (it != this->types->end()) {
		type = it.value();
		it = this->types->erase(it);
		delete type;
		type = NULL;
	}

	this->types->clear();
	delete this->types;
	this->types = NULL;
}

QString Graph::setName(QString name) {
	QString newName = name;

	if (newName != NULL) {
		this->name = newName;
	}

	return this->name;
}

Node* Graph::addNode(QString name, Type* type) {
	Node* node = new Node(this->incEleIdCounter(), name, type, this);

	newNodes.insert(node->getId(), node);
	nodes->insert(node->getId(), node);
	nodesByType.insert(type->getId(), node);

	return node;
}

Edge* Graph::addEdge(QString name,
		Node* srcNode, Node* dstNode,
		Type* type, bool isOriented) {
	Edge* edge = new Edge(this->incEleIdCounter(),
			name, this, srcNode, dstNode, type, isOriented);

	edge->linkNodes(&newEdges);
	edge->linkNodes(edges);
	edgesByType.insert(type->getId(), edge);

	return edge;
}

Type* Graph::addType(QString name, QMap<QString, QString> *settings) {
	Type* type = new Type(this->incEleIdCounter(), name, this,
			settings);

	this->newTypes.insert(type->getId(), type);
	this->types->insert(type->getId(), type);
	this->typesByName->insert(type->getName(), type);

	return type;
}

qlonglong Graph::getMaxEleIdFromElements() {
	qlonglong max = 0;

	if (this->nodes != NULL && !this->nodes->isEmpty()) {
		QMap<qlonglong, Node* >::iterator iNodes =
				this->nodes->end();
		iNodes--;

		if (iNodes.key() > max)
			max = iNodes.key();
	}

	if (this->nodes != NULL && !this->types->isEmpty()) {
		QMap<qlonglong, Type*>::iterator iTypes = this->types->end();
		iTypes--;

		if (iTypes.key() > max)
			max = iTypes.key();
	}

	if (this->nodes != NULL && !this->edges->isEmpty()) {
		QMap<qlonglong, Edge* >::iterator iEdges =
				this->edges->end();
		iEdges--;

		if (iEdges.key() > max)
			max = iEdges.key();
	}

	return max;
}

QString Graph::toString() const {
	QString str;
	QTextStream(&str) << " name: " << name << " elements: "
			<< ele_id_counter;
	return str;
}

QList<Type*> Graph::getTypesByName(QString name) {
	return this->typesByName->values(name);
}

void Graph::removeType(Type* type) {
	if (type != NULL && type->getGraph() == this) {
		this->types->remove(type->getId());
		this->newTypes.remove(type->getId());
		this->typesByName->remove(type->getName());

		//vymazeme vsetky hrany resp. metahrany daneho typu
		this->removeAllEdgesOfType(type);

		//vymazeme vsetky uzly daneho typu
		this->removeAllNodesOfType(type);

	}
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
	QList<Node* > nodesToKill;
	nodesToKill = nodesByType.values(type->getId());

	if (!nodesToKill.isEmpty()) {
		for (qlonglong i = 0; i < nodesToKill.size(); i++) { //prejdeme kazdy jeden uzol
			this->removeNode(nodesToKill.at(i));
		}
		nodesToKill.clear();
	}
}

void Graph::removeEdge(Edge* edge) {
	if (edge != NULL && edge->getGraph() == this) {
		this->edges->remove(edge->getId());
		this->newEdges.remove(edge->getId());
		this->edgesByType.remove(edge->getType()->getId(), edge);
		edge->unlinkNodes();
	}

}

void Graph::removeNode(Node* node) {
	if (node != NULL && node->getGraph() == this) {

		this->nodes->remove(node->getId());
		this->newNodes.remove(node->getId());
		this->nodesByType.remove(node->getType()->getId(), node);
		node->removeAllEdges();

		//zistime ci nahodou dany uzol nie je aj typom a osetrime specialny pripad ked uzol je sam sebe typom (v DB to znamena, ze uzol je ROOT uzlom/typom, teda uz nemoze mat ziaden iny typ)
		if (this->types->contains(node->getId())) {
			this->removeType(this->types->value(node->getId()));
		}

	}
}
