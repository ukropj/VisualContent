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
#include <osg/Stateset>
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
class Edge: public osg::Geode {
public:

	/**
	 * \fn public constructor Edge(qlonglong id, QString name, Graph* graph, osg::ref_ptr<Model::Node> srcNode, osg::ref_ptr<Model::Node> dstNode, Type* type, bool isOriented, int pos = 0, osg::ref_ptr<osg::Camera> camera = 0)
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
	Edge(qlonglong id, QString name, Graph* graph,
			osg::ref_ptr<Model::Node> srcNode,
			osg::ref_ptr<Model::Node> dstNode, Model::Type* type,
			bool isOriented, int pos = 0);

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
	 * \return osg::ref_ptr<Model::Node> starting Node of the Edge
	 */
	osg::ref_ptr<Model::Node> getSrcNode() const {
		return srcNode;
	}

	/**
	 * \fn inline public constant getDstNode
	 * \brief  Returns ending Node of the Edge
	 * \return osg::ref_ptr<Model::Node> ending Node of the Edge
	 */
	osg::ref_ptr<Model::Node> getDstNode() const {
		return dstNode;
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
	 * \fn inline public constant isOriented
	 * \brief Returns if the Edge is oriented or not
	 * \return bool true, if the Edge is oriented
	 */
	bool isOriented() const {
		return oriented;
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
	void updateGeometry(osg::Vec3 viewVec = osg::Vec3d(0, 0, 1));

	/**
	 *  \fn inline public constant  getEdgeColor
	 *  \brief Returns the color of the Edge
	 *  \return osg::Vec4 color of the Edge
	 */
	osg::Vec4 getEdgeColor() const {
		if (selected)
			return osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		else
			return edgeColor;
	}

	/**
	 *  \fn inline public  setEdgeColor(osg::Vec4 val)
	 *  \brief Sets the color of the Edge
	 *  \param     val   new color
	 */
	void setEdgeColor(osg::Vec4 color) {
		edgeColor = color;
		if (geometry != NULL) {
			osg::Vec4Array * colorArray =
					dynamic_cast<osg::Vec4Array *> (geometry->getColorArray());
			colorArray->pop_back();
			colorArray->push_back(color);
		}
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
	 *  \fn inline public  getGraph
	 *  \brief Returns the Graph to which is the Edge assigned
	 *  \return Graph * Edge's Graph
	 */
	Graph* getGraph() {
		return graph;
	}

	osg::ref_ptr<osgText::FadeText> getLabel() {
		return label;
	}

	static osg::ref_ptr<osg::StateSet> createStateSet(bool oriented); // XXX

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
	 *  osg::ref_ptr<Model::Node> srcNode
	 *  \brief Starting Node of the Edge
	 */
	osg::ref_ptr<Model::Node> srcNode;

	/**
	 *  osg::ref_ptr<Model::Node> dstNode
	 *  \brief Ending Node of the Edge
	 */
	osg::ref_ptr<Model::Node> dstNode;

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

	osg::ref_ptr<osg::Geometry> geometry;

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

	osg::ref_ptr<osg::Geometry> createGeometry();

	osg::ref_ptr<osgText::FadeText> createLabel(QString text);
};
}

#endif
