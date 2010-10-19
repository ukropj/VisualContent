/*!
 * Core.cpp
 * Projekt 3DVisual
 */

#include "Core/Main.h"

using namespace AppCore;

int main(int argc, char *argv[]) {
	Main(argc, argv);
}

Main::Main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	app.setApplicationName("VisualContent");

	manager = new IOManager();
	this->cg = new Vwr::CoreGraph();
	this->cw = new Window::CoreWindow(manager, this->cg, &app);
	this->cw->resize(1024, 768);
	this->cw->show();

	app.exec();
}

Main::~Main() {
}
