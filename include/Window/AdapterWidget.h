#ifndef Window_ADAPTER_WIDGET_H
#define Window_ADAPTER_WIDGET_H 1

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QMdiArea>

using Qt::WindowFlags;

#include <iostream>


namespace Window
{
	/**
	*  \class AdapterWidget
	*  \brief Adapter
	*  \author Adam Pazitnaj 
	*/
	class AdapterWidget : public QGLWidget
	{

	public:

		AdapterWidget( QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0 );
		virtual ~AdapterWidget() {}
		osgViewer::GraphicsWindow* getGraphicsWindow() { return _gw.get(); }
		const osgViewer::GraphicsWindow* getGraphicsWindow() const { return _gw.get(); }

	protected:
		void init();
		virtual void resizeGL( int width, int height );
		virtual void keyPressEvent( QKeyEvent* event );
		virtual void keyReleaseEvent( QKeyEvent* event );
		virtual void mousePressEvent( QMouseEvent* event );
		virtual void mouseDoubleClickEvent ( QMouseEvent * event );
		virtual void mouseReleaseEvent( QMouseEvent* event );
		virtual void mouseMoveEvent( QMouseEvent* event );
 		virtual void wheelEvent(QWheelEvent *event);
		osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;

	};
}

#endif
