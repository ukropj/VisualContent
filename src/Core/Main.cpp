#include <QApplication>
#include "Window/CoreWindow.h"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);
	app.setApplicationName("VisualContent");

	Window::CoreWindow* w = new Window::CoreWindow();
//	w->resize(1024, 768);
	w->showMaximized();

//	Model::Graph* graph = new Model::Graph("test", 1);
//	graph->addType("edge");
//	cout << "done";

	return app.exec();
}

