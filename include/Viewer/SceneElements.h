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
#include <osg/Geometry>

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
	SceneElements(QMap<qlonglong, Model::Node* > *nodes, QMap<qlonglong, Model::Edge* > *edges,
			SceneGraph* sceneGraph);

	/**
	 *  \fn public destructor  ~NodeGroup
	 *  \brief destructor
	 */
	~SceneElements(void);

	void updateNodes(float interpolationSpeed);

	void updateEdges();

	/**
	 *  \fn inline public  getGroup
	 *  \brief Returns wrapped nodes group
	 *  \return osg::ref_ptr nodes group
	 */
	osg::ref_ptr<osg::Group> getElementsGroup() {
		return elementsGroup;
	}

	QMap<qlonglong, osg::ref_ptr<OsgNode> >* getNodes() {
		return &nodes;
	}

//	QMap<qlonglong, osg::ref_ptr<OsgEdge> >* getEdges() {
//		return &edges;
//	}

private:
	SceneGraph* sceneGraph;

	/**
	 *  QMap<qlonglong,osg::ref_ptr<OsgNode> > nodes
	 *  \brief wrapped nodes
	 */
	QMap<qlonglong, osg::ref_ptr<OsgNode> > nodes;
	QMap<qlonglong, OsgEdge*> edges;

	/**
	 *  osg::ref_ptr group
	 *  \brief nodes group
	 */
	osg::ref_ptr<osg::Group> elementsGroup;

	/**
	 *  \fn private  initNodes
	 *  \brief inits nodes
	 */
	osg::ref_ptr<osg::Group> initNodes(QMap<qlonglong, Model::Node* > *nodes);
	osg::ref_ptr<osg::Group> initEdges(QMap<qlonglong, Model::Edge* > *edges);


	osg::ref_ptr<osg::AutoTransform> wrapNode(Model::Node* node);

	osg::ref_ptr<osg::Group> wrapEdge(Model::Edge* edge);

	osg::ref_ptr<osg::Group> getNodeGroup1(Model::Node* node,
			Model::Edge* parentEdge);

	osg::ref_ptr<osg::Group> getNodeGroup2(Model::Node* firstNode);

	osg::ref_ptr<osg::StateSet> createStateSet() const;

	osg::ref_ptr<osg::Geometry> edgesGeometry;
	osg::ref_ptr<osg::Geometry> edgesOGeometry;
};
}

#endif
