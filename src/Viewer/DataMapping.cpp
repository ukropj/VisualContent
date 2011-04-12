/*
 * DataMapping.cpp
 *
 *  Created on: 2.3.2011
 *      Author: jakub
 */

#include "Viewer/DataMapping.h"
#include "Model/Type.h"
#include "Util/Config.h"
#include <QDebug>

using namespace Vwr;

QColor DataMapping::allColors[] = {Qt::white, Qt::red, Qt::yellow, Qt::green, Qt::blue, Qt::magenta, Qt::cyan, Qt::gray,
		Qt::darkRed, Qt::darkYellow, Qt::darkGreen, Qt::darkBlue, Qt::darkMagenta, Qt::darkCyan, Qt::darkGray};

DataMapping::DataMapping(QList<QString> keys) {
	if (Util::Config::getValue("Viewer.Node.GenerateRandomContent") != "0") {
		contentType = RANDOM;
	} else {
		contentType = NO_CONTENT;
	}
	// prepare some default mappings
	if (keys.contains("label")) {
		insertMapping(LABEL, "label");
	} else {
		insertMapping(LABEL, "id");
	}
	if (keys.contains("color")) {
		insertMapping(COLOR, "color");
	}
	if (keys.contains("img")) {
		insertMapping(CONTENT, "img");
		contentType = IMAGE;
	} else if (keys.contains("url")) {
		insertMapping(CONTENT, "url");
		contentType = WEB;
	} else if (keys.contains("osg")) {
		insertMapping(CONTENT, "osg");
		contentType = OSG;
	}
}

QList<DataMapping::ContentType> DataMapping::getContentTypes() {
	QList<ContentType> contentTypes;
	contentTypes << RANDOM << IMAGE << TEXT << WEB << OSG;
	return contentTypes;
}

QList<DataMapping::ValueType> DataMapping::getValueTypes() {
	QList<ValueType> propTypes;
	propTypes << LABEL << COLOR << CONTENT << DIRECTION;
	return propTypes;
}

DataMapping::PropertyType DataMapping::getPropertyType(ValueType val) {
	switch (val) {
	case COLOR :
	case LABEL : return ALL;
	case CONTENT : return NODE;
	case DIRECTION : return EDGE;
	default : return ALL;
	}
}

osg::Vec4f DataMapping::getColor(QString key, PropertyType type) {
	if (key.isEmpty()) {
		return getDefaultColor(type);
	}

	if (!colors.contains(key)) {
		QColor c = allColors[colors.size() % 7];
		colors.insert(key, osg::Vec4f((float)c.red(), (float)c.green(), (float)c.blue(), (float)c.alpha()));
	}
	return colors.value(key);
}


osg::Vec4f DataMapping::getDefaultColor(PropertyType type) {
	switch (type) {
	case NODE :
	case CLUSTER :
	default :
		return Util::Config::getColorF("Viewer.Node.Color");
	case EDGE :
		return Util::Config::getColorF("Viewer.Edge.Color");
	}
}

float DataMapping::getScale(PropertyType type) {
	switch (type) {
	case NODE :
	case CLUSTER :
	default :
		return Util::Config::getValue("Viewer.Node.Scale").toFloat();
	case EDGE :
		return Util::Config::getValue("Viewer.Edge.Scale").toFloat();
	}
}
