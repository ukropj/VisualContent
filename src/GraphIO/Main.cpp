/**
* @mainpage DataViz
*
* 3D graph visualisation system designed to present visual content in nodes of graph.
*
* @author Jakub Ukrop
*
* https://github.com/ukropj/VisualContent
*/

#include <QApplication>
#include "Window/CoreWindow.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	app.setOrganizationName("FIIT STU");
	app.setApplicationName("DataViz");

	Window::CoreWindow* w = new Window::CoreWindow();
	w->showMaximized();
	w->setFocus();

	return app.exec();
}
