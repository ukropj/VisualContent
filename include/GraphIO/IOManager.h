#ifndef Manager_MANAGER_DEF
#define Manager_MANAGER_DEF 1

#include <vector>
#include <QMap>
#include <QTime>
#include <QString>
#include <QtXml/QDomElement>
#include <QFile>
#include <QXmlStreamReader>
#include <QProgressDialog>

namespace Model {
class Graph;
class Node;
class Type;
}

/// IO classes
namespace GraphIO {

/// GraphML parser
/**
 * \author Jakub Ukrop
 */
class IOManager {
public:
	IOManager();
	~IOManager();
	Model::Graph* loadGraph(QIODevice* device, QProgressDialog* pd);

private:
	void readGraphML();
	void readKey();
	void readGraph();
	void readNode();
	void readEdge();
	QMap<QString, QString>* readData(Model::Type* type);

	QXmlStreamReader xml;
	Model::Graph* graph;
	Model::Type* nodeType;
	Model::Type* edgeType;
	QMap<QString, qlonglong > readNodes;

	bool defaultDirection;

	int step;
	QProgressDialog* pd;
};
}

#endif
