/*!
 * Graph.h
 * Projekt 3DVisual
 */

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

#define METASTRENGTH 1

namespace Model  {
class Node;
class Edge;
class PseudoEdge;
class Type;

/**
 *  \class Graph
 *  \brief Object represents a graph structure.
 *
 *	Graph object is a representation of an graph. A graph is a set of Nodes connected via Edges. Each Node and/or Edge has its Type.
 *	A graph is layouted and displayed by the application. Each graph can have 1 or more GraphLayouts. A GraphLayout represents single layout of the Nodes and Edges. Each GraphLayout can have it's own meta-Edges and meta-Nodes
 * which can be used by the user to influence the layout of the graph.
 *
 *  \author Aurel Paulovic
 *  \date 29. 4. 2010
 */
class Graph {
public:

	/**
	 *  \fn public overloaded constructor  Graph(qlonglong graph_id, QString name, qlonglong layout_id_counter, qlonglong ele_id_counter, QSqlDatabase* conn)
	 *  \brief Creates new Graph Object
	 *  \param   graph_id     ID of the graph
	 *  \param   name     name of the graph
	 *  \param   layout_id_counter     current maximum graphLayout ID (for empty graph use 1)
	 *  \param   ele_id_counter     current maximum element (Node, Type, Edge) ID (for empty graph use 1)
	 *  \param   conn     connection to DB
	 */
	Graph(QString name = "graph", qlonglong ele_id_counter = 0);

	/**
	 *  \fn public destructor  ~Graph
	 *  \brief Destroys the Graph object and performs delete on Type objects
	 */
	~Graph();

	/**
	 *  \fn inline public static  getMetaStrength
	 *  \brief Returns the strength of meta-element for layout algorithm
	 *  \return float strength of the meta-element
	 */
	float static getMetaStrength() {
		return (float) METASTRENGTH;
	}

	/**
	 *  \fn public  setName(QString name)
	 *  \brief Sets new name
	 *  \param   name     new Name for the Graph
	 *  \return QString resultant name of the Graph
	 */
	QString setName(QString newName);

	/**
	 *  \fn inline public  getName
	 *  \brief Returns the name of the Graph
	 *  \return QString name of the Graph
	 */
	QString getName() const {
		return name;
	}

	/**
	 *  \fn inline public  getEleIdCounter
	 *  \brief Returns current value of element ID counter
	 *  \return qlonglong current value of element ID counter
	 */
	qlonglong getEleIdCounter() const {
		return ele_id_counter;
	}

	/**
	 *  \fn public  addNode(QString name, Type* type, osg::Vec3f position = osg::Vec3f(0,0,0))
	 *  \brief Creates new Node and adds it to the Graph
	 *  \param   name    name of the Node
	 *  \param   type   Type of the Node
	 *  \param   position     position of the Node
	 *  \return osg::ref_ptr the added Node
	 */
	Node* addNode(Type* type, QMap<QString, QString>* data = 0);

	/**
	 *  \fn public  addEdge(QString name, Node* srcNode, Node* dstNode, Type* type, bool isOriented)
	 *  \brief Creates new Edge and adds it to the Graph
	 *  \param   name     name of the Edge
	 *  \param   srcNode    starting Node of the Edge
	 *  \param   dstNode     ending Node of the Edge
	 *  \param   type    Type of the Edge
	 *  \param   isOriented   true, if the Edge is oriented
	 *  \return osg::ref_ptr the added Edge
	 */
	Edge* addEdge(Node* srcNode, Node* dstNode, Type* type, QMap<QString, QString>* data = 0);
	Edge* addEdge(qlonglong srcNodeId, qlonglong dstNodeId, Type* type, QMap<QString, QString>* data = 0);

	/**
	 *  \fn public  addType(QString name, QMap <QString, QString> *settings = 0)
	 *  \brief Creates new Type and adds it to the Graph
	 *  \param    name     name of the Type
	 *  \param   settings     settings of the Type
	 *  \return Type * the added Type
	 */
	Type* addType(QString name, QMap<QString, QString> *settings = 0); //implemented

	/**
	 *  \fn public  removeNode(Node* node)
	 *  \brief Removes a Node from the Graph
	 *  \param      node   the Node to be removed from the Graph
	 */
	void removeNode(Node* node);

	/**
	 *  \fn public  removeEdge(Edge* edge)
	 *  \brief Removes an Edge from the Graph
	 *  \param       edge the Edge to be removed from the Graph
	 */
	void removeEdge(Edge* edge);

	/**
	 *  \fn public  removeType(Type* type)
	 *  \brief Removes a Type from the Graph
	 *  \param  type the Type to be removed from the Graph
	 */
	void removeType(Type* type);

	/**
	 *  \fn inline public constant  getNodes
	 *  \brief Returns QMap of the Nodes assigned to the Graph
	 *  \return QMap<qlonglong,Node* > * Nodes assigned to the Graph
	 */
	QMap<qlonglong, Node*>* getNodes() {
		return &nodes;
	}

	/**
	 *  \fn inline public constant  getEdges
	 *  \brief Returns QMap of the Edges assigned to the Graph
	 *  \return QMap<qlonglong,Edge* > * Edges assigned to the Graph
	 */
	QMap<qlonglong, Edge*>* getEdges() {
		return &edges;
	}

	QMap<uint, PseudoEdge*>* getPseudoEdges() {
		return &pseudoEdges;
	}

	/**
	 *  \fn inline public constant  getTypes
	 *  \brief Returns QMap of the Types assigned to the Graph
	 *  \return QMap<qlonglong,Type*> * Tyeps assigned to the Graph
	 */
	QMap<qlonglong, Type*>* getTypes() {
		return &types;
	}

	/**
	 *  \fn inline public constant  isFrozen
	 *  \brief Returns true, if the Graph is frozen (for layout algorithm)
	 *  \return bool true, if the Graph is frozen
	 */
	bool isFrozen() const {
		return frozen;
	}

	/**
	 *  \fn inline public  setFrozen(bool val)
	 *  \brief Sets the frozen flag of the Graph to val
	 *  \param      val     true, if the graph shall be frozen
	 */
	void setFrozen(bool val) {
		frozen = val;
	}

	/**
	 *  \fn public  getTypesByName(QString name)
	 *  \brief Returns all Types assigned to the Graph with name
	 *  \param       name     name of the searched Type
	 *  \return QList<Type*> searched Types
	 */
	QList<Type*> getTypesByName(QString name);

	/**
	 *  \fn public constant  toString
	 *  \brief Returns human-readable string representing the Graph
	 *  \return QString
	 */
	QString toString() const;

private:

	/**
	 *  \fn inline private  incEleIdCounter
	 *  \brief Increments and returns the incremented value of element ID counter
	 *  \return qlonglong incremented value of element counter
	 */
	qlonglong incEleIdCounter() {
		return ++ele_id_counter;
	}

	/**
	 * \fn	private removeAllEdgesOfType(Type* type)
	 * \brief Removes all Edges that are of type
	 *  \param      type   Type
	 */
	void removeAllEdgesOfType(Type* type);

	/**
	 * \fn private removeAllNodesOfType(Type* type)
	 * \brief Removes all Nodes that are of type
	 *  \param      type     Type
	 */
	void removeAllNodesOfType(Type* type);

	/**
	 *  \fn private  getMaxEleIdFromElements
	 *  \brief Returns the maximum value of IDs of all elements of the Graph
	 *  \return qlonglong returns maximum ID value of elements
	 */
	qlonglong getMaxEleIdFromElements();

	/**
	 *  QString name
	 *  \brief Name of the Graph
	 */
	QString name;

	/**
	 *  qlonglong ele_id_counter
	 *  \brief Element ID counter
	 */
	qlonglong ele_id_counter;

	/**
	 *  QMultiMap<QString,Type*> * typesByName
	 *  \brief Types sorted by their name attribute
	 */
	QMultiMap<QString, Type*> typesByName;

	/**
	 *  QMap<qlonglong,Node* > * nodes
	 *  \brief Nodes in the Graph
	 */
	QMap<qlonglong, Node* > nodes;

	/**
	 *  QMap<qlonglong,Edge* > * edges
	 *  \brief Edges in the Graph
	 */
	QMap<qlonglong, Edge* > edges;
	QMap<uint, PseudoEdge* > pseudoEdges;

	/**
	 *  QMap<qlonglong,Type*> * types
	 *  \brief Types in the Graph
	 */
	QMap<qlonglong, Type*> types;

	/**
	 *  bool frozen
	 *  \brief Flag if the Graph is frozen or not (used by layout algorithm)
	 */
	bool frozen;

	/**
	 *  QMap<qlonglong,Edge* > edgesByType
	 *  \brief Edges in the Graph sorted by their Type
	 */
	QMultiMap<qlonglong, Edge* > edgesByType;

	/**
	 *  QMap<qlonglong,Node* > nodesByType
	 *  \brief Nodes in the Graph sorted by their Type
	 */
	QMultiMap<qlonglong, Node* > nodesByType;

};
}

#endif
