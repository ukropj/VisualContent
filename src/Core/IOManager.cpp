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

using namespace AppCore;
using namespace Model;

IOManager::IOManager() {
	graph = NULL;
	defaultDirection = false;
}

IOManager::~IOManager() {
}

Graph* IOManager::loadGraph(QIODevice* device, QProgressDialog* pd) {
	graph = NULL;
	QDomDocument doc("graphMLDocument");
	doc.setContent(device);
	int elementCount = 0;
	elementCount += doc.elementsByTagName("node").size();
	elementCount += doc.elementsByTagName("edge").size();

	this->pd = pd;
	this->pd->reset();
	this->pd->setLabelText("Parsing file ...");
	this->pd->setMaximum(elementCount);
	step = 0;
	device->reset();

	xml.setDevice(device);
	if (xml.readNextStartElement()) {
		if (xml.name() == "graphml") {
			readGraphML();
		}
	}
	if (pd->wasCanceled())
		return NULL;

	return graph;
}

void IOManager::readGraphML() {
	Q_ASSERT(xml.isStartElement() && xml.name() == "graphml");

	graph = new Graph();
	nodeType = graph->addType("node");
	nodeType->addKey("id", "");
	edgeType = graph->addType("edge");
	edgeType->addKey("id", "");
	edgeType->addKey("directed", "false");
	edgeType->setEdgeType(true);

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

	while (xml.readNextStartElement()) {
		if (pd->wasCanceled())
			return;
		if (xml.name() == "node") {
			readNode();
			pd->setValue(++step);
		} else if (xml.name() == "edge") {
			readEdge();
			pd->setValue(++step);
		} else {
			qWarning() << "Element skipped: " << xml.name();
			xml.skipCurrentElement();
		}
	}
}

void IOManager::readNode() {
	Q_ASSERT(xml.isStartElement() && xml.name() == "node");

	QString id = xml.attributes().value("id").toString();
	QMap<QString, QString>* data = readData(nodeType);
	Node* node = graph->addNode(nodeType, data);
	readNodes.insert(id, node->getId());
//	qDebug() << "n "<< id;
}


void IOManager::readEdge() {
	Q_ASSERT(xml.isStartElement() && xml.name() == "edge");

	QString directed = xml.attributes().value("directed").toString();
	QString sourceId = xml.attributes().value("source").toString();
	QString targetId = xml.attributes().value("target").toString();
	QMap<QString, QString>* data = readData(edgeType);
	data->insert("directed", directed == "true" || defaultDirection ? "true" : "false");
	Edge* edge = graph->addEdge(readNodes.value(sourceId), readNodes.value(targetId),
			edgeType, data);
//	qDebug() << data->value("id");
}

QMap<QString, QString>* IOManager::readData(Type* type) {
	Q_ASSERT(xml.isStartElement() && (xml.name() == "node" || xml.name() == "edge"));
	QMap<QString, QString>* data = new QMap<QString, QString>();
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
