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

#include "Model/Node.h"

#define METASTRENGTH 1

namespace Model  {
class Type;
class Node;
class Edge;

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
	Graph(QString name, qlonglong ele_id_counter);

	/**
	 *  \fn public destructor  ~Graph
	 *  \brief Destroys the Graph object and performs delete on Type objects
	 */
	~Graph(void);

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
	QString setName(QString name);

	/**
	 *  \fn inline public  getName
	 *  \brief Returns the name of the Graph
	 *  \return QString name of the Graph
	 */
	QString getName() {
		return name;
	}

	/**
	 *  \fn inline public  getEleIdCounter
	 *  \brief Returns current value of element ID counter
	 *  \return qlonglong current value of element ID counter
	 */
	qlonglong getEleIdCounter() {
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
	osg::ref_ptr<Node> addNode(QString name, Type* type, osg::Vec3f position =
			osg::Vec3f(0, 0, 0));

	/**
	 *  \fn public  addEdge(QString name, osg::ref_ptr<Node> srcNode, osg::ref_ptr<Node> dstNode, Type* type, bool isOriented)
	 *  \brief Creates new Edge and adds it to the Graph
	 *  \param   name     name of the Edge
	 *  \param   srcNode    starting Node of the Edge
	 *  \param   dstNode     ending Node of the Edge
	 *  \param   type    Type of the Edge
	 *  \param   isOriented   true, if the Edge is oriented
	 *  \return osg::ref_ptr the added Edge
	 */
	Edge* addEdge(QString name, osg::ref_ptr<Node> srcNode,
			osg::ref_ptr<Node> dstNode, Type* type, bool isOriented);

	/**
	 *  \fn public  addType(QString name, QMap <QString, QString> *settings = 0)
	 *  \brief Creates new Type and adds it to the Graph
	 *  \param    name     name of the Type
	 *  \param   settings     settings of the Type
	 *  \return Type * the added Type
	 */
	Type* addType(QString name, QMap<QString, QString> *settings = 0); //implemented

	/**
	 *  \fn public  removeNode(osg::ref_ptr<Node> node)
	 *  \brief Removes a Node from the Graph
	 *  \param      node   the Node to be removed from the Graph
	 */
	void removeNode(osg::ref_ptr<Node> node);

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
	 *  \return QMap<qlonglong,osg::ref_ptr<Node> > * Nodes assigned to the Graph
	 */
	QMap<qlonglong, osg::ref_ptr<Node> >* getNodes() const {
		return nodes;
	}

	/**
	 *  \fn inline public constant  getEdges
	 *  \brief Returns QMap of the Edges assigned to the Graph
	 *  \return QMap<qlonglong,Edge* > * Edges assigned to the Graph
	 */
	QMap<qlonglong, Edge* >* getEdges() const {
		return edges;
	}

	/**
	 *  \fn inline public constant  getMetaNodes
	 *  \brief Returns QMap of the meta-Nodes assigned to the Graph
	 *  \return QMap<qlonglong,osg::ref_ptr<Node> > * meta-Nodes assigned to the Graph
	 */
	QMap<qlonglong, osg::ref_ptr<Node> >* getMetaNodes() const {
		return metaNodes;
	}

	/**
	 *  \fn inline public constant  getMetaEdges
	 *  \brief Returns meta-Edges assigned to the Graph
	 *  \return QMap<qlonglong,Edge* > * meta-Edges assigned to the Graph
	 */
	QMap<qlonglong, Edge* >* getMetaEdges() const {
		return metaEdges;
	}

	/**
	 *  \fn inline public constant  getTypes
	 *  \brief Returns QMap of the Types assigned to the Graph
	 *  \return QMap<qlonglong,Type*> * Tyeps assigned to the Graph
	 */
	QMap<qlonglong, Type*>* getTypes() const {
		return types;
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

	/**
	 *  \fn public  getNodeMetaType
	 *  \brief Returns MetaType for meta-Nodes
	 *  \return Type * MetaType for the Nodes
	 */
	Type* getNodeMetaType();

	/**
	 *  \fn public  getEdgeMetaType
	 *  \brief Returns MetaType for meta-Edges
	 *  \return Type * MetaType for the Edges
	 */
	Type* getEdgeMetaType();
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
	QMultiMap<QString, Type*>* typesByName;

	/**
	 *  QMap<qlonglong,osg::ref_ptr<Node> > newNodes
	 *  \brief New Nodes that have been added to the Graph but are not yet in database
	 */
	QMap<qlonglong, osg::ref_ptr<Node> > newNodes;

	/**
	 *  QMap<qlonglong,Type*> newTypes
	 *  \brief New Types that have been added to the Graph but are not yet in database
	 */
	QMap<qlonglong, Type*> newTypes;

	/**
	 *  QMap<qlonglong,Edge* > newEdges
	 *  \brief New Edges that have been added to the Graph but are not yet in database
	 */
	QMap<qlonglong, Edge* > newEdges;

	/**
	 *  QMap<qlonglong,osg::ref_ptr<Node> > * nodes
	 *  \brief Nodes in the Graph
	 */
	QMap<qlonglong, osg::ref_ptr<Node> >* nodes;

	/**
	 *  QMap<qlonglong,Edge* > * edges
	 *  \brief Edges in the Graph
	 */
	QMap<qlonglong, Edge* >* edges;

	/**
	 *  QMap<qlonglong,osg::ref_ptr<Node> > * metaNodes
	 *  \brief Meta-Nodes in the Graph
	 */
	QMap<qlonglong, osg::ref_ptr<Node> >* metaNodes;

	/**
	 *  QMap<qlonglong,Edge* > * metaEdges
	 *  \brief Meta-Edges in the Graph
	 */
	QMap<qlonglong, Edge* >* metaEdges;

	/**
	 *  QMap<qlonglong,Type*> * types
	 *  \brief Types in the Graph
	 */
	QMap<qlonglong, Type*>* types;

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
	 *  QMap<qlonglong,osg::ref_ptr<Node> > nodesByType
	 *  \brief Nodes in the Graph sorted by their Type
	 */
	QMultiMap<qlonglong, osg::ref_ptr<Node> > nodesByType;

	/**
	 *  QMap<qlonglong,Edge* > metaEdgesByType
	 *  \brief Meta-Edges in the Graph sorted by their Type
	 */
	QMultiMap<qlonglong, Edge* > metaEdgesByType;

	/**
	 *  QMap<qlonglong,osg::ref_ptr<Node> > metaNodesByType
	 *  \brief Meta-Nodes in the Graph sorted by their Type
	 */
	QMultiMap<qlonglong, osg::ref_ptr<Node> > metaNodesByType;
};
}

#endif

/* POZNAMKY

 QUERY na vyber stromovej hierarchie uzlov s urcenym ci je uzol meta na zaklade parenta (type_id) ktory je metauzlom

 WITH RECURSIVE q AS (
 SELECT node_id, type_id, "name", meta, CAST("name" As varchar(1000)) As fullname
 FROM nodes
 WHERE node_id = type_id
 UNION ALL
 SELECT n.node_id, n.type_id, n."name", (CASE WHEN n.meta OR q.meta THEN true ELSE false END) AS meta,
 CAST(q.fullname || '->' || n."name" As varchar(1000)) As fullname
 FROM nodes AS n
 INNER JOIN q ON
 n.type_id = q.node_id AND n.type_id != n.node_id
 )
 SELECT node_id, type_id, "name", meta, fullname
 FROM q
 ORDER BY fullname


 WITH RECURSIVE q AS (
 SELECT node_id, type_id, "name", meta, layout_id
 FROM nodes
 WHERE node_id = type_id
 UNION ALL
 SELECT n.node_id, n.type_id, n."name",
 (n.meta OR q.meta) AS meta,
 COALESCE(n.layout_id, q.layout_id) AS layout_id
 FROM nodes AS n
 INNER JOIN q ON
 n.type_id = q.node_id AND n.type_id != n.node_id
 )
 SELECT node_id, type_id, "name", meta, layout_id
 FROM q

 */

/*
 EXPLAIN ANALYZE SELECT * FROM crosstab(
 'SELECT graph_id, val_name, val FROM graph_settings ORDER BY 1,2',
 'SELECT val_name FROM graph_settings GROUP BY val_name'
 ) AS (graph_id bigint, layout_count text,"name" text,file_size text,path text)
 */
