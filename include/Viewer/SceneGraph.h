/**
 *  SceneGraph.h
 *  Projekt 3DVisual
 */
#ifndef VIEWER_CORE_GRAPH_DEF
#define VIEWER_CORE_GRAPH_DEF 1

#include <math.h>

#include <QMap>
#include <QList>
#include <QProgressDialog>

#include <osg/ref_ptr>
#include <osg/CullFace>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/Depth>
#include <osg/TextureCubeMap>
#include <osg/AutoTransform>
#include <osgUtil/Optimizer>

namespace Model {
class Graph;
}

namespace Vwr {
class SceneElements;
class OsgNode;
class OsgEdge;
class OsgFrame;

class SceneGraph {
public:
	SceneGraph();
	~SceneGraph();

	void reload(Model::Graph* graph, QProgressDialog* progressBar = 0);
	void reloadConfig();

	void update(bool forceIdeal = false);
	void setUpdatingNodes(bool val);

	osg::ref_ptr<osg::Group> getRoot() const;
	osg::ref_ptr<OsgFrame> getNodeFrame() const;

	void addPermanentNode(osg::ref_ptr<osg::Node> node);
	void createExperiment();

	void setEdgeLabelsVisible(bool visible);
	void setNodeLabelsVisible(bool visible);
	void toggleFixedNodes(QList<OsgNode* > nodes);
	void setFrozen(bool val);

private:

	int cleanUp();
	osg::ref_ptr<osg::Node> createSkyBox();
	osg::ref_ptr<osg::Node> createControlFrame();
	void createDataMapping(Model::Graph* graph);

	Model::Graph* graph;
	Vwr::SceneElements* sceneElements;
	osg::ref_ptr<osg::Group> root;
	osg::ref_ptr<OsgFrame> nodeFrame;
	osg::ref_ptr<osg::Group> specialNodes;

	bool updateNodes;
	int specialPosition;
	int elementsPosition;

};
}

#endif
