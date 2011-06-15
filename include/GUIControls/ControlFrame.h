#ifndef ControlFrame_H_
#define ControlFrame_H_

#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>
#include <osg/Geode>
#include <osgGA/GUIEventAdapter>

#include <string>

#include <QObject>
#include <QString>
#include <QMap>

namespace Vwr {
class SceneGraph;
class AbstractNode;
}

/// Vizual control elements
/**
 * \author Jakub Ukrop
 */
namespace Controls {
class FrameButton;
using namespace Vwr;

/// Frame around selected Vwr::AbstractNode. Behaves as HUD. Contains control elements (buttons).
class ControlFrame: public QObject, public osg::PositionAttitudeTransform {
Q_OBJECT
public slots:
	void nodeAdded(AbstractNode* node);
	void nodeRemoved(AbstractNode* node);
public:
	ControlFrame(Vwr::SceneGraph* sceneGraph);
	~ControlFrame();
	void show();
	void hide();
	void addNode(Vwr::AbstractNode* node);
	void setNode(Vwr::AbstractNode* node);
	Vwr::AbstractNode* getNode() const;

	bool isNodeSet() const;
	bool isActive() const;

	void updateGeometry();
	void updateProjection();
	bool activateAction(osg::Geode* button);
	void deactivateAction();

	bool handlePush(const osgGA::GUIEventAdapter& event);
	bool handleDoubleclick(const osgGA::GUIEventAdapter& event);
	bool handleDrag(const osgGA::GUIEventAdapter& event);
	bool handleMove(const osgGA::GUIEventAdapter& event);
	bool handleRelease(const osgGA::GUIEventAdapter& event);
	bool handleKeyDown(const osgGA::GUIEventAdapter& event);
	bool handleKeyUp(const osgGA::GUIEventAdapter& event);

	osg::Vec2f getPushPos() const {
		return originPos;
	}
	osg::Vec2f getCurrentPos() const {
		return currentPos;
	}
	osg::Vec2f getLastDragPos() const {
		return lastDragPos;
	}

	Vwr::SceneGraph* getSceneGraph() const {
		return sceneGraph;
	}
private:
	void createButtons();
	void createBorder();
	void insertButton(osg::ref_ptr<FrameButton> button, osg::Transform* transform);
	void updateButtons(Vwr::AbstractNode* node, bool nodeAdded = false);

	osg::Vec2f originPos;
	osg::Vec2f currentPos;
	osg::Vec2f lastDragPos;

	Vwr::AbstractNode* myNode;
	Vwr::SceneGraph* sceneGraph;

	QMap<std::string, osg::ref_ptr<FrameButton> > buttons;
	osg::ref_ptr<FrameButton> activeButton;
	osg::ref_ptr<FrameButton> nullButton;

	osg::ref_ptr<osg::AutoTransform> mt;
	osg::ref_ptr<osg::AutoTransform> mt2;
	osg::ref_ptr<osg::Geode> rect;

	static osg::Vec2f MIN_SIZE;
};

}

#endif /* ControlFrame_H_ */
