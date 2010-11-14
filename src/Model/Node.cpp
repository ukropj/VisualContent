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

#include <qDebug>

typedef osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>
		ColorIndexArray;
using namespace Model;

Node::Node(qlonglong id, QString name, Type* nodeType, Graph* graph) {
	this->id = id;
	this->name = name;
	this->type = nodeType;
	this->graph = graph;
	this->edges = new QMap<qlonglong, Edge*> ;

	int pos = 0;
	int cnt = 0;

	labelText = this->name;

	while ((pos = labelText.indexOf(QString(" "), pos + 1)) != -1) {
		if (++cnt % 3 == 0)
			labelText = labelText.replace(pos, 1, "\n");
	}

	position = osg::Vec3f(0, 0, 0);
	force = osg::Vec3f();
	velocity = osg::Vec3f(0, 0, 0);
	fixed = false;
	ignore = false;
	frozen = false;
	osgNode = NULL;
}

Node::~Node(void) {
	foreach(qlonglong i, edges->keys())
		{
			edges->value(i)->unlinkNodes();
		}
	edges->clear();

	delete edges;
}

void Node::addEdge(Edge* edge) {
	edges->insert(edge->getId(), edge);
}

void Node::removeEdge(Edge* edge) {
	edges->remove(edge->getId());
}

void Node::removeAllEdges() {
	foreach(qlonglong i, edges->keys())
		{
			edges->value(i)->unlinkNodesAndRemoveFromGraph();
		}
	edges->clear();
}

bool Node::equals(Node* node) {
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
