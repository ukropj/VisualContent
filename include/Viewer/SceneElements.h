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
class Type;
}

namespace Vwr {
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
			QMap<qlonglong, Model::Type* > *types, QProgressDialog* progressBar = 0);
	~SceneElements();

	void updateNodes(float interpolationSpeed, float maxClusterSize);
	void updateEdges();
	void setEdgeLabelsVisible(bool visible);

	osg::Group* getElementsGroup();
	QList<OsgNode* > getNodes();
	QList<OsgEdge* > getEdges();


private:
	QSet<qlonglong> nodeIds;
	QList<OsgNode* > nodes;
	QList<OsgEdge* > edges;

	osg::ref_ptr<osg::Group> elementsGroup;

	osg::ref_ptr<osg::Group> initNodes(QMap<qlonglong, Model::Node* > *nodes);
	osg::ref_ptr<osg::Group> initEdges(QMap<qlonglong, Model::Edge* > *edges);

	osg::ref_ptr<OsgNode> wrapNode(Model::Node* node);

	osg::ref_ptr<osg::Group> getNodeGroup1(Model::Node* node,
			Model::Edge* parentEdge);
	osg::ref_ptr<osg::Group> getNodeGroup2(Model::Node* firstNode);

	osg::ref_ptr<osg::StateSet> createStateSet() const;

	osg::ref_ptr<osg::Geometry> edgesGeometry;
	osg::ref_ptr<osg::Geometry> edgesOGeometry;
	osg::ref_ptr<osg::Geode> edgeLabels;

	int step;
	QProgressDialog* pd;
};
}

#endif
