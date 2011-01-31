#include "Window/ViewerQT.h"
#include "Util/Config.h"
#include "Util/CameraHelper.h"
#include "Viewer/PickHandler.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/SceneGraph.h"

#include <osgViewer/ViewerBase>

using namespace Window;

ViewerQT::ViewerQT(Vwr::SceneGraph* sceneGraph, QWidget* parent,
		const char* name, const QGLWidget* shareWidget, WindowFlags f) :
		AdapterWidget(parent, name, shareWidget, f) {
	Util::CameraHelper::setCamera(getCamera());
	this->sceneGraph = sceneGraph;

	osg::DisplaySettings::instance()->setStereo(Util::Config::getValue(
			"Viewer.Display.Stereoscopic").toInt());
	osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::ANAGLYPHIC);

	getCamera()->setViewport(new osg::Viewport(0, 0, width(), height()));
	getCamera()->setProjectionMatrixAsPerspective(60,
			static_cast<double> (width()) / static_cast<double> (height()),
			0.01,
			Util::Config::getValue("Viewer.Display.ViewDistance").toFloat());
	getCamera()->setGraphicsContext(getGraphicsWindow());
	getCamera()->setComputeNearFarMode(
			osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	manipulator = new Vwr::CameraManipulator();
	pickHandler = new Vwr::PickHandler(this->sceneGraph);

	addEventHandler(new osgViewer::StatsHandler);
	addEventHandler(pickHandler);
	setCameraManipulator(manipulator);

	getCamera()->setClearColor(osg::Vec4(0, 0, 0, 1));
	getCamera()->setViewMatrixAsLookAt(osg::Vec3d(-10, 0, 0), osg::Vec3d(0,
			0, 0), osg::Vec3d(0, 1, 0));
	setSceneData(this->sceneGraph->getRoot());

	setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_timer.start(10);
}

ViewerQT::~ViewerQT() {
	_timer.stop();
	removeEventHandler(pickHandler);
	pickHandler = NULL;
	delete sceneGraph;
//	qDebug() << "ViewerQT deleted";
}

Vwr::PickHandler* ViewerQT::getPickHandler() const {
	return pickHandler;
}

Vwr::CameraManipulator* ViewerQT::getCameraManipulator() const {
	return manipulator;
}

void ViewerQT::reloadConfig() {
	manipulator->setMaxSpeed(Util::Config::getValue(
			"Viewer.CameraManipulator.MaxSpeed").toFloat());
	manipulator->setTrackballSize(Util::Config::getValue(
			"Viewer.CameraManipulator.Sensitivity").toFloat());
	osg::DisplaySettings::instance()->setStereo(Util::Config::getValue(
			"Viewer.Display.Stereoscopic").toInt());
}

void ViewerQT::setRendering(bool flag) {
	if (flag) {
		_timer.start();
	} else {
		_timer.stop();
	}
}

void ViewerQT::paintGL() {
	frame();
	sceneGraph->update();
}
