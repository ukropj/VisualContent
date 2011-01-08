/**
 *  PickHandler.h
 *  Projekt 3DVisual
 */
#ifndef PICKHANDLER_DEF
#define PICKHANDLER_DEF 1

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgViewer/Viewer>
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Vec3f>

#include <QLinkedList>
#include <QTimer>

namespace Vwr {
class CameraManipulator;
class SceneGraph;
class OsgNode;
class OsgEdge;
class OsgFrame;

/**
 *  \class PickHandler
 *  \brief Handles picking events
 */
class PickHandler: public QObject, public osgGA::GUIEventHandler {
Q_OBJECT

typedef QLinkedList<OsgNode* > NodeList;

public:

	enum InputMode {
		NORMAL = 0, DEBUG = 1
	};

	/**
	 *  \fn public constructor  PickHandler(Vwr::CameraManipulator * cameraManipulator, Vwr::CoreGraph * coreGraph)
	 *  \brief Creates picking handler
	 *  \param  cameraManipulator    camera manipulator
	 *  \param  coreGraph     core graph
	 */
	PickHandler(Vwr::CameraManipulator * cameraManipulator,
			Vwr::SceneGraph * sceneGraph);

	void reset();

	/**
	 *  \fn public virtual  handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	 *  \brief Handles events
	 *  \param   ea const     event adapter
	 *  \param   aa    action adapter
	 *  \return bool true, if event was handled
	 */
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void changeMode() {
		mode = mode == DEBUG ? NORMAL : DEBUG;
	}

	/**
	 *  \fn public  getSelectionCenter(bool nodesOnly)
	 *  \brief Returns selection mass center
	 *  \param       nodesOnly     if true, mass will be calculated form nodes only
	 *  \return osg::Vec3
	 */
	osg::Vec3 getSelectionCenter();

	/**
	 *  \fn inline public  getSelectedNodes
	 *  \brief Returns selected nodes
	 *  \return NodeList * selected nodes
	 */
	NodeList getSelectedNodes() const {
		return selectedNodes;
	}

private:
	// Store mouse xy location for button press & move events.
	osg::Vec2f lastPos;
	osg::Vec2f originPos;

	bool leftButtonPressed; // not used now

	// Perform a pick operation.
	OsgNode* pickOne(osgViewer::Viewer* viewer,
			const osgGA::GUIEventAdapter& event);
	QSet<OsgNode*> pickMore(osgViewer::Viewer* viewer,
			const osgGA::GUIEventAdapter& event);

	OsgNode* getNodeAt(osgViewer::Viewer* viewer, const double x,
			const double y);
	QSet<OsgNode*> getNodesInQuad(osgViewer::Viewer* viewer,
			const double xMin, const double yMin, const double xMax,
			const double yMax);
	OsgNode* getNode(osg::NodePath nodePath, bool pickActions = true);
	OsgEdge* getEdgeAt(osgViewer::Viewer* viewer, const double x,
			const double y);//todo implement ..

	void createSelectionQuad();
	void initSelectionQuad(osgViewer::Viewer * viewer);
	void drawSelectionQuad();

	/**
	 *  Vwr::CameraManipulator * cameraManipulator
	 *  \brief camera manipulator
	 */
	Vwr::CameraManipulator * cameraManipulator;

	/**
	 *  Vwr::CoreGraph * coreGraph
	 *  \brief core graph
	 */
	Vwr::SceneGraph * sceneGraph;

	/**
	 *  NodeList selectedNodes
	 *  \brief picked nodes list
	 */
	NodeList selectedNodes;

	OsgFrame* nodeFrame;

	bool multiPickEnabled;

	/**
	 *  QLinkedList<osg::ref_ptr<OsgEdge> > pickedEdges
	 *  \brief picked edges list
	 */
	//	QLinkedList<osg::ref_ptr<OsgEdge> > pickedEdges;

	/**
	 *  osg::ref_ptr projection
	 *  \brief custom node projection
	 */
	osg::ref_ptr<osg::Projection> quadProjection;

	/**
	 *  bool isDrawingSelectionQuad
	 *  \brief true, if selection quad is benign drawn
	 */
	bool isDrawingSelectionQuad;

	/**
	 *  bool isDragging
	 *  \brief true, if user is dragging mouse
	 */
	//	bool isDragging;

	/**
	 *  bool isManipulatingNodes
	 *  \brief true, if user is manipulating nodes
	 */
	bool isResizingNode;

	/**
	 *  osg::ref_ptr<osg::Geode> selectionQuad
	 *  \brief selection quad geode
	 */
	osg::ref_ptr<osg::Geode> selectionQuad;

	char pressedKey;

	osgViewer::Viewer* getViewer(osgGA::GUIActionAdapter& aa) {
		return dynamic_cast<osgViewer::Viewer*> (&aa);
	}

	/**
	 *  int pickMode
	 *  \brief current pick mode
	 */
	InputMode mode;

	bool select(OsgNode* node = NULL, bool singleOnly = false);

	void deselectAll();

	bool toggle(OsgNode* node = NULL);

	/**
	 *  \fn private  handlePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	 *  \brief handles push event
	 *  \param  ea    event adapter
	 *  \param   aa    action adapter
	 *  \return bool true, if event was handled
	 */
	bool handlePush(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);

	/**
	 *  \fn private  handleDoubleclick( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	 *  \brief handles doubleclick event
	 *  \param  ea     event adapter
	 *  \param  aa    action adapter
	 *  \return bool true, if event was handled
	 */
	bool handleDoubleclick(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);

	/**
	 *  \fn private  handleDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	 *  \brief handles drag event
	 *  \param   ea   event adapter
	 *  \param   aa   action adapter
	 *  \return bool true, if event was handled
	 */
	bool handleDrag(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);

	/**
	 *  \fn private  handleMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	 *  \brief handles move event
	 *  \param   ea   event adapter
	 *  \param   aa   action adapter
	 *  \return bool true, if event was handled
	 */
	bool handleMove(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);

	/**
	 *  \fn private  handleRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	 *  \brief handles release event
	 *  \param    ea   event adapter
	 *  \param  aa    action adapter
	 *  \return bool true, if event was handled
	 */
	bool handleRelease(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);
	bool handleKeyDown(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);
	bool handleKeyUp(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);

	bool isShift(const osgGA::GUIEventAdapter& event);
	bool isCtrl(const osgGA::GUIEventAdapter& event);
	bool isAlt(const osgGA::GUIEventAdapter& event);
	bool isKey(char key, const osgGA::GUIEventAdapter& event);

	/**
	 *  const osgGA::GUIEventAdapter * eaPush
	 *  \brief	variable for storing event for detecting double click
	 */
	//	const osgGA::GUIEventAdapter * pushEvent;

	/**
	 *  const osgGA::GUIEventAdapter * eaRel
	 *  \brief	variable for storing event for detecting double click
	 */
	//	const osgGA::GUIEventAdapter * releaseEvent;

	/**
	 *  osgGA::GUIActionAdapter * aaPush
	 *  \brief	variable for storing event for detecting double click
	 */
	//	osgGA::GUIActionAdapter * pushAction;

	/**
	 *  osgGA::GUIActionAdapter * aaRel
	 *  \brief	variable for storing event for detecting double click
	 */
	//	osgGA::GUIActionAdapter * releaseAction;

	/**
	 *  QTimer timer
	 *  \brief timer for detecting double click
	 */
	QTimer * timer;

public slots:
	/**
	 *	\fn public mouseTimerTimeout
	 *	\brief called when user don't double click
	 */
	void mouseTimerTimeout(); // not used now
};
}
#endif
