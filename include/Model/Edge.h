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

	qlonglong getId() const {
		return id;
	}

	QString data(QString key) const;

	Node* getSrcNode() const {
		return srcNode;
	}

	Node* getDstNode() const {
		return dstNode;
	}

	Node* getOtherNode(Node* node) const;

	Type* getType() const {
		return type;
	}

	void setReal(bool flag) {
		real = flag;
	}
	bool isReal() const {
		return real;
	}

	QString toString() const {
		QString str;
		QTextStream(&str) << "edge id:" << id;
		return str;
	}

	Graph* getGraph() const {
		return graph;
	}

private:
	Graph* graph;
	qlonglong id;

	bool real;
	Node* srcNode;
	Node* dstNode;

	Type* type;
	QMap<QString, QString>* edgeData;
};

class PseudoEdge {
public:
	PseudoEdge(Node* srcNode, Node* dstNode);
	~PseudoEdge() {}
	uint getId() const {return id;}
	void setReal(bool flag) {real = flag;}
	bool isReal() const {return real;}
	Node* getSrcNode() const {return srcNode;}
	Node* getDstNode() const {return dstNode;}

	static uint computeId(Node* srcNode, Node* dstNode);

private:
	Node* srcNode;
	Node* dstNode;
	uint id;
	bool real;
};
}

#endif
