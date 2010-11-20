/*!
 * Manager.cpp
 * Projekt 3DVisual
 */

#include "Core/IOManager.h"
#include "Model/Graph.h"
#include "Util/Config.h"

using namespace AppCore;
using namespace Model;

IOManager::IOManager() {
	//runTestCase(1);
}

IOManager::~IOManager() {
}

Graph* IOManager::loadGraph(QString filepath,
		Window::MessageWindows* msgWin) {
	// TODO presunut do samostatneho modulu pre spracovanie GraphML

	// ziskame graph element
	QDomElement rootElement;
	QDomNode graphElement;
	QFile graphMLDocument(filepath);
	if (graphMLDocument.open(QIODevice::ReadOnly)) {
		QDomDocument doc("graphMLDocument");
		if (doc.setContent(&graphMLDocument)) {
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
	}

	// ak mame rootElement tak
	if (!rootElement.isNull()) {
		QString graphname = "Graph " + rootElement.attribute("id");
		bool defaultDirection;
		if (rootElement.attribute("edgedefault") == "directed") {
			defaultDirection = true;
		} else {
			defaultDirection = false;
		}
		QDomNodeList nodes = rootElement.elementsByTagName("node");
		QDomNodeList edges = rootElement.elementsByTagName("edge");

		Graph *newGraph = this->createGraph(graphname);
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
		int colors = 6;
		// pole farieb FIXME prerobit cez nejaky QList alebo nieco take, oddelit farby hran od farieb uzlov
		qint8 nodeTypeSettings[6][4] = { { 0, 1, 0, 1 }, { 0, 1, 1, 1 }, { 1,
				0, 0, 1 }, { 1, 0, 1, 1 }, { 1, 1, 0, 1 }, { 1, 1, 1, 1 }, };
		qint8 iColor = 0;

		// vypis % pri nacitavani grafu
		int step = 0;
		int stepLength = (int) (nodes.count() + edges.count()) / 100;
		if (stepLength == 0) {
			// zadame defaultnu hodnotu, aby to nezlyhavalo
			stepLength = 50;
		}

		// prechadzame uzlami
		for (unsigned int i = 0; i < nodes.length(); i++) {

			if (i % stepLength == 0) {
				msgWin->setProgressBarValue(step++);
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
						node = newGraph->addNode(name, nodeType);
					else
						node = newGraph->addNode(name, newNodeType);
					readNodes->insert(nameId, node);
				}
			}
		}

		iColor = 0;

		// prechadzame hranami
		for (uint i = 0; i < edges.length(); i++) {

			if (i % stepLength == 0) {
				msgWin->setProgressBarValue(step++);
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

					newGraph->addEdge(sourceId + targetId, readNodes->value(
							sourceId), readNodes->value(targetId), newEdgeType,
							directed);
				}
			}
		}

		return newGraph;
	}

	return NULL;
}

void IOManager::exportGraph(Graph* graph, QString filepath) {
	// TODO export to GraphML
}

Graph* IOManager::createGraph(QString graphname) {
	return new Graph(graphname, 0);
}

Graph* IOManager::simpleGraph() {
	Graph *newGraph = new Graph("simple", 0);
	Type *type = newGraph->addType("default");
	Type *type2 = newGraph->addType("default2");

	Node *u1 = newGraph->addNode("u1", type);
	Node *u2 = newGraph->addNode("u2", type);
	Node *u3 = newGraph->addNode("u3", type);

	newGraph->addEdge("e1", u1, u2, type2, false);
	newGraph->addEdge("e2", u1, u3, type2, false);
	newGraph->addEdge("e3", u2, u3, type2, false);

	return newGraph;
}
