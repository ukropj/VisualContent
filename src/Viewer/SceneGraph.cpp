#include "Viewer/SceneGraph.h"
#include "Viewer/EdgeGroup.h"
#include "Viewer/NodeGroup.h"
#include "Util/DataHelper.h"
#include "Util/TextureWrapper.h"
#include "Util/Config.h"
#include "Viewer/SkyTransform.h"
#include "Model/Graph.h"

#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osgWidget/Box>
#include <osgWidget/Label>
#include <osgWidget/Window>
#include <osgWidget/WindowManager>
#include <osgWidget/Browser>
//#include <osgWidget/GeometryHints>
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
	at->setPosition(osg::Vec3f(0,0,0));
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
}

void SceneGraph::reload(Model::Graph * newGraph) {
	cleanUp();
	int currentPos = 1;

	if (root->getNumChildren() > 1) {
		for (int x = 6; x > 0; x--) // XXX preco 6 ??
			root->removeChildren(x, 1);
	}

	graph = newGraph;

	nodesGroup = new Vwr::NodeGroup(graph->getNodes());
	root->addChild(nodesGroup->getGroup());
	nodesPosition = currentPos++;

	edgesGroup = new Vwr::EdgeGroup(graph->getEdges(), Util::Config::getValue(
			"Viewer.Textures.EdgeScale").toFloat());
	root->addChild(edgesGroup->getGroup());
	edgesPosition = currentPos++;

	//	this->qmetaNodesGroup = new Vwr::NodeGroup(qmetaNodes);
	//	root->addChild(qmetaNodesGroup->getGroup());
	//	qmetaNodesPosition = currentPos++;
	//
	//	this->qmetaEdgesGroup = new Vwr::EdgeGroup(qmetaEdges,
	//			Util::Config::getValue("Viewer.Textures.EdgeScale").toFloat());
	//	root->addChild(qmetaEdgesGroup->getGroup());
	//	qmetaEdgesPosition = currentPos++;

	root->addChild(initEdgeLabels());
	labelsPosition = currentPos++;

	customNodesPosition = currentPos;

	osgUtil::Optimizer opt;
	opt.optimize(edgesGroup->getGroup(), osgUtil::Optimizer::CHECK_GEOMETRY);

	nodesFreezed = false;
}

void SceneGraph::cleanUp() {
	//	delete qmetaEdgesGroup;
	delete edgesGroup;
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

osg::ref_ptr<osg::Group> SceneGraph::initEdgeLabels() {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setName("edges_labels");

	QMapIterator<qlonglong, osg::ref_ptr<Model::Edge> > i(*graph->getEdges());

	while (i.hasNext()) {
		i.next();
		geode->addDrawable(i.value()->getLabel());
	}

	osg::ref_ptr<osg::Group> labels = new osg::Group;
	labels->addChild(geode);
	labels->setNodeMask(0);

	return labels;
}

osg::ref_ptr<osg::Group> SceneGraph::initCustomNodes() {
	osg::ref_ptr<osg::Group> customNodes = new osg::Group;

	QLinkedList<osg::ref_ptr<osg::Node> >::const_iterator i =
			customNodeList.constBegin();
	while (i != customNodeList.constEnd()) {
		customNodes->addChild(*i);
		++i;
	}

	return customNodes;
}

void SceneGraph::update() {
	root->removeChildren(customNodesPosition, 1);

	synchronize();

	float graphScale = Util::Config::getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	if (!this->nodesFreezed) {
		float interpolationSpeed = Util::Config::getValue(
				"Viewer.Display.InterpolationSpeed").toFloat();
		nodesGroup->updateNodeCoordinates(interpolationSpeed);
		//		qmetaNodesGroup->updateNodeCoordinates(interpolationSpeed);
	} else {
		nodesGroup->updateNodeCoordinates(1);
		//		qmetaNodesGroup->updateNodeCoordinates(1);
	}

	edgesGroup->updateEdgeCoords(getViewVector());
//	qDebug() << "Edge group updated";
	//	qmetaEdgesGroup->updateEdgeCoords();
	root->addChild(initCustomNodes());
}

osg::Vec3d SceneGraph::getViewVector() {
	osg::Vec3d up;
	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d viewVec(0, 0, 1);
	if (camera != NULL) {
		camera->getViewMatrixAsLookAt(eye, center, up);
		viewVec = eye - center;
		//		qDebug() << eye.x() << " " << eye.y() << " " << eye.z() << "\n";
		//		qDebug() << center.x() << " " << center.y() << " " << center.z() << "\n";
	} else {
		qWarning("SceneGraph::getViewVector camera == NULL");
	}
	viewVec.normalize();
	return viewVec;
}

void SceneGraph::synchronize() {
	//	qmetaNodesGroup->synchronizeNodes();
	//	qmetaEdgesGroup->synchronizeEdges();
}

void SceneGraph::setEdgeLabelsVisible(bool visible) {
	root->getChild(labelsPosition)->setNodeMask(visible);
}

void SceneGraph::setNodeLabelsVisible(bool visible) {
	QMapIterator<qlonglong, osg::ref_ptr<Model::Node> > i(*graph->getNodes());
	while (i.hasNext()) {
		i.next();
		i.value()->showLabel(visible);
	}
}

void SceneGraph::reloadConfig() {
	root->setChild(backgroundPosition, createSkyBox());

	QMapIterator<qlonglong, osg::ref_ptr<Model::Node> > i(*graph->getNodes());
	while (i.hasNext()) {
		i.next();
		i.value()->reloadConfig();
	}

	root->setChild(labelsPosition, initEdgeLabels());
	Util::TextureWrapper::reloadTextures();
}

SceneGraph::~SceneGraph() {
	cleanUp();
}

void SceneGraph::setNodesFreezed(bool val) {
	this->nodesFreezed = val;
	nodesGroup->freezeNodePositions();
	//	qmetaNodesGroup->freezeNodePositions();
}

void SceneGraph::setFrozen(bool val) {
	graph->setFrozen(val);
}
