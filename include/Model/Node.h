/*!
 * Node.h
 * Projekt 3DVisual
 */
#ifndef DATA_NODE_DEF
#define DATA_NODE_DEF 1

#include <vector>
#include <string>
#include <QMap>
#include <QString>
#include <QTextStream>

#include <osg/Vec3f>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/CullFace>
#include <osgText/Text>

#include "Model/Edge.h"

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
class Node: public osg::Geode {
public:

	/**
	 *  \fn public constructor  Node(qlonglong id, QString name, Type* type, Graph* graph, osg::Vec3f position)
	 *  \brief Creates new Node object
	 *  \param  id     ID of the Node
	 *  \param  name     name of the Node
	 *  \param  type    Type of the Node
	 *  \param  graph   Graph to which the Node belongs
	 *  \param  position    Node position in space
	 */
	Node(qlonglong id, QString name, Type* type, Graph* graph,
			osg::Vec3f position);

	/**
	 *  \fn public virtual destructor  ~Node
	 *  \brief Destroys the Node object
	 */
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
	osg::Vec3f getTargetPosition() const {
		return osg::Vec3(targetPosition);
	}

	/**
	 *  \fn inline public  setTargetPosition(osg::Vec3f val)
	 *  \brief Sets node target position in space
	 *  \param      val   new position
	 */
	void setTargetPosition(osg::Vec3f val) {
		targetPosition.set(val);
	}

	/**
	 *  \fn public  getCurrentPosition(bool calculateNew = false, float interpolationSpeed = 1.0f)
	 *  \brief Returns node actual position
	 *  \param      calculateNew    If true, new position will be calculated through interpolation
	 *  \param      float   interpolation speed
	 *  \return osg::Vec3f actual position
	 *
	 *
	 */
	osg::Vec3f getCurrentPosition(bool calculateNew = false,
			float interpolationSpeed = 1.0f);

	/**
	 *  \fn inline public  setCurrentPosition(osg::Vec3f val)
	 *  \brief Sets current node position
	 *  \param   val  current node position
	 */
	void setCurrentPosition(osg::Vec3f val) {
		currentPosition.set(val);
	}

	/**
	 *	\fn public removeAllEdges
	 *	\brief Removes all Edges which connect to the Node
	 *
	 *	It unlinks all edges connected to the Node and removes them from the Graph (an Edge can't exist without both it's Nodes)
	 */
	void removeAllEdges();

	/**
	 *  \fn inline public constant  getEdges
	 *  \brief Returns all Edges connected to the Node
	 *  \return QMap<qlonglong,osg::ref_ptr<Edge> > * Edges connected to the Node
	 */
	QMap<qlonglong, osg::ref_ptr<Edge> > * getEdges() const {
		return edges;
	}

	/**
	 *  \fn inline public  setEdges(QMap<qlonglong, osg::ref_ptr<Edge> > *val)
	 *  \brief Sets (overrides) new Edges which are connected to the Node
	 *  \param   val   new Edges
	 */
	void setEdges(QMap<qlonglong, osg::ref_ptr<Edge> > *val) {
		edges = val;
	}

	/**
	 *  \fn public  addEdge(osg::ref_ptr<Edge> edge)
	 *  \brief Adds new Edge to the Node
	 *  \param    edge   new Edge
	 */
	void addEdge(osg::ref_ptr<Edge> edge);
	void removeEdge(osg::ref_ptr<Edge> edge);

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
	void setFixed(bool fixed) {
		this->fixed = fixed;

		if (fixed && !this->containsDrawable(square))
			this->addDrawable(square);
		else if (!fixed)
			this->removeDrawable(square);
	}

	/**
	 *  \fn inline public constant  isFixed
	 *  \brief Returns if the Node is fixed
	 *  \return bool true, if the Node is fixed
	 */
	bool isFixed() const {
		return fixed;
	}

	/**
	 *  \fn inline public  setSelected(bool selected)
	 *  \brief Sets node picked state
	 *  \param     selected     picked state
	 */
	void setSelected(bool selected) {
		if (selected)
			setDrawableColor(0, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		else
			setDrawableColor(0, color);

		this->selected = selected;
	}

	/**
	 *  \fn inline public constant  isSelected
	 *  \brief Returns if the Node is selected
	 *  \return bool true, if the Node is selected
	 */
	bool isSelected() const {
		return selected;
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
		velocity = osg::Vec3(0, 0, 0);
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
				<< targetPosition.x() << "," << targetPosition.y() << ","
				<< targetPosition.z() << "]" << (isFixed() ? "fixed" : "");
		return str;
	}

	/**
	 *  \fn inline public  setColor(osg::Vec4 color)
	 *  \brief Sets default node color
	 *  \param     color   default color
	 */
	void setColor(osg::Vec4 color) {
		this->color = color;

		if (!selected)
			setDrawableColor(0, color);
	}

	/**
	 *  \fn inline public constant  getColor
	 *  \brief Returns color of the Node
	 *  \return osg::Vec4 color of the Node
	 */
	osg::Vec4 getColor() const {
		return color;
	}

	/**
	 *  \fn public  showLabel(bool visible)
	 *  \brief If true, node name will be shown.
	 *  \param     visible     node name shown
	 */
	void showLabel(bool visible);

	/**
	 *  \fn inline public constant  isUsingInterpolation
	 *  \brief Returns if the Node is using interpolation or not
	 *  \return bool true, if the Node is using interpolation
	 */
	bool isUsingInterpolation() const {
		return usingInterpolation;
	}

	/**
	 *  \fn inline public  setUsingInterpolation(bool val)
	 *  \brief Sets if the Node is using interpolation or not
	 *  \param      val
	 */
	void setUsingInterpolation(bool val) {
		usingInterpolation = val;
	}

	/**
	 *  \fn public  reloadConfig
	 *  \brief Reloads node configuration
	 */
	void reloadConfig();

private:

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
	osg::Vec3f targetPosition;

	/**
	 *  osg::Vec3f currentPosition
	 *  \brief node current position
	 */
	osg::Vec3f currentPosition;

	/**
	 *  QMap<qlonglong, osg::ref_ptr<Edge> > * edges
	 *  \brief Edges connected to the Node
	 */
	QMap<qlonglong, osg::ref_ptr<Edge> > * edges;

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
	 *  \brief node fixed state
	 */
	bool fixed;

	/**
	 *  bool ignore
	 *  \brief node ignoring flag
	 */
	bool ignore;

	/**
	 *  bool selected
	 *  \brief node picked state
	 */
	bool selected;

	/**
	 *  bool usingInterpolation
	 *  \brief node interpolation usage
	 */
	bool usingInterpolation;

	/**
	 *  \fn private static  createNode(const float & scale, osg::StateSet* bbState)
	 *  \brief Creates node drawable
	 *  \param      scale    node scale
	 *  \param  bbState    node stateset
	 *  \return osg::ref_ptr node drawable
	 */
	static osg::ref_ptr<osg::Drawable> createNode(const float & scale,
			osg::StateSet* bbState);

	/**
	 *  \fn private static  createStateSet(Type * type = 0)
	 *  \brief Creates node stateset
	 *  \param   type     node type
	 *  \return osg::ref_ptr node stateset
	 */
	static osg::ref_ptr<osg::StateSet> createStateSet(Type * type = 0);

	/**
	 *  \fn private static  createLabel(const float & scale, QString name)
	 *  \brief Creates node label from name
	 *  \param      scale     label scale
	 *  \param       name     label text
	 *  \return osg::ref_ptr node label
	 */
	static osg::ref_ptr<osg::Drawable> createLabel(const float & scale,
			QString name);

	/**
	 *  \fn private static  createSquare
	 *  \brief Creates square around node
	 *  \param  scale   square scale
	 *  \param  bbState     square stateset
	 *  \return osg::ref_ptr square drawable
	 */
	static osg::ref_ptr<osg::Drawable> createSquare(const float & scale,
			osg::StateSet* bbState);

	/**
	 *  osg::Vec4 color
	 *  \brief Color of the Node
	 */
	osg::Vec4 color;

	/**
	 *  \fn private  setDrawableColor(int pos, osg::Vec4 color)
	 *  \brief Sets drawble color
	 *  \param     pos     drawable position
	 *  \param     color     drawable color
	 */
	void setDrawableColor(int pos, osg::Vec4 color);

	/**
	 *  QString labelText
	 *  \brief Text show in the label
	 */
	QString labelText;

	/**
	 *  osg::ref_ptr label
	 *  \brief Label drawable
	 */
	osg::ref_ptr<osg::Drawable> label;

	/**
	 *  osg::ref_ptr square
	 *  \brief Square drawable - esed to mark fixed nodes
	 */
	osg::ref_ptr<osg::Drawable> square;
};
}

#endif
