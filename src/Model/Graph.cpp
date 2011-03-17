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
//	qDebug() << "Graph created " << name;
	this->name = name;
	this->ele_id_counter = ele_id_counter;
	frozen = false;
}

Graph::~Graph() {
	qDeleteAll(edges); // edges must be deleted before nodes (see ~Edge())
	qDeleteAll(nodes);
	qDeleteAll(types);

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

Node* Graph::addNode(Type* type, QMap<QString, QString>* data) {
	if (type == NULL) {
		type = addType("new_type_node" + incEleIdCounter());
	}
	Node* node = new Node(incEleIdCounter(), type, data, this);

	nodes.insert(node->getId(), node);
	nodesByType.insert(type->getId(), node);
	return node;
}

Edge* Graph::addEdge(qlonglong srcNodeId, qlonglong dstNodeId, Type* type, QMap<QString, QString>* data) {
	return addEdge(nodes.value(srcNodeId), nodes.value(dstNodeId), type, data);
}

Edge* Graph::addEdge(Node* srcNode, Node* dstNode, Type* type, QMap<QString, QString>* data) {
	if (srcNode == NULL || dstNode == NULL) {
		qWarning() << "Trying to add edge to NULL node!";
		return NULL;
	}
	if (srcNode->equals(dstNode)) {
		qWarning() << "Trying to add edge to self on node: " << srcNode->toString();
		return NULL;
	}
	if (areIncident(srcNode, dstNode)) {
		//qWarning() << "Edge between " << srcNode->toString() << " and "
		//		<< dstNode->toString() << "already exists!";
		return NULL;
	}

	if (type == NULL) {
		type = addType("new_type_edge" + incEleIdCounter());
	}
	Edge* edge = new Edge(incEleIdCounter(), srcNode, dstNode, type, data, this);

	edges.insert(edge->getId(), edge);
	edgesByType.insert(type->getId(), edge);
	return edge;
}

Type* Graph::addType(QString name, QMap<QString, QString> *settings) {
	Type* type = new Type(incEleIdCounter(), name, this, settings);
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
	QTextStream(&str) << name << " (" << nodes.size() << " nodes, "
			<< edges.size() << " edges, " << types.size() << " types)";
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
		for (qlonglong i = 0; i < nodesToKill.size(); i++) {
			removeNode(nodesToKill.at(i));
		}
		nodesToKill.clear();
	}
}

void Graph::removeNode(Node* node) {
	if (node != NULL && node->getGraph() == this) {
		QList<Edge*> edgesToKill = node->getEdges()->values();
		node->removeAllEdges();
		qDebug() << edgesToKill.size();
		for (qlonglong i = 0; i < edgesToKill.size(); i++) {
			removeEdge(edgesToKill.at(i));
		}
		nodes.remove(node->getId());
		nodesByType.remove(node->getType()->getId(), node);
		delete node;
	}
}

void Graph::removeEdge(Edge* edge) {
	if (edge != NULL && edge->getGraph() == this) {
		edges.remove(edge->getId());
		edgesByType.remove(edge->getType()->getId(), edge);
		delete edge;
	}
}

void Graph::removeType(Type* type) {
	if (type != NULL && type->getGraph() == this) {
		types.remove(type->getId());
		typesByName.remove(type->getName());

		removeAllEdgesOfType(type);
		removeAllNodesOfType(type);
	}
	qDebug() << "Type removed";
}

bool Graph::areIncident(Node* u, Node* v) const {
	if (u == NULL || v == NULL)
		return false;
	Edge* e;
	if (u->getEdges()->size() <= v->getEdges()->size())
		e = u->getEdgeTo(v);
	else
		e = v->getEdgeTo(u);
	return e != NULL && !e->isIgnored();
}

void Graph::cluster() {
	// TODO implement
	/*Type* clusterType = typesByName.value("node");
	Type* edgeType = typesByName.value("edge");
	int size = nodes.size();
	for (qlonglong i = 0; i < nodes.size(); i++) {
		Node* n = nodes.at(i);
		if (n->getEdges()->size() == 1) {
			Node* u = u->getIncidentNodes().at(0);
			QList<Node*> nghbrs = u->getIncidentNodes();
			for(QList<Node*>::const_iterator i = nghbrs.constBegin(); i != nghbrs.constEnd(); ++i) {
				if ((*i)->getEdges()->size() == 1) {
					(*i)->setParent(u);
				}
			}
		}
	}*/
}
