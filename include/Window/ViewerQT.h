/**
 *  ViewerQT.h
 *  Projekt 3DVisual
 */
#ifndef Window_VIEWER_QT_DEF
#define Window_VIEWER_QT_DEF 1

#include "Window/AdapterWidget.h"
#include <osgViewer/ViewerBase>

#include "Util/Config.h"
#include "Util/CameraHelper.h"
#include "Viewer/PickHandler.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/SceneGraph.h"

/*!
 * \brief
 * Widget, ktory zobrazuje OSG sceny.
 *
 * \author
 * Michal Paprcka
 * \version
 * 1.0
 * \date
 * 1.12.2009
 */

namespace Window {
/**
 *  \class ViewerQT
 *  \brief
 *  \author Adam Pazitnaj
 *  \date 29. 4. 2010
 */
class ViewerQT: public osgViewer::Viewer, public AdapterWidget {
public:

	ViewerQT(Vwr::SceneGraph* sceneGraph, QWidget * parent = 0,
			const char * name = 0, const QGLWidget * shareWidget = 0,
			WindowFlags f = 0) :
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

	~ViewerQT() {
		_timer.stop();

		removeEventHandler(pickHandler);
		pickHandler = NULL;
		delete sceneGraph;
		qDebug() << "ViewerQT deleted";
	}

	Vwr::PickHandler* getPickHandler() const {
		return pickHandler;
	}
	Vwr::CameraManipulator* getCameraManipulator() const {
		return manipulator;
	}

	void reloadConfig() {
		manipulator->setMaxSpeed(Util::Config::getValue(
				"Viewer.CameraManipulator.MaxSpeed").toFloat());
		manipulator->setTrackballSize(Util::Config::getValue(
				"Viewer.CameraManipulator.Sensitivity").toFloat());

		osg::DisplaySettings::instance()->setStereo(Util::Config::getValue(
				"Viewer.Display.Stereoscopic").toInt());

	}

	void setRendering(bool flag) {
		if (flag) {
			_timer.start();
		} else {
			_timer.stop();
		}
	}

protected:
	QTimer _timer;
	Vwr::SceneGraph* sceneGraph;

	virtual void paintGL() {
		frame();
		sceneGraph->update();
	}

private:
	Vwr::CameraManipulator* manipulator;
	Vwr::PickHandler* pickHandler;
};
}

#endif

