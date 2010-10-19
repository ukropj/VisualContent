/*!
 * Type.cpp
 * Projekt 3DVisual
 */
#include "Model/Type.h"
#include "Viewer/DataHelper.h"

using namespace Model;

Type::Type(qlonglong id, QString name, Graph* graph,
		QMap<QString, QString> * settings) {
	this->id = id;
	this->name = name;
	this->graph = graph;
	this->meta = false;

	Util::Config * appConf = Util::Config::getInstance();

	if (settings == NULL) {
		this->settings = new QMap<QString, QString> ;

		this->settings->insert("scale", appConf->getValue(
				"Viewer.Textures.DefaultNodeScale"));
		this->settings->insert("textureFile", appConf->getValue(
				"Viewer.Textures.Node"));
		this->settings->insert("color.R", "1");
		this->settings->insert("color.G", "1");
		this->settings->insert("color.B", "1");
		this->settings->insert("color.A", "1");
	} else
		this->settings = settings;

	loadConfig();
}

void Type::loadConfig() {
	typeTexture = Vwr::DataHelper::readTextureFromFile(settings->value(
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
