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

	Edge(qlonglong id, QString name, Graph* graph,
			Model::Node* srcNode,
			Model::Node* dstNode,
			Type* type,
			bool isOriented);

	~Edge(void);

	/**
	 *  \fn inline public  getId
	 *  \brief Returns ID of the Edge
	 *  \return qlonglong ID of the Edge
	 */
	qlonglong getId() const {
		return id;
	}

	/**
	 *  \fn inline public  getName
	 *  \brief Returns the name of the Edge
	 *  \return QString name of the Edge
	 */
	QString getName() const {
		return name;
	}

	/**
	 *  \fn inline public  setName(QString val)
	 *  \brief Sets new name
	 *  \param   name   new Name for the Edge
	 *  \return QString resultant name of the Edge
	 */
	void setName(QString val) {
		name = val;
	}

	/**
	 * \fn inline public constant getSrcModel::Node
	 * \brief  Returns the starting Model::Node of the Edge
	 * \return osg::ref_ptr<Model::Model::Node> starting Model::Node of the Edge
	 */
	Model::Node* getSrcNode() const {
		return srcNode;
	}

	/**
	 * \fn inline public constant getDstModel::Node
	 * \brief  Returns ending Model::Node of the Edge
	 * \return osg::ref_ptr<Model::Model::Node> ending Model::Node of the Edge
	 */
	Model::Node* getDstNode() const {
		return dstNode;
	}

	/**
	 * Retruns the other node of this edge.
	 */
	Model::Node* getOtherNode(Model::Node* node) const;

	/**
	 * \fn inline public constant getType
	 * \brief  Returns the Type of the Edge
	 * \return Type * Type of the Edge
	 */
	Type* getType() const {
		return type;
	}

	/**
	 * \fn inline public constant isOriented
	 * \brief Returns if the Edge is oriented or not
	 * \return bool true, if the Edge is oriented
	 */
	bool isOriented() const {
		return oriented;
	}

	/**
	 * \fn inline public constant toString
	 * \brief  Returns human-readable string representation of the Edge
	 * \return QString
	 */
	QString toString() const {
		QString str;
		QTextStream(&str) << "edge id:" << id << " name:" << name;
		return str;
	}

	/**
	 *  \fn inline public  getGraph
	 *  \brief Returns the Graph to which is the Edge assigned
	 *  \return Graph * Edge's Graph
	 */
	Graph* getGraph() {
		return graph;
	}

	float getMinNodeDistance();

private:
	/**
	 *  Graph * graph
	 *  \brief Graph to which the Edge belongs
	 */
	Graph* graph;

	/**
	 *  qlonglong id
	 *  \brief ID of the Edge
	 */
	qlonglong id;

	//! Meno hrany.
	/**
	 *  QString name
	 *  \brief Name of the Edge
	 */
	QString name;

	/**
	 *  osg::ref_ptr<Model::Model::Node> srcModel::Node
	 *  \brief Starting Model::Node of the Edge
	 */
	Model::Node* srcNode;

	/**
	 *  osg::ref_ptr<Model::Model::Node> dstModel::Node
	 *  \brief Ending Model::Node of the Edge
	 */
	Model::Node* dstNode;

	/**
	 *  Type * type
	 *  \brief Type of the Edge
	 */
	Type* type;

	/**
	 *  bool oriented
	 *  \brief True, if the Edge is oriented
	 */
	bool oriented;

	/**
	 *  float length
	 *  \brief Length of the Edge
	 */
	float length;

	/**
	 *  bool selected
	 *  \brief True, if edge is picked
	 */
};
}

#endif
