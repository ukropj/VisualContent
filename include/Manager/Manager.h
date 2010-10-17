/*!
 * Manager.h
 * Projekt 3DVisual
 */
#ifndef Manager_MANAGER_DEF
#define Manager_MANAGER_DEF 1

#include <vector>
#include <QMap>
#include <QString>
#include <QtXml/QDomElement>
#include <QFile>

#include "Core/Core.h"
#include "Data/Graph.h"
#include "Layout/FRAlgorithm.h"
#include "Layout/LayoutThread.h"
#include "QOSG/CoreWindow.h"
#include "Viewer/CoreGraph.h"
#include "QOSG/MessageWindows.h"

namespace Manager
{
    /**
     * \class Manager
     * \brief Manager provides functionality to manage graphs (loading, creating, holding, editing and deleting).
     *
     * Class is implemented as singleton.
     *
     * \author Pavol Perdik
     * \date 25.4.2010
     */
    class GraphManager
	{
	public:
            ~GraphManager();

            /**
             * \fn loadGraph
             * \brief Loads graph from GraphML file.
             */
            Data::Graph* loadGraph(QString filepath);

            /**
             * \fn simpleGraph
             * \brief Creates simple triangle graph. Method was created as example of using API for creating graphs.
             */
            Data::Graph* simpleGraph();

            /**
             * \fn saveGraph
             * \brief Saves graph.
             */
            void saveGraph(Data::Graph* graph);

            /**
             * \fn exportGraph
             * \brief Exports graph into file (not yet implemented).
             */
            void exportGraph(Data::Graph* graph, QString filepath);

            /**
             * \fn createGraph
             * \brief Creates empty graph, puts it into the working graphs and returns it.
             */
            Data::Graph* createGraph(QString graphname);

            /**
             * \fn closeGraph
             * \brief Removes graph from working graphs. Do NOT remove it from DB.
             */
            void closeGraph(Data::Graph* graph);

            /**
             * \fn getActiveGraph
             * \brief Returns active graphs. (imeplemnted as workaround for working with only one graph for now).
             */
            Data::Graph* getActiveGraph() { return activeGraph; }

            /**
             * \fn getInstance
             * \brief Returns instance of class.
             */
            static Manager::GraphManager* getInstance();
	private:
                /**
                *  \fn private runTestCase(qint32 action)
                *  \brief Runs one of predefined Graph tests
                *  \param action  code of the test
                */
                void runTestCase(qint32 action);


                /**
                *  \fn private  emptyGraph
                *  \brief returns empty graph
                *  \deprecated
                *  \return Data::Graph *
                */
                Data::Graph* emptyGraph();

                /**
                *  \fn private constructor GraphManager
                *  \brief private constructor
                *  \param  app 
                */
                GraphManager();


                /**
                *  Manager::GraphManager * manager
                *  \brief singleton object
                */
                static GraphManager * manager;

               /**
                *  Data::Graph * activeGraph
                *  \brief active graph
                */
                Data::Graph *activeGraph;
	};
}

#endif
