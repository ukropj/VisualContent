/*!
 * Manager.cpp
 * Projekt 3DVisual
 */

#include "Core/IOManager.h"
#include "Model/Graph.h"
#include "Model/Node.h"
#include "Model/Type.h"
#include "Model/Edge.h"
#include "Util/Config.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>

using namespace AppCore;
using namespace Model;

IOManager::IOManager() {
	graph = NULL;
	progress = NULL;
	defaultDirection = false;
}

IOManager::~IOManager() {
}

// Obsolete
Graph* IOManager::loadGraphOld(QIODevice* device, QProgressDialog* progressBar) {
	QTime t;
	t.start();

	// ziskame graph element
	QDomElement rootElement;
	QDomNode graphElement;
	QDomDocument doc("graphMLDocument");
	if (doc.setContent(device)) {
		QDomElement docElem = doc.documentElement();
		if (!docElem.isNull() && docElem.nodeName() == "graphml") {
			QDomNodeList graphNodes = docElem.elementsByTagName("graph");
			if (graphNodes.length() > 0) {
				graphElement = graphNodes.item(0);
				if (!graphElement.isNull() && graphElement.parentNode()
						== docElem && graphElement.isElement()) {
					rootElement = graphElement.toElement();
				}
			}
		}
	}
	// ak mame rootElement tak
	if (!rootElement.isNull()) {
		QString graphname = rootElement.attribute("id");
		bool defaultDirection;
		if (rootElement.attribute("edgedefault") == "directed") {
			defaultDirection = true;
		} else {
			defaultDirection = false;
		}
		QDomNodeList nodes = rootElement.elementsByTagName("node");
		QDomNodeList edges = rootElement.elementsByTagName("edge");
		qDebug() << "Old load element count" << " " << t.elapsed();

		Graph *newGraph = new Graph(graphname, 0);
		if (newGraph == NULL)
			return NULL;

		// ziskame pristup ku nastaveniam
		QString edgeTypeAttribute = Util::Config::getValue(
				"GraphMLParser.edgeTypeAttribute");
		QString nodeTypeAttribute = Util::Config::getValue(
				"GraphMLParser.nodeTypeAttribute");

		// pridavame default typy
		Type *edgeType = newGraph->addType("edge");
		Type *nodeType = newGraph->addType("node");

		QMap<QString, Node* >* readNodes = new QMap<QString,
				Node* > ();
		// skusal som aj cez QList, ale vobec mi to neslo, tak som to spravil len takto jednoducho cez pole
		int colors = 7;
		// pole farieb FIXME prerobit cez nejaky QList alebo nieco take, oddelit farby hran od farieb uzlov
		qint8 nodeTypeSettings[7][4] = { { 1, 0, 0, 1 }, { 0, 0, 1, 1 }, { 0, 1, 1, 1 }, { 0,
				1, 0, 1 }, { 1, 0, 1, 1 }, { 1, 1, 0, 1 }, { 0, 0, 0, 1 }, };
		qint8 iColor = 0;

		// vypis % pri nacitavani grafu
		progressBar->reset();
		progressBar->setLabelText("Loading file...");
		progressBar->setMaximum((nodes.count() + edges.count()) * 2);
		int step = 0;
		// prechadzame uzlami
		for (unsigned int i = 0; i < nodes.length(); i++) {
			progressBar->setValue(step++);
			if (progressBar->wasCanceled()) {
				qDebug() << "cancelled";
				return NULL;
			}

			QDomNode nodeNode = nodes.item(i);
			if (!nodeNode.isNull() && nodeNode.isElement()) {
				QDomElement nodeElement = nodeNode.toElement();
				if (nodeElement.parentNode() == graphElement) {
					QString nameId = nodeElement.attribute("id");
					QString name = NULL;
					// pozerame sa na data ktore nesie
					Type *newNodeType;
					newNodeType = NULL;
					QDomNodeList nodeDataList = nodeElement.elementsByTagName(
							"data");
					for (unsigned int j = 0; j < nodeDataList.length(); j++) {
						QDomNode nodeData = nodeDataList.item(j);
						if (!nodeData.isNull() && nodeData.isElement()) {
							QDomElement nodeDataElement = nodeData.toElement();
							QString dataName = nodeDataElement.attribute("key");
							QString dataValue = nodeDataElement.text();
							// rozpoznavame typy
							if (dataName == nodeTypeAttribute) {
								// overime ci uz dany typ existuje v grafe
								QList<Type*> types =
										newGraph->getTypesByName(dataValue);
								if (types.isEmpty()) {
									QMap<QString, QString> *settings =
											new QMap<QString, QString> ;

									settings->insert(
											"color.R",
											QString::number(
													nodeTypeSettings[iColor][0]));
									settings->insert(
											"color.G",
											QString::number(
													nodeTypeSettings[iColor][1]));
									settings->insert(
											"color.B",
											QString::number(
													nodeTypeSettings[iColor][2]));
									settings->insert(
											"color.A",
											QString::number(
													nodeTypeSettings[iColor][3]));
									settings->insert(
											"scale",
											Util::Config::getValue(
													"Viewer.Textures.DefaultNodeScale"));
									settings->insert("textureFile",
											Util::Config::getValue(
													"Viewer.Textures.Node"));

									newNodeType = newGraph->addType(dataValue,
											settings);

									if (iColor == colors) {
										iColor = 0;
									} else {
										iColor++;
									}
								} else {
									newNodeType = types.first();
								}

							} else {
								// kazde dalsie data nacitame do nosica dat - Node.name
								// FIXME potom prerobit cez Adamove Node.settings
								if (name == NULL) {
									name = dataName + ":" + dataValue;
								} else {
									name += " | " + dataName + ":" + dataValue;
								}
							}
						}
					}

					// ak sme nenasli name, tak ako name pouzijeme aspon ID
					if (name == NULL) {
						name = nameId;
					}

					// ak nebol najdeny ziaden typ, tak pouzijeme defaultny typ
					Node* node;
					if (newNodeType == NULL)
						node = newGraph->addNode(nodeType);
					else
						node = newGraph->addNode(newNodeType);
					readNodes->insert(nameId, node);
				}
			}
		}
		iColor = 0;

		// prechadzame hranami
		for (uint i = 0; i < edges.length(); i++) {
			progressBar->setValue(step++);
			if (progressBar->wasCanceled()) {
				qDebug() << "cancelled";
				return NULL;
			}

			QDomNode edgeNode = edges.item(i);

			if (!edgeNode.isNull() && edgeNode.isElement()) {
				QDomElement edgeElement = edgeNode.toElement();
				if (edgeElement.parentNode() == rootElement) {
					QString sourceId = edgeElement.attribute("source");
					QString targetId = edgeElement.attribute("target");
					QString direction = NULL;
					bool directed = false;
					direction = edgeElement.attribute("directed");
					if (direction == NULL) {
						directed = defaultDirection;
						if (directed)
							direction = "_directed";
						else
							direction = "";
					} else {
						if (direction == "true") {
							direction = "_directed";
							directed = true;
						} else {
							direction = "";
							directed = false;
						}
					}

					// pozerame sa na data ktore hrana nesie
					Type *newEdgeType;
					newEdgeType = NULL;
					QDomNodeList edgeDataList = edgeElement.elementsByTagName(
							"data");
					for (unsigned int j = 0; j < edgeDataList.length(); j++) {
						QDomNode edgeData = edgeDataList.item(j);
						if (!edgeData.isNull() && edgeData.isElement()) {
							QDomElement edgeDataElement = edgeData.toElement();
							QString dataName = edgeDataElement.attribute("key");
							QString dataValue = edgeDataElement.text();
							// rozpoznavame typy deklarovane atributom relation
							if (dataName == edgeTypeAttribute) {
								// overime ci uz dany typ existuje v grafe
								QList<Type*> types =
										newGraph->getTypesByName(dataValue
												+ direction);
								if (types.isEmpty()) {
									QMap<QString, QString> *settings =
											new QMap<QString, QString> ;

									// FIXME spravit tak, aby to rotovalo po tom poli - palo az to budes prerabat tak pre hrany pouzi ine pole, take co ma alfu na 0.5.. a to sa tyka aj uzlov s defaultnym typom
									settings->insert(
											"color.R",
											QString::number(
													nodeTypeSettings[iColor][0]));
									settings->insert(
											"color.G",
											QString::number(
													nodeTypeSettings[iColor][1]));
									settings->insert(
											"color.B",
											QString::number(
													nodeTypeSettings[iColor][2]));
									settings->insert(
											"color.A",
											QString::number(
													nodeTypeSettings[iColor][3]));
									settings->insert(
											"scale",
											Util::Config::getInstance()->getValue(
													"Viewer.Textures.DefaultNodeScale"));

									if (!directed)
										settings->insert(
												"textureFile",
												Util::Config::getValue(
														"Viewer.Textures.Edge"));
									else {
										settings->insert(
												"textureFile",
												Util::Config::getValue(
														"Viewer.Textures.OrientedEdgePrefix"));
										settings->insert(
												"textureFile",
												Util::Config::getValue(
														"Viewer.Textures.OrientedEdgeSuffix"));
									}

									newEdgeType = newGraph->addType(dataValue
											+ direction, settings);

									if (iColor == colors) {
										iColor = 0;
									} else {
										iColor++;
									}
								} else {
									newEdgeType = types.first();
								}

							} else {
								// kazde dalsie data nacitame do nosica dat - Edge.name
								// FIXME potom prerobit cez Adamove Node.settings
							}
						}
					}

					// ak nebol najdeny typ, tak pouzijeme defaulty
					if (newEdgeType == NULL)
						newEdgeType = edgeType;
					newGraph->addEdge(readNodes->value(sourceId),
							readNodes->value(targetId), newEdgeType);
				}
			}
		}
		qDebug() << "Old load " << newGraph->getName() << " " << t.elapsed();
		return newGraph;
	}
	return NULL;
}


Graph* IOManager::loadGraph(QIODevice* device, QProgressDialog* progressBar) {
	graph = NULL;
	QDomDocument doc("graphMLDocument");
	doc.setContent(device);
	int elementCount = 0;
	elementCount += doc.elementsByTagName("node").size();
	elementCount += doc.elementsByTagName("edge").size();
	device->reset();

	progress = progressBar;
	progress->reset();
	progress->setLabelText("Loading file...");
	progress->setMaximum(elementCount + 1);

	QTime t;
	t.start();
	xml.setDevice(device);
	if (xml.readNextStartElement()) {
		if (xml.name() == "graphml") {
			readGraphML();
		}
	}
	qDebug() << "New load "<<graph->getName() << " " << t.elapsed();
	if (progress->wasCanceled())
		return NULL;
	return graph;
}

void IOManager::readGraphML() {
	Q_ASSERT(xml.isStartElement() && xml.name() == "graphml");

	graph = new Graph();
	nodeType = graph->addType("node");
	nodeType->insertMapping(Type::LABEL, "id");
	edgeType = graph->addType("edge");
	edgeType->insertMapping(Type::LABEL, "id");
	edgeType->insertMapping(Type::IS_ORIENTED, "oriented");

	while (xml.readNextStartElement()) {
		if (xml.name() == "key") {
			readKey();
		} else if (xml.name() == "graph") {
			readGraph();
		} else {
			qWarning() << "Element skipped: " << xml.name();
			xml.skipCurrentElement();
		}
	}
}

void IOManager::readKey() {
	Q_ASSERT(xml.isStartElement() && xml.name() == "key");
	QString name = xml.attributes().value("id").toString();
	QString target = xml.attributes().value("for").toString();

	QString defaultVal = "";
	while (xml.readNextStartElement()) {
		if (xml.name() == "default") {
			defaultVal = xml.readElementText();
		} else {
			qWarning() << "Element skipped: " << xml.name();
			xml.skipCurrentElement();
		}
	}

	QStringList targets = target.split(QRegExp("[, ]+"));
	foreach(QString t, targets) {
		if (t == "edge") {
			edgeType->addKey(name, defaultVal);
		} else if (t == "node") {
			nodeType->addKey(name, defaultVal);
		} else if (t == "all") {
			nodeType->addKey(name, defaultVal);
			edgeType->addKey(name, defaultVal);
		}
	}
}

void IOManager::readGraph() {
	Q_ASSERT(xml.isStartElement() && xml.name() == "graph");

	QString name = xml.attributes().value("id").toString();
	graph->setName(name);
	readNodes.clear();

	defaultDirection = xml.attributes().value("edgedefault") == "directed";

	int step = 0;

	while (xml.readNextStartElement()) {
		if (progress->wasCanceled())
			return;
		if (xml.name() == "node") {
			progress->setValue(step++);
			readNode();
		} else if (xml.name() == "edge") {
			progress->setValue(step++);
			readEdge();
		} else {
			qWarning() << "Element skipped: " << xml.name();
			xml.skipCurrentElement();
		}
	}
}

void IOManager::readNode() {
	Q_ASSERT(xml.isStartElement() && xml.name() == "node");

	QString id = xml.attributes().value("id").toString();
	Data* data = readData(nodeType);
	data->insert("id", id);
	Node* node = graph->addNode(nodeType, data);
	readNodes.insert(id, node->getId());
//	qDebug() << "n "<< id;
}


void IOManager::readEdge() {
	Q_ASSERT(xml.isStartElement() && xml.name() == "edge");

	QString directed = xml.attributes().value("directed").toString();
	QString sourceId = xml.attributes().value("source").toString();
	QString targetId = xml.attributes().value("target").toString();
	Data* data = readData(edgeType);
	data->insert("id", sourceId + targetId);
	data->insert("oriented", directed == "true" || defaultDirection ? "true" : "false");
	Edge* edge = graph->addEdge(readNodes.value(sourceId), readNodes.value(targetId),
			edgeType, data);
//	qDebug() << data->value("id");
}

Data* IOManager::readData(Type* type) {
	Q_ASSERT(xml.isStartElement() && (xml.name() == "node" || xml.name() == "edge"));
	Data* data = new Data();
	foreach (QString key, type->getKeys()) {
		QString defaultVal = type->getDefaultValue(key);
		if (!defaultVal.isEmpty()) {
			data->insert(key, defaultVal);
		}
	}

	while (xml.readNextStartElement()) {
		if (xml.name() == "data") {
			QString key = xml.attributes().value("key").toString();
			QString value = xml.readElementText();
			data->insert(key, value);
		} else {
			qWarning() << "Element skipped: " << xml.name();
			xml.skipCurrentElement();
		}
	}
	return data;
}
