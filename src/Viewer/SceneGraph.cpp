#include "Viewer/SceneGraph.h"
#include "Viewer/SceneElements.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Viewer/SkyTransform.h"
#include "Util/TextureWrapper.h"
#include "Util/Config.h"
#include "Model/Graph.h"
#include "Window/ViewerQT.h"

#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osgWidget/Box>
#include <osgWidget/Label>
#include <osgWidget/Window>
#include <osgWidget/WindowManager>
#include <osgWidget/Browser>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Texture2D>

#include <qdebug.h>
#include <QtGui>

using namespace Vwr;

osg::ref_ptr<osg::Node> experiments() {

	osgWidget::Label* label = new osgWidget::Label("", "");
	label->addSize(22.0f, 22.0f);
	label->setColor(1.0f, 1.0f, 1.0f, 0.5f);
	label->setImage("img/devil.jpg", true);
	//	window->addWidget(label);
	//	window->resize();
	//	this->addDrawable(window);
	//	sw->addChild(window);
	//
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(label);

	//	osgWidget::Window* widget = new osgWidget::Widget("");
	//	osgWidget::Box* widget = new osgWidget::Box("");
	//	widget->addWidget(label);
	//	widget->attachMoveCallback();
	//	widget->attachScaleCallback();
	//	widget->resize();


	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform();
	at->setPosition(osg::Vec3f(0, 0, 0));
	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	at->addChild(geode);

	//	return geode;
	return at;

	//	at->addChild(widget);
	//	osgWidget::WindowManager* wm = new osgWidget::WindowManager(&viewer,
	//			1280.0f, 1024.0f, MASK_2D,
	//			// osgWidget::WindowManager::WM_USE_RENDERBINS |
	//			osgWidget::WindowManager::WM_PICK_DEBUG);
	//	wm->addChild(widget);
	//	at->addChild(widget);
	//	customNodeList.append(at);

	//	osgWidget::GeometryHints hints(osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(1.0f,
	//			0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f), osg::Vec4(1.0f, 1.0f,
	//			1.0f, 1.0f),
	//			osgWidget::GeometryHints::RESIZE_HEIGHT_TO_MAINTAINCE_ASPECT_RATIO);
	//
	//	osg::ref_ptr<osg::Group> group = new osg::Group;
	//
	//	osg::ref_ptr<osgWidget::Browser> browser = new osgWidget::Browser;
	//	if (browser->open("www.google.com", hints)) {
	//		group->addChild(browser.get());
	//		hints.position.x() += 1.1f;
	//	}
	//
	//	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	//	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_CAMERA);
	////	at->addChild(geode);
}

SceneGraph::SceneGraph(Model::Graph* graph) {
	// TODO cleanup class

	camera = NULL;

	root = new osg::Group();
	root->setName("root");
	root->addChild(createSkyBox());
	backgroundPosition = 0;

	reload(graph);
	//	customNodeList.append(experiments());

	//	customNodeList.append(osgDB::readNodeFile("img/axes.osg"));
}

SceneGraph::~SceneGraph() {
	cleanUp();
}

void SceneGraph::reload(Model::Graph * newGraph) {
	if (newGraph == NULL)
		return;
	isUpdating = false;

	int currentPos = cleanUp(); // first available pos

	graph = newGraph;
	sceneElements = new SceneElements(graph->getNodes(), this);

	root->addChild(sceneElements->getElementsGroup());
	elementsPosition = currentPos++;
	customNodesPosition = currentPos++;

	osgUtil::Optimizer opt;
	opt.optimize(root, osgUtil::Optimizer::ALL_OPTIMIZATIONS);

	isUpdating = true;

	qDebug() << "Scene graph loaded (" << graph->getName() << ")";
}

int SceneGraph::cleanUp() {
//	for (int i = 0; i < root->getNumChildren(); i++) {
//		qDebug() << QString::fromStdString(root->getChild(i)->getName());
//	}
	root->removeChildren(1, root->getNumChildren() - 1);
	// NOTE: first child is skybox

	delete sceneElements;
	graph = NULL;
	return 1;
}

osg::ref_ptr<osg::Node> SceneGraph::createSkyBox() {
	osg::ref_ptr<osg::Texture2D> skymap = Util::TextureWrapper::getCoudTexture(
			2048, 1024,
			Util::Config::getValue("Viewer.Display.BackGround.R").toInt(),
			Util::Config::getValue("Viewer.Display.BackGround.G").toInt(),
			Util::Config::getValue("Viewer.Display.BackGround.B").toInt(), 255);

	skymap->setDataVariance(osg::Object::DYNAMIC);
	skymap->setFilter(osg::Texture::MIN_FILTER,
			osg::Texture::LINEAR_MIPMAP_LINEAR);
	skymap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	skymap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	skymap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
	stateset->setRenderBinDetails(-1, "RenderBin");

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1, 1);
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable(
			new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 1));
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	geode->setCullingActive(false);
	geode->setStateSet(stateset);
	geode->addDrawable(drawable);

	osg::ref_ptr<osg::Transform> transform = new SkyTransform;
	transform->setCullingActive(false);
	transform->addChild(geode);

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setRequiresClear(false);
	clearNode->addChild(transform);
	clearNode->setName("skybox");

	return clearNode;
}

osg::ref_ptr<osg::Group> SceneGraph::initCustomNodes() {
	osg::ref_ptr<osg::Group> customNodes = new osg::Group;

	QLinkedList<osg::ref_ptr<osg::Node> >::const_iterator i =
			customNodeList.constBegin();
	while (i != customNodeList.constEnd()) {
		customNodes->addChild(*i);
		++i;
	}
	customNodes->setName("custom_nodes");

	return customNodes;
}

void SceneGraph::update(bool forceIdeal) {
	root->removeChildren(customNodesPosition, 1); // XXX why?

	if (isUpdating || forceIdeal) {
		float interpolationSpeed = Util::Config::getValue(
				"Viewer.Display.InterpolationSpeed").toFloat();
		if (forceIdeal)
			interpolationSpeed = 1;
		sceneElements->updateNodeCoords(interpolationSpeed);
	}

	sceneElements->updateEdgeCoords();
	root->addChild(initCustomNodes());
}

void SceneGraph::setEdgeLabelsVisible(bool visible) {
	QMapIterator<qlonglong, osg::ref_ptr<OsgEdge> > i(
			*sceneElements->getEdges());
	while (i.hasNext()) {
		i.next();
		i.value()->showLabel(visible);
	}
}

void SceneGraph::setNodeLabelsVisible(bool visible) {
	QMapIterator<qlonglong, osg::ref_ptr<OsgNode> > i(
			*sceneElements->getNodes());
	while (i.hasNext()) {
		i.next();
		i.value()->showLabel(visible);
	}
}

void SceneGraph::reloadConfig() {
	Util::TextureWrapper::reloadTextures();

	root->setChild(backgroundPosition, createSkyBox());

	QMapIterator<qlonglong, osg::ref_ptr<OsgNode> > i(
			*sceneElements->getNodes());
	while (i.hasNext()) {
		i.next();
		i.value()->reloadConfig();
	}
}

void SceneGraph::setUpdating(bool val) {
	isUpdating = val;
}

void SceneGraph::setFrozen(bool val) {
	if (graph != NULL) {
		graph->setFrozen(val);
	} else {
		qWarning("No graph set in SceneGraph");
	}
}
