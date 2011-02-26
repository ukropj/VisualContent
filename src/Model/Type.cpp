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
		settings->insert("textureFile", appConf->getValue(
				"Viewer.Textures.Node"));
		osg::Vec4f color = appConf->getColorF("Viewer.Node.Color");
		settings->insert("color.R", QString::number(color.x()));
		settings->insert("color.G", QString::number(color.y()));
		settings->insert("color.B", QString::number(color.z()));
		settings->insert("color.A", QString::number(color.w()));
	} else {
		settings = settingsMap;
	}

	texture = Util::TextureWrapper::readTextureFromFile(settings->value(
			"textureFile"));
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
