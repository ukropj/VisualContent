/*!
 * Type.cpp
 * Projekt 3DVisual
 */
#include "Model/Type.h"

using namespace Model;

Type::Type(qlonglong id, QString name, Graph* graph,
		QMap<QString, QString>* settingsMap) {
	this->id = id;
	this->name = name;
	this->graph = graph;

	if (settingsMap == NULL) {
		settings = new QMap<QString, QString> ;
	} else {
		settings = settingsMap;
	}
	edgeType = false;
}

Type::~Type() {
	graph = NULL;
}
