/**
 *  ViewerQT.h
 *  Projekt 3DVisual
 */
#ifndef Window_VIEWER_QT_DEF
#define Window_VIEWER_QT_DEF 1

#include "Window/AdapterWidget.h"
#include <osgViewer/ViewerBase>

#include "Util/Config.h"
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
	/*!
	 * \param parent Rodic widgetu.
	 * \param name Meno widgetu.
	 * \param shareWidget Zdielanie widgetu.
	 * \param f Znacky pre vytvaranie okna.
	 * \param cg CoreGraf, ktory sa bude vykreslovat.
	 * Vytvori widget, ktory dokaze zobrazit OSG grafy.
	 */
	ViewerQT(Vwr::SceneGraph* sceneGraph, QWidget * parent = 0,
			const char * name = 0, const QGLWidget * shareWidget = 0,
			WindowFlags f = 0) :
		AdapterWidget(parent, name, shareWidget, f) {
		this->sceneGr = sceneGraph;
		sceneGr->setCamera(this->getCamera());

		osg::DisplaySettings::instance()->setStereo(Util::Config::getValue(
				"Viewer.Display.Stereoscopic").toInt());
		osg::DisplaySettings::instance()->setStereoMode(
				osg::DisplaySettings::ANAGLYPHIC);

		getCamera()->setViewport(new osg::Viewport(0, 0, width(), height()));
		getCamera()->setProjectionMatrixAsPerspective(60,
				static_cast<double> (width()) / static_cast<double> (height()),
				0.01,
				Util::Config::getValue("Viewer.Display.ViewDistance").toFloat());
		getCamera()->setGraphicsContext(getGraphicsWindow());
		getCamera()->setComputeNearFarMode(
				osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

		manipulator = new Vwr::CameraManipulator();
		pickHandler = new Vwr::PickHandler(manipulator, sceneGr);

		addEventHandler(new osgViewer::StatsHandler);
		addEventHandler(pickHandler);
		setCameraManipulator(manipulator);

		getCamera()->setClearColor(osg::Vec4(0, 0, 0, 1));
		getCamera()->setViewMatrixAsLookAt(osg::Vec3d(-10, 0, 0), osg::Vec3d(0,
				0, 0), osg::Vec3d(0, 1, 0));

		setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

		connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));

		_timer.start(10);
		setSceneData(sceneGr->getRoot()); // TODO move to ViewerQT
	}

	~ViewerQT() {
		_timer.stop();
	}

	/**
	 *  \fn inline public constant  getPickHandler
	 *  \brief Returns pick handler
	 *  \return Vwr::PickHandler * pick handler
	 */
	Vwr::PickHandler * getPickHandler() const {
		return pickHandler;
	}

	/**
	 *  \fn inline public constant  getCameraManipulator
	 *  \brief Returns camera manipulator
	 *  \return Vwr::CameraManipulator * camera manipulator
	 */
	Vwr::CameraManipulator * getCameraManipulator() const {
		return manipulator;
	}

	/**
	 *  \fn inline public  reloadConfig
	 *  \brief Reloads configuration
	 */
	void reloadConfig() {
		manipulator->setMaxSpeed(Util::Config::getValue(
				"Viewer.CameraManipulator.MaxSpeed").toFloat());
		manipulator->setTrackballSize(Util::Config::getValue(
				"Viewer.CameraManipulator.Sensitivity").toFloat());

		osg::DisplaySettings::instance()->setStereo(Util::Config::getValue(
				"Viewer.Display.Stereoscopic").toInt());
	}

protected:

	/**
	 *  QTimer _timer
	 *  \brief
	 */
	QTimer _timer;

	/**
	 *  Vwr::CoreGraph * cg
	 *  \brief core graph
	 */
	Vwr::SceneGraph* sceneGr;

	/**
	 *  \fn inline protected virtual  paintGL
	 *  \brief Paints new frame
	 */
	virtual void paintGL() {
		frame();
		sceneGr->update();
	}

private:

	/**
	 *  Vwr::CameraManipulator * manipulator
	 *  \brief camera manipulator
	 */
	Vwr::CameraManipulator* manipulator;

	/**
	 *  Vwr::PickHandler * pickHandler
	 *  \brief pick handler
	 */
	Vwr::PickHandler* pickHandler;

};
}

#endif

