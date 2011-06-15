#include "Model/Node.h"
#include "Model/Cluster.h"
#include "Model/Edge.h"
#include "Model/Type.h"
#include "Model/Graph.h"
#include "Util/Config.h"
#include <QTextStream>
#include <math.h>

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
	weight = 1;
	osgNode = NULL;
	edges.clear();

	parent = NULL;
	nodeData->insert("id", QString("N%1").arg(id));
}

Node::~Node(void) {
	// don't delete node directly, use Graph::removeNode()
	edges.clear();
	graph = NULL;
	osgNode = NULL;
//	qDebug() << "node removed";
}

void Node::addEdge(Edge* edge) {
	edges.insert(edge->getId(), edge);
}

void Node::removeEdge(Edge* edge) {
	edges.remove(edge->getId());
}

void Node::removeAllEdges() {
	edges.clear();
}

Edge* Node::getEdgeTo(const Node* otherNode, bool ignoreClusters) const {
	if (edges.size() > otherNode->edges.size())
		return otherNode->getEdgeTo(this, ignoreClusters);

	for (EdgeIt i = edges.constBegin(); i != edges.constEnd(); ++i) {
		Edge* edge = i.value();
		if (this->equals(edge->getOtherNode(otherNode)))
			return edge;
	}
	return NULL;
}

QSet<Node*> Node::getIncidentNodes(bool ignoreClusters) const {
	QSet<Node*> nodes;
	for (EdgeIt i = edges.constBegin(); i != edges.constEnd(); ++i) {
		Edge* edge = i.value();
		nodes.insert(edge->getOtherNode(this));
	}

	if (ignoreClusters) {
		return nodes;
	} else {
		QSet<Node*> visibleNodes;
		QSetIterator<Node*> nodeIt(nodes);
		while (nodeIt.hasNext()) {
			Node* node = nodeIt.next();
			Node* cluster = node->getTopCluster();
			if (cluster != NULL && cluster != this) {
				visibleNodes.insert(cluster);
			} else {
				visibleNodes.insert(node);
			}
		}
		return visibleNodes;
	}
}

void Node::setOsgNode(Vwr::OsgNode* osgNode) {
	this->osgNode = osgNode;
}

QString Node::data(QString key) const {
	return nodeData->value(key);
}

float Node::getWeight() const {
	return weight;
}

bool Node::equals(const Node* node) const {
	if (this == node) {
		return true;
	}
	if (node == NULL) {
		return false;
	}
	if (id != node->getId()) {
		return false;
	}
	return true;
}

QString Node::toString() const {
	QString str;
	QTextStream(&str) << "N" << getId()
//			<< " [" << position.x() << "," << position.y() << "," << position.z() << "]"
//			<< (isFixed() ? " fixed" : "")
//			<< " p:" << (parent != NULL ? "yes" : "no parent")
//			<< " ch:" << children.size()
			;
	return str;
}

void Node::setIgnored(bool flag) {
	if (ignore == flag)
		return;
	ignore = flag;
}

void Node::setParent(Cluster* newParent) {
	if (newParent == NULL) {
		qWarning() << "Node " << toString() << " cannot have NULL as parent!";
		return;
	}
	if (parent != NULL) {
		qWarning() << "Node " << toString() << " already has parent! (" << parent->toString() << ")";
		return;
	}
	if (this->equals(newParent)) {
		qWarning() << "Cannot set self as parent! (" << toString() << ")";
		return;
	}
	newParent->weight += this->weight;
	newParent->children.insert(this);
	parent = newParent;
}

bool Node::canBeClustered() const {
	if (isIgnored())
		return false;
	if (parent == NULL || !parent->canCluster()) {
		return false;
	}
	return true;
}

Node* Node::getTopCluster() const {
	if (!isIgnored() || parent == NULL) {
		return NULL;
	} else {
		if (parent->isIgnored())
			return parent->getTopCluster();
		else
			return parent;
	}
}

