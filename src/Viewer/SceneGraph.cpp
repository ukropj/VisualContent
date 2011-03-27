#include "Viewer/SceneGraph.h"
#include "Viewer/SceneElements.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Viewer/ControlFrame.h"
#include "Viewer/SkyTransform.h"
#include "Viewer/WidgetContent.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"
#include "Util/CameraHelper.h"
#include "Model/Graph.h"

#include "Viewer/DataMapping.h"
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Model/Type.h"
#include "Window/DataMappingDialog.h"

#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osg/Texture>
#include <osg/MatrixTransform>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <QDebug>
#include <QtGui>

#include "OsgQtBrowser/QWidgetImage.h"

using namespace Vwr;

SceneGraph::SceneGraph() {
	root = new osg::Group();
	root->setName("root");

	specialNodes = new osg::Group();
	specialNodes->setName("custom_nodes");
	specialNodes->addChild(createSkyBox());
	specialNodes->addChild(createControlFrame());
	root->addChild(specialNodes);

	sceneElements = NULL;
	graph = NULL;
	elementsPosition = 1;

	updateNodes = true;
}

SceneGraph::~SceneGraph() {
	if (sceneElements != NULL)
		delete sceneElements;
//	qDebug() << "SceneGraph deleted";
}

int SceneGraph::cleanUp() {
	root->removeChildren(elementsPosition, root->getNumChildren() - elementsPosition);
	if (sceneElements != NULL)
		delete sceneElements;
	sceneElements = NULL;
	graph = NULL; // graph is not deleted here, it may be still used even if graphics is gone
	actualMappings.clear();
	return root->getNumChildren();
}

void SceneGraph::reload(Model::Graph* newGraph, QProgressDialog* progressBar) {
	if (newGraph == NULL)
		return;
	int currentPos = cleanUp(); // first available pos

	graph = newGraph;
	sceneElements = new SceneElements(graph->getNodes(), graph->getEdges(), graph->getTypes(), progressBar);

	elementsPosition = currentPos++;
	root->addChild(sceneElements->getElementsGroup());

	osgUtil::Optimizer opt;
	opt.optimize(root, osgUtil::Optimizer::CHECK_GEOMETRY);
	updateNodes = true;

	qDebug() << "Scene graph loaded: " << graph->toString();
	//	osgDB::writeNodeFile(*root, "graph.osg");
}

osg::ref_ptr<osg::Node> SceneGraph::createControlFrame() {
	controlFrame = new ControlFrame(this);

    osg::Camera* hudCamera = new osg::Camera;
    hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    hudCamera->setProjectionMatrixAsOrtho2D(0,1,0,1);
    hudCamera->setViewMatrix(osg::Matrix::identity());
    hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
    hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

	hudCamera->addChild(controlFrame);
	return hudCamera;
}

osg::ref_ptr<osg::Node> SceneGraph::createSkyBox() {
	osg::ref_ptr<osg::Texture2D> skymap = Util::TextureWrapper::createCloudTexture(
			2048, 1024,
			Util::Config::getColorI("Viewer.Display.BackGround1"),
			Util::Config::getColorI("Viewer.Display.BackGround2"));

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
	clearNode->setName("skybox_background");

	return clearNode;
}

void SceneGraph::addPermanentNode(osg::ref_ptr<osg::Node> node) {
	specialNodes->addChild(node);
}

osg::ref_ptr<osg::Group> SceneGraph::getRoot() const {
	return root;
}

osg::ref_ptr<ControlFrame> SceneGraph::getNodeFrame() const{
	return controlFrame;
}


void SceneGraph::update(bool forceIdeal) {
//	qDebug() << "SE Updating";
	if (graph == NULL || sceneElements == NULL)
		return;

	if (updateNodes || forceIdeal) {
		float interpolationSpeed = Util::Config::getValue(
				"Viewer.Display.InterpolationSpeed").toFloat();
		if (forceIdeal)
			interpolationSpeed = 1;
		sceneElements->updateNodes(interpolationSpeed);
	}
	sceneElements->updateEdges();
	controlFrame->updateGeometry();
}

void SceneGraph::setNodeLabelsVisible(bool visible) {
	if (sceneElements == NULL)
		return;
	QListIterator<OsgNode* > i(sceneElements->getNodes());
	while (i.hasNext()) {
		i.next()->showLabel(visible);
	}
}

void SceneGraph::setEdgeLabelsVisible(bool visible) {
	if (sceneElements == NULL)
		return;
	sceneElements->setEdgeLabelsVisible(visible);
}

void SceneGraph::reloadConfig() {
//	Util::TextureWrapper::reloadTextures();
//
//	root->setChild(backgroundPosition, createSkyBox());
//
//	QMapIterator<qlonglong, osg::ref_ptr<OsgNode> > i(
//			*sceneElements->getNodes());
//	while (i.hasNext()) {
//		i.next();
//		i.value()->reloadConfig();
//	}
}

bool SceneGraph::setUpdatingNodes(bool val) {
	bool oldVal = updateNodes;
	updateNodes = val;
	return oldVal;
}

void SceneGraph::setFrozen(bool val) {
	if (graph == NULL) {
		qWarning("No graph set in SceneGraph::setFrozen()");
		return;
	}
	graph->setFrozen(val);
}

void SceneGraph::setDataMapping() {
	QList<Model::Type*> types = graph->getTypes()->values();
	if (types.size() > 0) {
		QListIterator<Model::Type*> ti(types);
		while (ti.hasNext()) {
			Model::Type* type = ti.next();
			if (!actualMappings.contains(type->getId()))
				actualMappings.insert(type->getId(), new DataMapping());
		}

		Window::DataMappingDialog* dialog =
				new Window::DataMappingDialog(types, &actualMappings, QApplication::activeWindow());
		dialog->exec();
		delete dialog;

		QListIterator<OsgNode* > i(sceneElements->getNodes());
		while (i.hasNext()) {
			OsgNode* node = i.next();
			node->setDataMapping(actualMappings.value(node->getNode()->getType()->getId()));
		}
		QListIterator<OsgEdge* > j(sceneElements->getEdges());
		while (j.hasNext()) {
			OsgEdge* edge = j.next();
			edge->setDataMapping(actualMappings.value(edge->getEdge()->getType()->getId()));
		}
	}
}

void SceneGraph::createExperiment() {
/*
	QWidget* widget = new QWidget;
	widget->setLayout(new QVBoxLayout);

	QString text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
	QLabel* l = new QLabel();
	l->setPixmap(QPixmap("img/pic11.jpg"));

	widget->layout()->addWidget(l);
	widget->setGeometry(1, 1, 350, 350);

	osg::ref_ptr<QWidgetImage> widgetImage = new QWidgetImage(widget);
//	widgetImage->getQWidget()->setAttribute(Qt::WA_TranslucentBackground);
	widgetImage->getQGraphicsViewAdapter()->setBackgroundColor(QColor(0, 0, 0, 0));

	// determines size & ratio!
	osg::Geometry* quad = osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0),
			osg::Vec3(100, 0, 0), osg::Vec3(0, 100, 0), 1, 1);
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(quad);

	osg::AutoTransform* mt = new osg::AutoTransform;
	mt->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	mt->setAutoScaleToScreen(true);
	mt->setPosition(osg::Vec3d(30,30,30));
//	mt->setMatrix(osg::Matrix::rotate(osg::Vec3(0, 1, 0),
//			osg::Vec3(0, 0, 1)));
	mt->addChild(geode);

	mt->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	mt->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	mt->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	mt->getOrCreateStateSet()->setAttribute(new osg::Program);

	osg::Texture2D* texture = new osg::Texture2D(widgetImage.get());
	texture->setResizeNonPowerOfTwoHint(false);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	mt->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture,
			osg::StateAttribute::ON);

	// this enables interaction with the widget (buggy), use current camera
	// nothing is shown without this code!
	osg::Camera* camera = 0;
	osgViewer::InteractiveImageHandler* handler =
			new osgViewer::InteractiveImageHandler(widgetImage.get(), texture, camera);
	quad->setEventCallback(handler);
	quad->setCullCallback(handler);

	customNodeList.append(mt);
*/

	// HUD dislpay:

	osg::Camera* camera = new osg::Camera;
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
//	camera->setAllowEventFocus(false);
	camera->setProjectionMatrix(osg::Matrix::ortho2D(
			Util::CameraHelper::windowX(), Util::CameraHelper::windowWidth(),
			Util::CameraHelper::windowY(), Util::CameraHelper::windowHeight()));

	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	text->setCharacterSize(50);
	text->setAxisAlignment(osgText::TextBase::XY_PLANE);
	text->setPosition(osg::Vec3f(200.0f, 300.0f, 0.0f));
	text->setText("QWERTY Z");
	text->setColor(osg::Vec4f(1,0,0,1));
	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
	textGeode->addDrawable(text.release());
	textGeode->setName("HUD");

//	osg::ref_ptr<osg::StateSet> stateSet1 = new osg::StateSet();
//	stateSet1->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//	stateSet1->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
//	textGeode->setStateSet(stateSet1);

//	camera->addChild(textGeode);
	camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	osg::Geometry* nodeQuad = osg::createTexturedQuadGeometry(
			osg::Vec3(-10, -10, 0),
			osg::Vec3(20, 0, 0), osg::Vec3(0, 20, 0), 1, 1);
	osg::Geode* g = new osg::Geode;
	g->addDrawable(nodeQuad);
	g->setName("G");
	camera->addChild(g);
//
//	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
//	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
//
//	g->setStateSet(stateSet);

//	osg::ref_ptr<osg::Group> group = new osg::Group;
//	group->addChild(g);
//	group->addChild(camera);

	osg::ref_ptr<osg::Node> scene = osgDB::readNodeFile("img/cow.osg");

	osg::ref_ptr<osg::Group> group = dynamic_cast<osg::Group*>(scene.get());
	if (!group)	{
		group = new osg::Group;
		group->addChild(scene.get());
	}

	group->addChild(camera);

	addPermanentNode(group);
}
