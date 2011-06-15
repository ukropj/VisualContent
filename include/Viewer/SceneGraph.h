#ifndef VIEWER_CORE_GRAPH_DEF
#define VIEWER_CORE_GRAPH_DEF 1

#include <math.h>

#include <QMap>
#include <QList>
#include <QObject>
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

namespace Controls {
class ControlFrame;
}

namespace Vwr {
class SceneElements;
class DataMapping;

/// Scene graph initializator and container.
class SceneGraph {
public:
	SceneGraph();
	~SceneGraph();

	/// Creates visualization of graph.
	void reload(Model::Graph* graph, int origNodeCount, QProgressDialog* pd);
	void reloadConfig();

	void update(bool forceIdeal = false);
	bool setUpdatingNodes(bool val);
	void setClusterThreshold(float value);

	osg::ref_ptr<osg::Group> getRoot() const;
	osg::ref_ptr<Controls::ControlFrame> getControlFrame() const;

	void addPermanentNode(osg::ref_ptr<osg::Node> node);

	void setEdgeLabelsVisible(bool visible);
	void setNodeLabelsVisible(bool visible);
	/// Freezes visualization.
	void setFrozen(bool val);

	void setDataMapping();

	void createExperiment();

private:
	int cleanUp();
	osg::ref_ptr<osg::Node> createSkyBox();
	osg::ref_ptr<osg::Node> createControlFrame();

	Model::Graph* graph;
	Vwr::SceneElements* sceneElements;
	osg::ref_ptr<osg::Group> root;
	osg::ref_ptr<Controls::ControlFrame> controlFrame;
	osg::ref_ptr<osg::Group> specialNodes;
	QMap<qlonglong, DataMapping*> actualMappings;

	bool updateNodes;
	int elementsPosition;

	float interpolationSpeed;
	int nodeCount;
	float maxAllowedClusterSize;
	bool autoClustering;

};
}

#endif
