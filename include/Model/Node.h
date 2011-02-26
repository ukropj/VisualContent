/*!
 * Node.h
 * Projekt 3DVisual
 */
#ifndef DATA_NODE_DEF
#define DATA_NODE_DEF 1

#include <osg/Vec3f>
#include <vector>
#include <string>
#include <QMap>
#include <QSet>
#include <QString>
#include <QTextStream>

#include "Model/Type.h"

namespace Vwr {
class OsgNode;
}

namespace Model {
class Data;
class Edge;
class Graph;

/**
 *  \class Node
 *  \brief Node object represents a single node in a Graph
 *  \author Aurel Paulovic, Michal Paprcka
 *  \date 29. 4. 2010
 */
class Node {
public:

	Node(qlonglong id, Type* type, Data* data, Graph* graph);
	~Node(void);

	void addEdge(Edge* edge);
	void removeEdge(Edge* edge);
	Edge* getEdgeTo(Node* otherNode);
	QSet<Node*> getIncidentNodes();

	qlonglong getId() const {
		return id;
	}

	Graph* getGraph() const {
		return graph;
	}

	Type* getType() const {
		return type;
	}

	QString data(Type::DataType key) const;

	osg::Vec3f getPosition() const {
		return osg::Vec3f(position);
	}

	void setPosition(osg::Vec3f pos) {
		position.set(pos);
	}

	QMap<qlonglong, Edge*>* getEdges() {
		return &edges;
	}

	void setForce(osg::Vec3f v) {
		force = v;
	}

	osg::Vec3f getForce() const {
		return force;
	}

	void addForce(osg::Vec3f v) {
		force += v;
	}

	void resetForce() {
		force = osg::Vec3f(0, 0, 0);
	}

	bool setFixed(bool flag) {
		if (flag == isFixed())
			return false;
		fixed = flag;
	}

	bool isFixed() const {
		return fixed;
	}

	void setFrozen(bool flag) {
		frozen = flag;
	}

	bool isFrozen() const {
		return frozen;
	}

	void setIgnored(bool b) {
		ignore = b;
	}

	bool isIgnored() const {
		return ignore;
	}

	void setVelocity(osg::Vec3f v) {
		velocity = v;
	}

	void resetVelocity() {
		velocity = osg::Vec3f(0, 0, 0);
	}

	osg::Vec3f getVelocity() const {
		return velocity;
	}

	/**
	 *  \fn public  equals(Node* node)
	 *  \brief Checks if the node and this object is the same object
	 *  \param   node     Node to compare
	 *  \return bool true, if this object and node are the same object
	 */
	bool equals(Node* node) const;

	QString toString() const {
		QString str;
		QTextStream(&str) << "N" << id << " " << getId() << "[" << position.x()
				<< "," << position.y() << "," << position.z() << "]"
				<< (isFixed() ? "fixed" : "");
		return str;
	}

	void setOsgNode(Vwr::OsgNode* osgNode);

	Vwr::OsgNode* getOsgNode() {
		return osgNode;
	}

private:

	qlonglong id;
	Data* nodeData;
	Type* type;
	Graph* graph;
	osg::Vec3f position;
	QMap<qlonglong, Edge*> edges;
	Vwr::OsgNode* osgNode;

	osg::Vec3f force;
	osg::Vec3f velocity;

	bool fixed;
	bool frozen;
	bool ignore;

};
}

#endif
