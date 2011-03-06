/*
 * OsgProperty.h
 *
 *  Created on: 2.3.2011
 *      Author: jakub
 */

#ifndef OSGPROPERTY_H_
#define OSGPROPERTY_H_

#include <QMap>
#include <QString>
#include <QColor>
#include <osg/Vec4f>
#include <osg/Texture2D>

namespace Vwr {

class OsgProperty {
public:
	enum ContentType {
		NO_CONTENT, TEXT, WEB, IMAGE, OSG, RANDOM
	};
	enum ValueType {
		LABEL, COLOR, CONTENT
	};
	enum PropertyType {
		NODE, EDGE, ALL
	};

	OsgProperty();

	osg::Vec4f getColor(QString key = "", PropertyType type = NODE);

	void insertMapping(ValueType prop, QString key) {
		if (!key.isEmpty())
			mapping.insert(prop, key);
		else
			mapping.remove(prop);
	}
	QString getMapping(ValueType prop) const {
		return mapping.value(prop, "");
	}
	bool hasMapping(ValueType prop) const {
		return !mapping.value(prop, "").isEmpty();
	}
	void setContentType(ContentType type) { contentType = type; }
	ContentType getContentType() const { return contentType; }

	static QList<OsgProperty::ContentType> getContentTypes(PropertyType type = ALL);
	static QList<OsgProperty::ValueType> getPropertyTypes(PropertyType type = ALL);
	static QString contentTypeToString(ContentType ct) {
		switch (ct) {
		case NO_CONTENT : return "None";
		case IMAGE : return "Image";
		case TEXT : return "Text";
		case WEB : return "HTML page";
		case OSG : return "3D object";
		case RANDOM : return "Generate";
		default : return QString("%1").arg((int)ct);
		}
	};
	static QString propertyTypeToString(ValueType prop) {
		switch (prop) {
		case COLOR : return "Color";
		case LABEL : return "Label";
		case CONTENT : return "Content";
		default : return QString("%1").arg((int)prop);
		}
	};
	static osg::Vec4f getDefaultColor(PropertyType type = NODE);

private:
	ContentType contentType;
	QMap<ValueType, QString> mapping;

	QMap<QString, osg::Vec4f> colors;
	static QColor allColors[];
};
}

#endif /* OSGPROPERTY_H_ */
