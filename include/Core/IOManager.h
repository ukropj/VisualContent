/*!
 * IOManager.h
 * Projekt 3DVisual
 */
#ifndef Manager_MANAGER_DEF
#define Manager_MANAGER_DEF 1

#include <vector>
#include <QMap>
#include <QTime>
#include <QString>
#include <QtXml/QDomElement>
#include <QFile>
#include <QProgressDialog>
#include <QXmlStreamReader>

namespace Model {
class Graph;
class Node;
class Type;
class Data;
}

namespace AppCore {

class IOManager {
public:
	IOManager();
	~IOManager();

	Model::Graph* loadGraph(QIODevice* device, QProgressDialog* progressBar);
	Model::Graph* loadGraphOld(QIODevice* device, QProgressDialog* progressBar);

private:
	void readGraphML();
	void readKey();
	void readGraph();
	void readNode();
	void readEdge();
	Model::Data* readData(Model::Type* type);

	QXmlStreamReader xml;
	QProgressDialog* progress;
	Model::Graph* graph;
	Model::Type* nodeType;
	Model::Type* edgeType;
	QMap<QString, qlonglong > readNodes;

	bool defaultDirection;
};
}

#endif
