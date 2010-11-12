/**
 *  SceneGraph.h
 *  Projekt 3DVisual
 */
#ifndef VIEWER_CORE_GRAPH_DEF
#define VIEWER_CORE_GRAPH_DEF 1

#include <math.h>

#include <osg/ref_ptr>
#include <osg/CullFace>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/Depth>
#include <osg/TextureCubeMap>
#include <osg/AutoTransform>
#include <osgUtil/Optimizer>

#include <QMap>
#include <QLinkedList>

#include "Model/Edge.h"
#include "Model/Node.h"

namespace Model {
class Graph;
}

namespace Vwr {
class SceneElements;
class EdgeGroup;
class OsgNode;
class OsgEdge;

/*!
 * \brief
 * Trieda vykreslujuca aktualny graf.
 *
 * \author
 * Michal Paprcka
 * \version
 * 3.0
 * \date
 * 7.12.2009
 */
class SceneGraph {
public:
	/*!
	 *
	 * \param in_nodes
	 * Zoznam uzlov
	 *
	 * \param in_edges
	 * Zoznam hran
	 *
	 * \param in_types
	 * Zoznam typov
	 *
	 * Konstruktor triedy.
	 *
	 */
	SceneGraph(Model::Graph* graph);
	/*!
	 * Destruktor.
	 */
	~SceneGraph();

	/**
	 *  \fn public  reload(Model::Graph * graph = 0)
	 *  \brief
	 *  \param   graph
	 */
	void reload(Model::Graph* graph = 0);

	/**
	 *  \fn public  reloadConfig
	 *  \brief
	 */
	void reloadConfig();

	/*!
	 * Zaktualizuje pozicie uzlov a hran na obrazovke.
	 * Vola sa strale do okola
	 */
	void update();

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

	/**
	 *  \fn inline public  setCamera(osg::ref_ptr<osg::Camera> camera)
	 *  \brief Sets current viewing camera to all edges
	 *  \param     camera     current camera
	 */
	void setCamera(osg::ref_ptr<osg::Camera> camera) {
		this->camera = camera;
	}

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

	/**
	 * Freezes/unfreezes current graph
	 */
	void setFrozen(bool val);

	void setNodesFreezed(bool val);

	QList<osg::Vec3f> getViewCoords() {
		osg::Vec3f eye, center, up;
		camera->getViewMatrixAsLookAt(eye, center, up);
		QList<osg::Vec3f> vects;
		vects << eye << center << up;
		return vects;
	}

private:

	bool nodesFreezed;

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

	osg::Vec3d getEye();

	/**
	 *  \fn private  cleanUp
	 *  \brief Cleans up memory
	 */
	void cleanUp();

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
