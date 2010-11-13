/**
 *  NodeGroup.h
 *  Projekt 3DVisual
 */
#ifndef VIEWER_NODEWRAPPER_DEF
#define VIEWER_NODEWRAPPER_DEF 1

#include <QMap>
#include <QSet>
#include <QLinkedList>

#include <osg/ref_ptr>
#include <osg/AutoTransform>
#include <osg/Group>

namespace Model {
class Node;
class Edge;
}

namespace Vwr {
class SceneGraph;
class OsgEdge;
class OsgNode;

/**
 *  \class SceneElements 
 *  \brief
 *  \author Michal Paprcka
 *  \date 29. 4. 2010
 */
class SceneElements {
public:

	/**
	 *  \fn public constructor  NodeGroup(QMap<qlonglong, Model::Node* > *nodes)
	 *  \brief Creates node group
	 *  \param  nodes    nodes to wrap
	 */
	SceneElements(QMap<qlonglong, Model::Node* > *nodes,
			SceneGraph* sceneGraph);

	/**
	 *  \fn public destructor  ~NodeGroup
	 *  \brief destructor
	 */
	~SceneElements(void);

	void updateNodeCoords(float interpolationSpeed);

	void updateEdgeCoords();

	/**
	 *  \fn inline public  getGroup
	 *  \brief Returns wrapped nodes group
	 *  \return osg::ref_ptr nodes group
	 */
	osg::ref_ptr<osg::Group> getElementsGroup() {
		return group;
	}

	QMap<qlonglong, osg::ref_ptr<OsgNode> >* getNodes() {
		return &nodes;
	}

	QMap<qlonglong, osg::ref_ptr<OsgEdge> >* getEdges() {
		return &edges;
	}

private:
	SceneGraph* sceneGraph;

	/**
	 *  QMap<qlonglong,osg::ref_ptr<OsgNode> > nodes
	 *  \brief wrapped nodes
	 */
	QMap<qlonglong, osg::ref_ptr<OsgNode> > nodes;
	QMap<qlonglong, osg::ref_ptr<OsgEdge> > edges;

	/**
	 *  osg::ref_ptr group
	 *  \brief nodes group
	 */
	osg::ref_ptr<osg::Group> group;

	/**
	 *  \fn private  initNodes
	 *  \brief inits nodes
	 */
	void initNodes();

	/**
	 *  \fn private  wrapChild(Model::Node* node, float graphScale)
	 *  \brief Wraps node as a transform
	 *  \param      node    node to wrap
	 *  \param      graphScale     graph scale
	 *  \return osg::ref_ptr node transform
	 */
	osg::ref_ptr<osg::AutoTransform> wrapNode(Model::Node* node);

	osg::ref_ptr<osg::Group> wrapEdge(Model::Edge* edge);

	/**
	 *  \fn private  getNodeGroup(Model::Node* node, osg::ref_ptr<Model::Edge> parentEdge, float graphScale)
	 *  \brief Recursively traverses all node and its children and creates a group from them
	 *  \param       node    node to group
	 *  \param       parentEdge    Edge with which was this node connected to its parent
	 *  \param       graphScale     graph scale
	 *  \return osg::ref_ptr nodes group
	 */
	osg::ref_ptr<osg::Group> getNodeGroup(Model::Node* node,
			Model::Edge* parentEdge);

	osg::ref_ptr<osg::Group> getNodeGroup(Model::Node* node);
};
}

#endif
