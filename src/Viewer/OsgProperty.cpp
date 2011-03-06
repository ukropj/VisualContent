/*
 * OsgProperty.cpp
 *
 *  Created on: 2.3.2011
 *      Author: jakub
 */

#include "Viewer/OsgProperty.h"
#include "Util/Config.h"

using namespace Vwr;

QColor OsgProperty::allColors[] = {Qt::white, Qt::red, Qt::yellow, Qt::green, Qt::blue, Qt::magenta, Qt::cyan, Qt::darkGray};

OsgProperty::OsgProperty() {
	contentType = RANDOM;
}

QList<OsgProperty::ContentType> OsgProperty::getContentTypes() {
	QList<ContentType> contentTypes;
	contentTypes << RANDOM << IMAGE << TEXT; // << WEB << OSG;
	return contentTypes;
}

QList<OsgProperty::ValueType> OsgProperty::getValueTypes() {
	QList<ValueType> propTypes;
	propTypes << LABEL << COLOR << CONTENT << DIRECTION;
	return propTypes;
}

OsgProperty::PropertyType OsgProperty::getPropertyType(ValueType val) {
	switch (val) {
	case COLOR :
	case LABEL : return ALL;
	case CONTENT : return NODE;
	case DIRECTION : return EDGE;
	default : return ALL;
	}
}

osg::Vec4f OsgProperty::getColor(QString key, PropertyType type) {
	if (key.isEmpty()) {
		return getDefaultColor(type);
	}

	if (!colors.contains(key)) {
		QColor c = allColors[colors.size() % 7];
		colors.insert(key, osg::Vec4f((float)c.red(), (float)c.green(), (float)c.blue(), (float)c.alpha()));
	}
	return colors.value(key);
}


osg::Vec4f OsgProperty::getDefaultColor(PropertyType type) {
	switch (type) {
	case NODE :
	default :
		return Util::Config::getColorF("Viewer.Node.Color");
	case EDGE :
		return Util::Config::getColorF("Viewer.Edge.Color");
	}
}
