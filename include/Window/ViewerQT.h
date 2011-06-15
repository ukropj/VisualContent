#ifndef Window_VIEWER_QT_DEF
#define Window_VIEWER_QT_DEF 1

#include "Window/AdapterWidget.h"

namespace Vwr {
	class SceneGraph;
	class PickHandler;
	class CameraManipulator;
}

namespace Window {
/*!
 * \brief Widget used to display OSG scene.
 *
 * \author Michal Paprcka
 * \author Adam Pazitnaj
 */
class ViewerQT: public osgViewer::Viewer, public AdapterWidget {
public:

	ViewerQT(Vwr::SceneGraph* sceneGraph, QWidget* parent = 0,
			const char* name = 0, const QGLWidget* shareWidget = 0,
			WindowFlags f = 0);
	~ViewerQT();

	Vwr::PickHandler* getPickHandler() const;
	Vwr::CameraManipulator* getCameraManipulator() const;

	void reloadConfig();
	void setRendering(bool flag);

protected:
	QTimer _timer;
	Vwr::SceneGraph* sceneGraph;

	virtual void paintGL();

private:
	Vwr::CameraManipulator* manipulator;
	Vwr::PickHandler* pickHandler;
};
}

#endif
