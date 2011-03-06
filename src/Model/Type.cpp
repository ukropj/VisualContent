/*!
 * Type.cpp
 * Projekt 3DVisual
 */
#include "Model/Type.h"
#include "Model/Graph.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"

using namespace Model;

Type::Type(qlonglong id, QString name, Graph* graph,
		QMap<QString, QString>* settingsMap) {
	this->id = id;
	this->name = name;
	this->graph = graph;

	Util::Config * appConf = Util::Config::getInstance();

	if (settingsMap == NULL) {
		settings = new QMap<QString, QString> ;

		settings->insert("scale", appConf->getValue(
				"Viewer.Textures.DefaultNodeScale"));
	} else {
		settings = settingsMap;
	}

	scale = settings->value("scale").toFloat();
	edgeType = false;
}

Type::~Type() {
	graph = NULL;
//	qDeleteAll(*settings);
}
