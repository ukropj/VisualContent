#include "Viewer/SceneGraph.h"
#include "Viewer/SceneElements.h"
#include "Viewer/OsgNode.h"
#include "Viewer/OsgEdge.h"
#include "Viewer/SkyTransform.h"
#include "Util/TextureWrapper.h"
#include "Util/Config.h"
#include "Model/Graph.h"
#include "Viewer/WidgetContent.h"

#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osg/Texture>
#include <osg/MatrixTransform>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <QDebug>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QPalette>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include "OsgQtBrowser/QWidgetImage.h"

using namespace Vwr;

SceneGraph::SceneGraph() {
	camera = NULL;

	root = new osg::Group();
	root->setName("root");
	root->addChild(createSkyBox());
	backgroundPosition = 0;
	sceneElements = new SceneElements(new QMap<qlonglong, Model::Node*> (), new QMap<qlonglong, Model::Edge*> (),
			this);

	updateNodes = true;

	graph == NULL;

	//	customNodeList.append(osgDB::readNodeFile("img/axes.osg"));
}

SceneGraph::~SceneGraph() {
	cleanUp();
	root->removeChild(0, 1);
}

void SceneGraph::reload(Model::Graph * newGraph, QProgressDialog* progressBar) {
	if (newGraph == NULL)
		return;
	int currentPos = cleanUp(); // first available pos

	graph = newGraph;
	sceneElements = new SceneElements(graph->getNodes(), graph->getEdges(), this, progressBar);

	root->addChild(sceneElements->getElementsGroup());
	elementsPosition = currentPos++;
	customNodesPosition = currentPos++;

	osgUtil::Optimizer opt;
	opt.optimize(root, osgUtil::Optimizer::CHECK_GEOMETRY);
	updateNodes = true;

	qDebug() << "Scene graph loaded (" << graph->getName() << ")";

	//	osgDB::writeNodeFile(*root, "graph.osg");
}

int SceneGraph::cleanUp() {
	root->removeChildren(1, root->getNumChildren() - 1);
	// NOTE: first child is skybox

	delete sceneElements;
	graph = NULL; // graph is not deleted here, it may be still used even is graphics is gone
	return 1;
}

osg::ref_ptr<osg::Node> SceneGraph::createSkyBox() {
	osg::ref_ptr<osg::Texture2D> skymap = Util::TextureWrapper::createCloudTexture(
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
	if (graph == NULL) {
		return;
	}
	root->removeChildren(customNodesPosition, 1); // XXX why?

	if (updateNodes || forceIdeal) {
		float interpolationSpeed = Util::Config::getValue(
				"Viewer.Display.InterpolationSpeed").toFloat();
		if (forceIdeal)
			interpolationSpeed = 1;
		sceneElements->updateNodes(interpolationSpeed);
	}

	sceneElements->updateEdges();
	root->addChild(initCustomNodes());
}

void SceneGraph::setEdgeLabelsVisible(bool visible) {
	//	QMapIterator<qlonglong, osg::ref_ptr<OsgEdge> > i(
	//			*sceneElements->getEdges());
	//	while (i.hasNext()) {
	//		i.next();
	//		i.value()->showLabel(visible);
	//	}
}

void SceneGraph::setNodeLabelsVisible(bool visible) {
	QMapIterator<qlonglong, osg::ref_ptr<OsgNode> > i(
			*sceneElements->getNodes());
	while (i.hasNext()) {
		i.next();
		i.value()->showLabel(visible);
	}
}

void SceneGraph::toggleFixedNodes(QLinkedList<OsgNode* > nodes) {
	QLinkedList<OsgNode* >::const_iterator i = nodes.constBegin();
	while (i != nodes.constEnd()) {
		(*i)->setFixed(!(*i)->isFixed());
		++i;
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

void SceneGraph::setUpdatingNodes(bool val) {
	updateNodes = val;
}

void SceneGraph::setFrozen(bool val) {
	if (graph == NULL) {
		qWarning("No graph set in SceneGraph");
		return;
	}
	graph->setFrozen(val);
}


void SceneGraph::createExperiment() {
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


//	customNodeList.append(mt);
}
