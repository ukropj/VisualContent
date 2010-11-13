/*!
 * IOManager.h
 * Projekt 3DVisual
 */
#ifndef Manager_MANAGER_DEF
#define Manager_MANAGER_DEF 1

#include <vector>
#include <QMap>
#include <QString>
#include <QtXml/QDomElement>
#include <QFile>

#include "Window/MessageWindows.h"

namespace Model {
class Graph;
}

namespace AppCore {

class IOManager {
public:
	IOManager();
	~IOManager();

	/**
	 * \fn loadGraph
	 * \brief Loads graph from GraphML file.
	 */
	Model::Graph* loadGraph(QString filepath, Window::MessageWindows* msgWin);

	/**
	 * \fn simpleGraph
	 * \brief Creates simple triangle graph. Method was created as example of using API for creating graphs.
	 */
	Model::Graph* simpleGraph();

	/**
	 * \fn exportGraph
	 * \brief Exports graph into file (not yet implemented).
	 */
	void exportGraph(Model::Graph* graph, QString filepath);

	/**
	 * \fn createGraph
	 * \brief Creates empty graph, puts it into the working graphs and returns it.
	 */
	Model::Graph* createGraph(QString graphname = 0);

private:
	/**
	 *  \fn private runTestCase(qint32 action)
	 *  \brief Runs one of predefined Graph tests
	 *  \param action  code of the test
	 */
	void runTestCase(qint32 action);
};
}

#endif
