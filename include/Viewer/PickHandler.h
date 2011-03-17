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

#include <QList>
#include <QTimer>

namespace Vwr {
class SceneGraph;
class OsgNode;
class AbstractNode;
class OsgEdge;
class ControlFrame;

/**
 *  \class PickHandler
 *  \brief Handles picking events
 */
class PickHandler: public QObject, public osgGA::GUIEventHandler {
Q_OBJECT

public:

	enum InputMode {
		NORMAL = 0, DEBUG = 1
	};

	PickHandler(SceneGraph * sceneGraph);
	~PickHandler();

	void reset();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void changeMode() {
		mode = mode == DEBUG ? NORMAL : DEBUG;
	}

	osg::Vec3 getSelectionCenter() const;
	QList<OsgNode* > getSelectedNodes() const;

private:
	// Store mouse xy location for button press & move events.
	osg::Vec2f lastPos;
	osg::Vec2f originPos;

	bool leftButtonPressed; // not used now

	// Perform a pick operation.
	OsgNode* pickOne(osgViewer::Viewer* viewer,
			const osgGA::GUIEventAdapter& event);
	QList<OsgNode*> pickMore(osgViewer::Viewer* viewer,
			const osgGA::GUIEventAdapter& event);

	OsgNode* getNodeAt(osgViewer::Viewer* viewer, const double x,
			const double y);
	QList<OsgNode*> getNodesInQuad(osgViewer::Viewer* viewer,
			const double xMin, const double yMin, const double xMax,
			const double yMax);
	OsgNode* getNode(osg::NodePath nodePath);

	void createControlHUD();
	void createSelectionQuad();
	void initSelectionQuad();
	void drawSelectionQuad();

	SceneGraph * sceneGraph;

	QList<OsgNode* > selectedNodes;

	osg::ref_ptr<osg::Geode> selectionQuad;
	osg::ref_ptr<ControlFrame> nodeFrame;

	// flags
	bool multiPickEnabled;
	bool isDrawingSelectionQuad;
	bool isResizingNode;
	char pressedKey;
	InputMode mode;

	osgViewer::Viewer* getViewer(osgGA::GUIActionAdapter& aa) {
		return dynamic_cast<osgViewer::Viewer*> (&aa);
	}

	bool select(OsgNode* node = NULL, bool singleOnly = false);
	void deselectAll();

	bool handlePush(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);
	bool handleDoubleclick(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);
	bool handleDrag(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);
	bool handleMove(const osgGA::GUIEventAdapter& event,
			osgGA::GUIActionAdapter& action);
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
	void mouseTimerTimeout(); // not used now
	void windowResized();
};
}
#endif
