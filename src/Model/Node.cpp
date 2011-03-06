/*!
 * Node.cpp
 * Projekt 3DVisual
 *
 * TODO - reload configu sa da napisat aj efektivnejsie. Pri testoch na hranach priniesol vsak podobny prepis len male zvysenie vykonu. Teraz na to
 * nemam cas, takze sa raz k tomu vratim 8)
 */
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Model/Type.h"
#include "Model/Graph.h"
#include "Util/Config.h"

#include <QDebug>

using namespace Model;

Node::Node(qlonglong id, Type* type, QMap<QString, QString>* data, Graph* graph) {
	Q_ASSERT(graph != NULL);
	Q_ASSERT(type != NULL);

	this->id = id;
	this->graph = graph;
	this->type = type;
	this->nodeData = data ? data : new QMap<QString, QString>();

	int pos = 0;
	int cnt = 0;

	position = osg::Vec3f(0, 0, 0);
	force = osg::Vec3f();
	velocity = osg::Vec3f(0, 0, 0);
	fixed = false;
	ignore = false;
	frozen = false;
	osgNode = NULL;
	edges.clear();
}

Node::~Node(void) {
	graph = NULL;
	qDeleteAll(edges);
	// NOTE: node deletes edges, not vice versa, edge will in turn call Node::removeEdge()
	if (osgNode != NULL) {
		osgNode = NULL;
	}
}

void Node::addEdge(Edge* edge) {
	edges.insert(edge->getId(), edge);
}

void Node::removeEdge(Edge* edge) {
	edges.remove(edge->getId());
}

Edge* Node::getEdgeTo(const Node* otherNode) const {
	for (EdgeIt i = edges.constBegin(); i != edges.constEnd(); ++i) {
		Edge* edge = i.value();
		if (this->equals(edge->getOtherNode(otherNode)))
				return edge;
	}
	return NULL;
}

QSet<Node*> Node::getIncidentNodes() const {
	// TODO replace foreadch with STL iterator
	QSet<Node*> nodes;
	for (EdgeIt i = edges.constBegin(); i != edges.constEnd(); ++i) {
		Edge* edge = i.value();
		nodes.insert(edge->getOtherNode(this));
	}
	return nodes;
}

void Node::setOsgNode(Vwr::OsgNode* osgNode) {
	this->osgNode = osgNode;
}

QString Node::data(QString key) const {
	return nodeData->value(key);
}

bool Node::equals(Node* node) const {
	if (this == node) {
		return true;
	}
	if (node == NULL) {
		return false;
	}
	if (id != node->getId()) {
		return false;
	}
	if ((graph == NULL && node->getGraph() != NULL) || (graph != NULL
			&& node->getGraph() == NULL)) {
		return false;
	}
	if (graph == NULL && node->getGraph() == NULL) {
		return true;
	}
	return true;
}
