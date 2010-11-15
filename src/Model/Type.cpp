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
		settings->insert("color.R", "1.0");
		settings->insert("color.G", "1.0");
		settings->insert("color.B", "1.0");
		settings->insert("color.A", "1.0");
	} else
		settings = settingsMap;

	texture = Util::TextureWrapper::readTextureFromFile(settings->value(
			"textureFile"));
	devil = Util::TextureWrapper::readTextureFromFile("img/devil.jpg");
	scale = settings->value("scale").toFloat();
}

Type::~Type() {
	graph = NULL;
//	qDeleteAll(*settings);
}

osg::Vec4f Type::getColor() const {
	float r = getValue("color.R").toFloat();
	float g = getValue("color.G").toFloat();
	float b = getValue("color.B").toFloat();
	float a = getValue("color.A").toFloat();
	return osg::Vec4(r, g, b, a);
}
