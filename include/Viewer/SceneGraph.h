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
class OsgFrame;

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

	/*!
	 * \brief
	 * Metoda, ktora vracia odkaz na koren grafu.
	 * \returns vrati odkaz na scenu
	 */
	osg::ref_ptr<osg::Group> const getRoot() {
		return root;
	}

	OsgFrame* const getNodeFrame() {
		return nodeFrame;
	}

	void addPermanentNode(osg::ref_ptr<osg::Node> node);

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

	void setFrozen(bool val);

	void setUpdatingNodes(bool val);

private:

	int cleanUp();
	osg::ref_ptr<osg::Node> createSkyBox();
	osg::ref_ptr<osg::Node> createControlFrame();

	Model::Graph * graph;
	Vwr::SceneElements * sceneElements;
	osg::ref_ptr<osg::Group> root;
	osg::ref_ptr<osg::Camera> camera;
	osg::ref_ptr<OsgFrame> nodeFrame;
	osg::ref_ptr<osg::Group> specialNodes;

	bool updateNodes;
	int specialPosition;
	int elementsPosition;
};
}

#endif
