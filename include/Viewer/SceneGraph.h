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
class ControlFrame;
class DataMapping;

class SceneGraph {
public:
	SceneGraph();
	~SceneGraph();

	void reload(Model::Graph* graph, QProgressDialog* progressBar = 0);
	void reloadConfig();

	void update(bool forceIdeal = false);
	void setUpdatingNodes(bool val);

	osg::ref_ptr<osg::Group> getRoot() const;
	osg::ref_ptr<ControlFrame> getNodeFrame() const;

	void addPermanentNode(osg::ref_ptr<osg::Node> node);

	void setEdgeLabelsVisible(bool visible);
	void setNodeLabelsVisible(bool visible);
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
	osg::ref_ptr<ControlFrame> controlFrame;
	osg::ref_ptr<osg::Group> specialNodes;
	QMap<qlonglong, DataMapping*> actualMappings;

	bool updateNodes;
	int elementsPosition;

};
}

#endif
