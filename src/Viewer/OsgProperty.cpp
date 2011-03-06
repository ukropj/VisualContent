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
	contentType = NO_CONTENT;
}

QList<OsgProperty::ContentType> OsgProperty::getContentTypes(PropertyType type) {
	QList<ContentType> contentTypes;
	contentTypes << NO_CONTENT;
	if (type == NODE || type == ALL)
		contentTypes << IMAGE << TEXT << RANDOM; // << WEB << OSG;
	return contentTypes;
}

QList<OsgProperty::ValueType> OsgProperty::getPropertyTypes(PropertyType type) {
	QList<ValueType> propTypes;
	propTypes << LABEL << COLOR;
	if (type == NODE || type == ALL) {
		propTypes << CONTENT;
	}
	if (type == EDGE || type == ALL) {
		;
	}
	return propTypes;
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
