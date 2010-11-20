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

	/**
	 *  \fn inline public constant  getId
	 *  \brief Returns ID of the Node
	 *  \return qlonglong ID of the Node
	 */
	qlonglong getId() const {
		return id;
	}

	/**
	 *  \fn inline public  getGraph
	 *  \brief Returns Graph to which the Nodes belongs
	 *  \return Graph * Graph
	 */
	Graph* getGraph() {
		return graph;
	}

	/**
	 *  \fn inline public constant  getName
	 *  \brief Returns name of the Node
	 *  \return QString name of the Node
	 */
	QString getName() const {
		return name;
	}

	/**
	 *  \fn inline public  setName(QString val)
	 *  \brief Sets new name to the Node
	 *  \param   val    new name
	 */
	void setName(QString val) {
		name = val;
	}

	/**
	 *  \fn inline public constant  getType
	 *  \brief Returns Type of the Node
	 *  \return Type * Type of the Node
	 */
	Type* getType() const {
		return type;
	}

	/**
	 *  \fn inline public  setType(Type* val)
	 *  \brief Sets new Type of the Node
	 *  \param   val  new Type
	 */
	void setType(Type* val) {
		type = val;
	}

	/**
	 *  \fn inline public constant  getTargetPosition
	 *  \brief Returns node target position in space
	 *  \return osg::Vec3f node position
	 */
	osg::Vec3f getPosition() const {
		return osg::Vec3f(position);
	}

	/**
	 *  \fn inline public  setTargetPosition(osg::Vec3f val)
	 *  \brief Sets node target position in space
	 *  \param      val   new position
	 */
	void setPosition(osg::Vec3f pos) {
		position.set(pos);
	}

	/**
	 *  \fn inline public constant  getEdges
	 *  \brief Returns all Edges connected to the Node
	 *  \return QMap<qlonglong,Edge* > * Edges connected to the Node
	 */
	QMap<qlonglong, Edge*>* getEdges() {
		return &edges;
	}

	/**
	 *  \fn public  addEdge(Edge* edge)
	 *  \brief Adds new Edge to the Node
	 *  \param    edge   new Edge
	 */
	void addEdge(Edge* edge);
	void removeEdge(Edge* edge);

	/**
	 *  \fn inline public  setForce(osg::Vec3f v)
	 *  \brief Sets force of node
	 *  \param      v  Force vector
	 */
	void setForce(osg::Vec3f v) {
		force = v;
	}

	/**
	 *  \fn inline public constant  getForce
	 *  \brief Gets force of node
	 *  \return osg::Vec3f Force vector
	 */
	osg::Vec3f getForce() const {
		return force;
	}

	/**
	 *  \fn inline public  addForce(bool fixed)
	 *  \brief Adds force V to node force
	 *  \param       v  Force V
	 */
	void addForce(osg::Vec3f v) {
		force += v;
	}

	/**
	 *  \fn inline public  resetForce
	 *  \brief Sets nodes force to zero value.
	 */
	void resetForce() {
		force = osg::Vec3f(0, 0, 0);
	}

	/**
	 *  \fn inline public  setFixed(bool fixed)
	 *  \brief Sets node fixed state
	 *  \param     fixed     fixed state
	 */
	bool setFixed(bool flag) {
		if (flag == isFixed())
			return false;
		fixed = flag;
	}

	/**
	 *  \fn inline public constant  isFixed
	 *  \brief Returns if the Node is fixed
	 *  \return bool true, if the Node is fixed
	 */
	bool isFixed() const {
		return fixed;
	}

	void setFrozen(bool flag) {
		frozen = flag;
	}

	/**
	 *  \fn inline public constant  isFixed
	 *  \brief Returns if the Node is fixed
	 *  \return bool true, if the Node is fixed
	 */
	bool isFrozen() const {
		return frozen;
	}

	/**
	 *  \fn inline public  setIgnored(bool b)
	 *  \brief Sets ignoring by layout algorithm.
	 *  \param       b  True, if node is ignored.
	 */
	void setIgnored(bool b) {
		ignore = b;
	}

	/**
	 *  \fn inline public constant  isIgnored
	 *  \brief Return if the Node is ignored
	 *  \return bool true, if the Node is ignored
	 */
	bool isIgnored() const {
		return ignore;
	}

	/**
	 *  \fn inline public  setVelocity(osg::Vec3f v)
	 *  \brief Sets node force for next iteration
	 *  \param    v  Force in actual iteration
	 */
	void setVelocity(osg::Vec3f v) {
		velocity = v;
	}

	/**
	 *  \fn inline public  resetVelocity
	 *  \brief Reset node force for next iteration
	 */
	void resetVelocity() {
		velocity = osg::Vec3f(0, 0, 0);
	}

	/**
	 *  \fn inline public constant  getVelocity
	 *  \brief Sets node force for next iteration.
	 *  \return osg::Vec3f Node force
	 */
	osg::Vec3f getVelocity() const {
		return velocity;
	}

	/**
	 *  \fn public  equals(Node* node)
	 *  \brief Checks if the node and this object is the same object
	 *  \param   node     Node to compare
	 *  \return bool true, if this object and node are the same object
	 */
	bool equals(Node* node);

	/**
	 *  \fn inline public constant  toString
	 *  \brief Returns human-readable string representing the Node
	 *  \return QString
	 */
	QString toString() const {
		QString str;
		QTextStream(&str) << "N" << id << " " << name << "["
				<< position.x() << "," << position.y() << ","
				<< position.z() << "]" << (isFixed() ? "fixed" : "");
		return str;
	}

	QString getLabel() const {
		return labelText;
	}

	void setOsgNode(osg::ref_ptr<Vwr::OsgNode> osgNode) {
		this->osgNode = osgNode;
	}

	osg::ref_ptr<Vwr::OsgNode> getOsgNode() {
		return osgNode;
	}

	float getRadius() const;

private:

	osg::ref_ptr<Vwr::OsgNode> osgNode;
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
