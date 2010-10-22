/*!
 * Type.cpp
 * Projekt 3DVisual
 */
#include "Model/Type.h"
#include "Viewer/DataHelper.h"

using namespace Model;

Type::Type(qlonglong id, QString name, Graph* graph,
		QMap<QString, QString> * settingsMap) {
	this->id = id;
	this->name = name;
	this->graph = graph;
	meta = false;

	Util::Config * appConf = Util::Config::getInstance();

	if (settingsMap == NULL) {
		settings = new QMap<QString, QString> ;

		settings->insert("scale", appConf->getValue(
				"Viewer.Textures.DefaultNodeScale"));
		settings->insert("textureFile", appConf->getValue(
				"Viewer.Textures.Node"));
		settings->insert("color.R", "1");
		settings->insert("color.G", "1");
		settings->insert("color.B", "1");
		settings->insert("color.A", "1");
	} else
		settings = settingsMap;

	texture = Vwr::DataHelper::readTextureFromFile(settings->value(
			"textureFile"));
	scale = settings->value("scale").toFloat();
}

Type::~Type(void) {
	if (settings != NULL) {
		settings->clear();
	}

	delete settings;
	settings = NULL;
}
