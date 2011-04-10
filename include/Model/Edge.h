/*!
 * Edge.h
 * Projekt 3DVisual
 */
#ifndef DATA_EDGE_DEF
#define DATA_EDGE_DEF 1

#include <string>
#include <QString>
#include <QTextStream>
#include <QtCore/QMap>

namespace Model {
class Type;
class Node;
class Graph;

/**
 *  \class Edge
 *  \brief Object represents an edge in a Graph.
 *  \author Aurel Paulovic, Michal Paprcka
 *  \date 29. 4. 2010
 */
class Edge {
public:

	Edge(qlonglong id, Node* srcNode, Node* dstNode, Type* type,
			QMap<QString, QString>* data, Graph* graph);
	~Edge(void);

	qlonglong getId() const {return id;}
	Type* getType() const {return type;}
	QString data(QString key) const;
	Graph* getGraph() const {return graph;}

	Node* getSrcNode() const {return srcNode;}
	Node* getDstNode() const {return dstNode;}
	Node* getOtherNode(const Node* node) const;

	void setIgnored(bool b) {ignore = b;}
	bool isIgnored() const {return ignore;}
	float getWeight() {return weight;}
	void setWeight(float weight) {this->weight = weight;}

	QString toString() const;

private:
	Graph* graph;
	qlonglong id;
	bool ignore;
	float weight;

	Node* srcNode;
	Node* dstNode;

	Type* type;
	QMap<QString, QString>* edgeData;
};

}

#endif
