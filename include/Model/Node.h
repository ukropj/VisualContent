#ifndef MODEL_NODE_DEF
#define MODEL_NODE_DEF 1

#include <osg/Vec3f>
#include <vector>
#include <string>
#include <QMap>
#include <QSet>
#include <QString>
#include <QDebug>

namespace Vwr {
class OsgNode;
}

namespace Model {
class Edge;
class Cluster;
class Type;
class Graph;

/**
 *  \class Node
 *  \brief Node object represents a single node in a Graph
 *  \author Aurel Paulovic, Michal Paprcka
 *  \date 29. 4. 2010
 */
class Node {
public:

	Node(qlonglong id, Type* type, QMap<QString, QString>* data, Graph* graph);
	~Node();

	void addEdge(Edge* edge);
	void removeEdge(Edge* edge);
	void removeAllEdges();
	Edge* getEdgeTo(const Node* otherNode, bool ignoreClusters = false) const;
	virtual QSet<Node*> getIncidentNodes(bool ignoreClusters = false) const;

	qlonglong getId() const {return id;}
	Graph* getGraph() const {return graph;}
	Type* getType() const {return type;}
	QString data(QString key) const;
	float getWeight() const;
	QMap<qlonglong, Edge*>* getEdges() {return &edges;}

	osg::Vec3f getPosition() const {return osg::Vec3f(position);}
	void setPosition(osg::Vec3f pos) {position.set(pos);}
	void setForce(osg::Vec3f v) {force = v;}
	osg::Vec3f getForce() const {return force;}
	void addForce(osg::Vec3f v) {force += v;}
	void resetForce() {force = osg::Vec3f(0, 0, 0);}
	void setVelocity(osg::Vec3f v) {velocity = v;}
	void resetVelocity() {velocity = osg::Vec3f(0, 0, 0);}
	osg::Vec3f getVelocity() const {return velocity;}

	void setFixed(bool flag) {fixed = flag;}
	bool isFixed() const {return fixed;}
	void setFrozen(bool flag) {frozen = flag;}
	bool isFrozen() const {return frozen;}
	void setIgnored(bool flag);
	bool isIgnored() const {return ignore;}

	bool equals(const Node* node) const;
	QString toString() const;

	void setOsgNode(Vwr::OsgNode* osgNode);
	Vwr::OsgNode* getOsgNode() const {return osgNode;}

	virtual void setParent(Cluster* parent);
	Cluster* getParent() const {return parent;}

	bool canBeClustered() const;

	Node* getTopCluster() const;
	virtual bool isCluster() const {return false;}

protected:
	Cluster* parent; // immediate parent cluster
	Graph* graph;
	Type* type;
	QMap<QString, QString>* nodeData;
	float weight;

private:
	qlonglong id;
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
