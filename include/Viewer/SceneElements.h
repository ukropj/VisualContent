/**
 *  NodeGroup.h
 *  Projekt 3DVisual
 */
#ifndef VIEWER_NODEWRAPPER_DEF
#define VIEWER_NODEWRAPPER_DEF 1

#include <QMap>
#include <QSet>
#include <QLinkedList>
#include <QProgressDialog>

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

	SceneElements(QMap<qlonglong, Model::Node* > *nodes, QMap<qlonglong, Model::Edge* > *edges,
			SceneGraph* sceneGraph, QProgressDialog* progressBar = 0);
	~SceneElements();

	void updateNodes(float interpolationSpeed);
	void updateEdges();

	osg::Group* getElementsGroup();
	QList<OsgNode* > getNodes();

private:
	SceneGraph* sceneGraph;

	QSet<qlonglong> nodeIds;
	QList<OsgNode* > nodes;
	QList<OsgEdge* > edges;

	osg::ref_ptr<osg::Group> elementsGroup;

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

	int step;
	QProgressDialog* pd;
};
}

#endif
