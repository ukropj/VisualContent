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

#include <osg/PrimitiveSet>
#include <osgText/Text>
#include <osgText/FadeText>

#include "Model/Node.h"

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
class Edge: public osg::DrawArrays {
public:

	/**
	 * \fn public constructor Edge(qlonglong id, QString name, Graph* graph, osg::ref_ptr<Node> srcNode, osg::ref_ptr<Node> dstNode, Type* type, bool isOriented, int pos = 0, osg::ref_ptr<osg::Camera> camera = 0)
	 * \brief  Creates new Edge object connecting two Nodes
	 *
	 * \param id	ID of the Edge
	 * \param name 		name of the Edge
	 * \param graph 	Graph to which the Edge belongs
	 * \param srcNode 		starting Node
	 * \param dstNode 	ending Node
	 * \param type 		Type of the Edge.
	 * \param isOriented 		true if the Edge is oriented.
	 * \param pos int		first coordinate in Drawable coordinates array
	 * \param camera 	current camera used in viewer
	 */
	Edge(qlonglong id, QString name, Graph* graph, osg::ref_ptr<Node> srcNode,
			osg::ref_ptr<Node> dstNode, Model::Type* type, bool isOriented,
			int pos = 0);

	/**
	 *  \fn public destructor  ~Edge
	 *  \brief Destroys the Edge object
	 */
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
	 * \fn inline public constant getSrcNode
	 * \brief  Returns the starting Node of the Edge
	 * \return osg::ref_ptr<Node> starting Node of the Edge
	 */
	osg::ref_ptr<Node> getSrcNode() const {
		return srcNode;
	}

	/**
	 * \fn inline public constant setSrcNode
	 * \brief  Sets new starting Node of the Edge
	 *
	 * OBSOLETE - DO NOT USE IT
	 *
	 * \param val new starting Node of the Edge
	 */
	void setSrcNode(osg::ref_ptr<Node> val) {
		srcNode = val;
	}

	/**
	 * \fn inline public constant getDstNode
	 * \brief  Returns ending Node of the Edge
	 * \return osg::ref_ptr<Node> ending Node of the Edge
	 */
	osg::ref_ptr<Node> getDstNode() const {
		return dstNode;
	}

	/**
	 * \fn inline public setDstNode(osg::ref_ptr<Node> val)
	 * \brief Sets new ending Node of the Edge
	 *
	 * OBSOLETE - DO NOT USE IT
	 *
	 * \param val new ending Node of the Edge
	 */
	void setDstNode(osg::ref_ptr<Node> val) {
		dstNode = val;
	}

	/**
	 * \fn inline public constant getType
	 * \brief  Returns the Type of the Edge
	 * \return Type * Type of the Edge
	 */
	Model::Type* getType() const {
		return type;
	}

	/**
	 * \fn void setType(Type* val)
	 * \brief  Sets new Type of the Edge
	 *
	 * OBSOLETE - DO NOT USE IT
	 *
	 * \param  val new Type of the Edge
	 */
	void setType(Model::Type* val) {
		type = val;
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
	 * \fn inline public setOriented(bool val)
	 * \brief  Sets if the Edge is oriented
	 * \param val 		true, if the Edge is oriented
	 */
	void setOriented(bool val) {
		oriented = val;
	}

	/**
	 * \fn public linkNodes(QMap<qlonglong, osg::ref_ptr<Edge> > *edges)
	 * \brief  Links the Edge to it's Nodes and adds itself to the edges
	 * \param  edges
	 */
	void linkNodes(QMap<qlonglong, osg::ref_ptr<Edge> > *edges);

	/**
	 * \fn public unlinkNodes
	 * \brief Unlinks the Edge from the Nodes
	 */
	void unlinkNodes();

	/**
	 * \fn public unlinkNodesAndRemoveFromGraph
	 * \brief Unlinks the Edge from the Nodes and removes the Edge from it's Graph
	 */
	void unlinkNodesAndRemoveFromGraph();

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
	 *  \fn inline public constant  getCooridnates
	 *  \brief Returns coordinates of the Edge
	 *  \return osg::ref_ptr coordinates of the Edge
	 */
	osg::ref_ptr<osg::Vec3Array> getCooridnates() const {
		return coordinates;
	}

	/**
	 *  \fn inline public constant  getLength
	 *  \brief Returns length of the Edge
	 *  \return float length of the Edge
	 */
	float getLength() const {
		return length;
	}

	/**
	 *  \fn public  updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstNode)
	 *  \brief Updates coordinates of the Edge
	 *  \param    srcPos    new coordinates of the starting Node
	 *  \param   dstNode    new coordinates of the ending Node
	 */
	void updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstPos,
			osg::Vec3 viewVec = osg::Vec3d(0, 0, 1));

	/**
	 *  \fn inline public constant  getEdgeColor
	 *  \brief Returns the color of the Edge
	 *  \return osg::Vec4 color of the Edge
	 */
	osg::Vec4 getEdgeColor() const {
		if (selected)
			return osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		else
			return edgeColor;
	}

	/**
	 *  \fn inline public  setEdgeColor(osg::Vec4 val)
	 *  \brief Sets the color of the Edge
	 *  \param     val   new color
	 */
	void setEdgeColor(osg::Vec4 val) {
		edgeColor = val;
	}

	/**
	 *  \fn inline public constant  getEdgeTexCoords
	 *  \brief Returns Texture coordinates array.
	 *  \return osg::ref_ptr<osg::Vec2Array>
	 */
	osg::ref_ptr<osg::Vec2Array> getEdgeTexCoords() const {
		return edgeTexCoords;
	}

	/**
	 *  \fn inline public constant  isSelected
	 *  \brief Returns selected flag
	 *  \return bool true, if the Edge is selected
	 */
	bool isSelected() const {
		return selected;
	}

	/**
	 *  \fn inline public  setSelected(bool val)
	 *  \brief Sets the selected flag of the Edge
	 *  \param       val   true, if the Edge is selected
	 */
	void setSelected(bool val) {
		selected = val;
	}

	/**
	 *  \fn public  createLabel(QString name)
	 *  \brief Creates new label
	 *  \param      name     new label
	 *  \return osg::ref_ptr added label
	 */
	osg::ref_ptr<osg::Drawable> createLabel(QString name);

	/**
	 *  \fn inline public  getGraph
	 *  \brief Returns the Graph to which is the Edge assigned
	 *  \return Graph * Edge's Graph
	 */
	Graph* getGraph() {
		return graph;
	}

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
	 *  osg::ref_ptr<Node> srcNode
	 *  \brief Starting Node of the Edge
	 */
	osg::ref_ptr<Node> srcNode;

	/**
	 *  osg::ref_ptr<Node> dstNode
	 *  \brief Ending Node of the Edge
	 */
	osg::ref_ptr<Node> dstNode;

	/**
	 *  Type * type
	 *  \brief Type of the Edge
	 */
	Model::Type* type;

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
	bool selected;

	/**
	 *  osg::ref_ptr coordinates
	 *  \brief Coordinates of the Edge
	 */
	osg::ref_ptr<osg::Vec3Array> coordinates;

	/**
	 *  osg::ref_ptr<osg::Vec2Array> edgeTexCoords
	 *  \brief Texture coordinates array.
	 */
	osg::ref_ptr<osg::Vec2Array> edgeTexCoords;

	/**
	 *  osg::ref_ptr label
	 *  \brief Label of the Edge
	 */
	osg::ref_ptr<osgText::FadeText> label;

	/**
	 *  osg::Vec4 edgeColor
	 *  \brief Color of the Edge
	 */
	osg::Vec4 edgeColor;

};
}

#endif
