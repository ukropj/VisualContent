/**
 *  SceneGraph.h
 *  Projekt 3DVisual
 */
#ifndef VIEWER_CORE_GRAPH_DEF
#define VIEWER_CORE_GRAPH_DEF 1

#include <math.h>

#include <QMap>
#include <QLinkedList>
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

class SceneGraph {
public:
	SceneGraph();
	~SceneGraph();

	/**
	 *  \fn public  reload(Model::Graph * graph = 0)
	 *  \brief
	 *  \param   graph
	 */
	void reload(Model::Graph* graph, QProgressDialog* progressBar = 0);

	/**
	 *  \fn public  reloadConfig
	 *  \brief
	 */
	void reloadConfig();

	/*!
	 * Zaktualizuje pozicie uzlov a hran na obrazovke.
	 * Vola sa strale do okola
	 */
	void update(bool forceIdeal = false);

	/**
	 *  \fn inline public  getCustomNodeList
	 *  \brief
	 *  \return QLinkedList<osg::ref_ptr<osg::Node> > *
	 */
	QLinkedList<osg::ref_ptr<osg::Node> > * getCustomNodeList() {
		return &customNodeList;
	}

	/*!
	 * \brief
	 * Metoda, ktora vracia odkaz na koren grafu.
	 * \returns vrati odkaz na scenu
	 */
	osg::ref_ptr<osg::Group> const getRoot() {
		return root;
	}

	void createExperiment();

	/**
	 *  \fn public  setEdgeLabelsVisible(bool visible)
	 *  \brief If true, edge labels will be visible
	 *  \param      visible     edge label visibility
	 */
	void setEdgeLabelsVisible(bool visible);

	/**
	 *  \fn public  setNodeLabelsVisible(bool visible)
	 *  \brief If true, node labels will be visible
	 *  \param       visible     node label visibility
	 */
	void setNodeLabelsVisible(bool visible);

	void toggleFixedNodes(QLinkedList<Vwr::OsgNode* > nodes);

	/**
	 * Freezes/unfreezes current graph
	 */
	void setFrozen(bool val);

	void setUpdatingNodes(bool val);

private:

	bool updateNodes;

	/**
	 *  Vwr::NodeGroup * nodesGroup
	 *  \brief node group
	 */
	Vwr::SceneElements * sceneElements;

	/**
	 *  Model::Graph * graph
	 *  \brief current graph
	 */
	Model::Graph * graph;

	/**
	 *  \fn private  initCustomNodes
	 *  \brief inits custom nodes
	 *  \return osg::ref_ptr
	 */
	osg::ref_ptr<osg::Group> initCustomNodes();

	/**
	 *  \fn private  createSkyBox
	 *  \brief creates sky
	 *  \return osg::ref_ptr skybox node
	 */
	osg::ref_ptr<osg::Node> createSkyBox();

	/**
	 *  osg::ref_ptr camera
	 *  \brief current viewing camera
	 */
	osg::ref_ptr<osg::Camera> camera;

	/**
	 *  osg::ref_ptr root
	 *  \brief root node
	 */
	osg::ref_ptr<osg::Group> root;

	/**
	 *  QLinkedList<osg::ref_ptr<osg::Node> > customNodeList
	 *  \brief list of custom nodes
	 */
	QLinkedList<osg::ref_ptr<osg::Node> > customNodeList;

	/**
	 *  \fn private  cleanUp
	 *  \brief Cleans up memory
	 */
	int cleanUp();

	/**
	 *  int backgroundPosition
	 *  \brief background node position
	 */
	int backgroundPosition;

	/**
	 *  int nodesPosition
	 *  \brief nodes group position
	 */
	int elementsPosition;

	/**
	 *  int customNodesPosition
	 *  \brief custom nodes group position
	 */
	int customNodesPosition;
};
}

#endif
