#ifndef DATA_GRAPH_DEF
#define DATA_GRAPH_DEF 1

#include <QString>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QtSql>
#include <QMutableMapIterator>

#include <osg/ref_ptr>
#include <osg/Vec3f>

/// Data representations of graph and its elements.
namespace Model  {
class Node;
class Edge;
class Type;
class Cluster;

typedef QMap<qlonglong, Node*>::const_iterator NodeIt;
typedef QMap<qlonglong, Edge*>::const_iterator EdgeIt;
typedef QMap<qlonglong, Type*>::const_iterator TypeIt;

/**
 *  \class Graph
 *  \brief Object represents a graph structure.
 *
 *	Graph object is a representation of an graph. A graph is a set of Nodes connected via Edges. Each Node and/or Edge has its Type.
 *	A graph is layouted and displayed by the application.
 *
 *  \author Aurel Paulovic
 *  \author Jakub Ukrop
 */
class Graph {
public:

	Graph(QString name = "graph", qlonglong ele_id_counter = 0);
	~Graph();

	QString setName(QString newName);
	QString getName() const {return name;}
	qlonglong getEleIdCounter() const {return ele_id_counter;}

	Node* addNode(Type* type = NULL, QMap<QString, QString>* data = NULL);

	Edge* addEdge(Node* srcNode, Node* dstNode, Type* type = NULL, QMap<QString, QString>* data = NULL);
	Edge* addEdge(qlonglong srcNodeId, qlonglong dstNodeId, Type* type = NULL, QMap<QString, QString>* data = NULL);

	Type* addType(QString name, QMap<QString, QString> *settings = NULL); //implemented

	void removeNode(Node* node);
	void removeEdge(Edge* edge);
	void removeType(Type* type);

	QMap<qlonglong, Node*>* getNodes() {return &nodes;}
	QMap<qlonglong, Edge*>* getEdges() {return &edges;}
	QMap<qlonglong, Type*>* getTypes() {return &types;}
	QList<Type*> getTypesByName(QString name);

	bool areIncident(Node* u, Node* v) const;

	bool isFrozen() const {return frozen;}
	void setFrozen(bool val) {frozen = val;}
	QString toString() const;

private:
	qlonglong incEleIdCounter() {return ++ele_id_counter;}
	qlonglong getMaxEleIdFromElements();

	void removeAllEdgesOfType(Type* type);
	void removeAllNodesOfType(Type* type);

	QString name;
	qlonglong ele_id_counter;
	bool frozen;

	QMap<qlonglong, Node* > nodes;
	QMap<qlonglong, Edge* > edges;
	QMap<qlonglong, Type*> types;

	QMultiMap<QString, Type*> typesByName;
	QMultiMap<qlonglong, Edge* > edgesByType;
	QMultiMap<qlonglong, Node* > nodesByType;

	friend class Clusterer;
};
}

#endif
