/*!
 * Graph.cpp
 * Projekt 3DVisual
 */
#include "Model/Graph.h"
#include "Model/Type.h"
#include "Model/Edge.h"
#include "Model/Node.h"
#include "Model/Cluster.h"

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

Cluster* Graph::addCluster(Type* type) { // TODO cluster type
	if (type == NULL) {
		type = addType("new_type_cluster" + incEleIdCounter());
	}
	Cluster* node = new Cluster(incEleIdCounter(), type, this);

	clusters.insert(node->getId(), node);
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
	Edge* e = v->getEdgeTo(u);
	return e != NULL && !e->isIgnored();
}

/*
int WHITE = 0, GRAY = 1, BLACK = 2;
double flow[][], capacity[][], res_capacity[][];
int parent[], color[];
double min_capacity[];
double max_flow;*/

void Graph::cluster(QMap<qlonglong, Node* > someNodes, bool clustersVisible, int maxLevels) {
	qDebug() << "clustering starts " << someNodes.size();
	clusters.clear();
	for (NodeIt ui = someNodes.begin(); ui != someNodes.end(); ++ui) {
		Node* u = ui.value();
//		qDebug() << "u: " << u->getId();
		if (u->getParent() == NULL) {
			Cluster* c = NULL;
			QSet<Node*> in = u->getIncidentNodes();
			QSet<Node*>::const_iterator i = in.begin();
			while (i != in.end()) {
				Node* v = *i;
//				qDebug() << "v: " << v->getId();
				if (v->getParent() == NULL) {
					if (c == NULL) {
						c = addCluster(u->getType()); // TODO cluster type
//						qDebug() << "new c: " << c->getId();
					}
					v->setParent(c);
					v->setIgnored(clustersVisible);
//					qDebug() << "v added to c";
				}
				++i;
			}
			if (c != NULL) {
				u->setParent(c);
				c->setIgnored(!clustersVisible);
				c->setExpanded(!clustersVisible);
				u->setIgnored(clustersVisible);
//				qDebug() << "u added to c";
			}
		}
	}
	nodes.unite(clusters);
	if (clusters.size() > 1 && maxLevels != 0) {
		QMap<qlonglong, Node*> newNodes(clusters);
		clusters.clear();
		cluster(newNodes, clustersVisible, maxLevels - 1);
	}
	qDebug() << "clustering ends " << clusters.size() << "/" <<  nodes.size();
}

/*void Graph::maxFlow(Node* source, Node* sink) {
	flow = new double[size][size];
	res_capacity = new double[size][size];
	parent = new int[size];
	min_capacity = new double[size];
	color = new int[size];
	queue = new int[size];

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			res_capacity[i][j] = capacity[i][j];

	while (BFS(source))
	{
		max_flow += min_capacity[sink];
		int v = sink, u;
		while (v != source)
		{
			u = parent[v];
			flow[u][v] += min_capacity[sink];
			flow[v][u] -= min_capacity[sink];
			res_capacity[u][v] -= min_capacity[sink];
			res_capacity[v][u] += min_capacity[sink];
			v = u;
		}
	}
}

bool Graph::BFS(Node* source, Node* sink) {
	int size = nodes.size();

	for (int i = 0; i < size; i++) {
		color[i] = WHITE;
		min_capacity[i] = INT_MAX;
	}

	first = last = 0;
//	queue[last++] = source;
	color[source] = GRAY;
	QQueue<Node*> queue;
	queue.enqueue(source);

	while (!queue.isEmpty()) {
//	while (first != last) {
		Node* v = queue.dequeue();
//		int v = queue[first++];
		for (NodeIt i = nodes->constBegin(); i != nodes->constEnd(); ++i) {
			Node* u = i.value();
//		for (int u = 0; u < size; u++)
			if (color[u] == WHITE && res_capacity[v][u] > 0) {
				min_capacity[u] = qMin(min_capacity[v], res_capacity[v][u]);
				parent[u] = v;
				color[u] = GRAY;
				if (u->equals(sink))
					return true;
//				queue[last++] = u;
				queue.enqueue(u);
			}
		}
	}
	return false;
}*/
