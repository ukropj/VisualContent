/*!
 * Core.h
 * Projekt 3DVisual
 */
#ifndef Core_CORE_DEF
#define Core_CORE_DEF 1

#include <QMap>
#include <QString>
#include <QApplication>

#include "Model/Graph.h"
#include "Model/FRAlgorithm.h"
#include "Window/CoreWindow.h"
#include "Viewer/CoreGraph.h"
#include "Core/IOManager.h"


namespace Window {
class CoreWindow;
}

namespace AppCore {

class IOManager;

/**
 * \class Core
 * \brief Core control whole application
 *
 * Class is implemented as singleton. Core provides functionality to work with layout threads, application windows and core graph.
 *
 * \author Pavol Perdik
 * \date 8.5.2010
 */
class Main {
public:
	/**
	 *  \fn private constructor  GraphManager(QApplication * app)
	 *  \brief
	 *  \param app
	 */
	Main(int argc, char *argv[]);

	~Main();

private:
	/**
	 *  Window::CoreWindow * cw
	 *  \brief instance of CoreWindow
	 */
	Window::CoreWindow * cw;

	/**
	 *  Vwr::CoreGraph * cg
	 *  \brief instance of CoreGraph
	 */
	Vwr::CoreGraph * cg;

	IOManager *manager;
};

}

#endif //Core_CORE_DEF
