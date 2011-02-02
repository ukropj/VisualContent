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

#include "Viewer/OsgNode.h"

namespace Model {
class Type;
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

	Node(qlonglong id, QString name, Type* type, Graph* graph);
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

	QString getName() const {
		return name;
	}

	void setName(QString val) {
		name = val;
	}

	Type* getType() const {
		return type;
	}

	void setType(Type* val) {
		type = val;
	}

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
		QTextStream(&str) << "N" << id << " " << name << "[" << position.x()
				<< "," << position.y() << "," << position.z() << "]"
				<< (isFixed() ? "fixed" : "");
		return str;
	}

	QString getLabel() const {
		return labelText;
	}

	void setOsgNode(Vwr::OsgNode* osgNode);

	Vwr::OsgNode* getOsgNode() {
		return osgNode;
	}

private:

	Vwr::OsgNode* osgNode;
	/**
	 *	qlonglong id
	 *	\brief ID of the Node
	 */
	qlonglong id;

	/**
	 *  QString name
	 *  \brief Name of the Node
	 */
	QString name;
	QString labelText;

	/**
	 *  Type * type
	 *  \brief Type of the Node
	 */
	Type* type;

	/**
	 *  Graph * graph
	 *  \brief Graph object to which the Node belongs
	 */
	Graph* graph;

	/**
	 *  osg::Vec3f targetPosition
	 *  \brief node target position
	 */
	osg::Vec3f position;

	/**
	 *  QMap<qlonglong, Edge* > * edges
	 *  \brief Edges connected to the Node
	 */
	QMap<qlonglong, Edge*> edges;

	/**
	 *  osg::Vec3f force
	 *  \brief Node force
	 */

	osg::Vec3f force;

	/**
	 *  osg::Vec3f velocity
	 *  \brief Size of node force in previous iteration
	 */
	osg::Vec3f velocity;

	/**
	 *  bool fixed
	 *  \brief node fixed state - fixed node is not moved by algorithm, toggled by used
	 */
	bool fixed;

	/**
	 *  bool frozen
	 *  \brief node frozen state - frozen node is not moved by algorithm still affects other nodes,
	 *  toggled automatically when moving a node, node movements are not interpolated
	 */
	bool frozen;

	/**
	 *  bool ignore
	 *  \brief node ignoring flag - node is ignored by algorithm, that means it does not
	 *  move and does not affect other nodes
	 */
	bool ignore;

};
}

#endif
