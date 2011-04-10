/*
 * DataMapping.h
 *
 *  Created on: 2.3.2011
 *      Author: jakub
 */

#ifndef DataMapping_H_
#define DataMapping_H_

#include <QMap>
#include <QString>
#include <QColor>
#include <osg/Vec4f>
#include <osg/Texture2D>

namespace Vwr {

class DataMapping {
public:
	enum ContentType {
		NO_CONTENT, TEXT, WEB, IMAGE, OSG, RANDOM, COMPOSITE
	};
	enum ValueType {
		LABEL, COLOR, CONTENT, DIRECTION
	};
	enum PropertyType {
		NODE, EDGE, CLUSTER, ALL
	};

	DataMapping();

	osg::Vec4f getColor(QString key = "", PropertyType type = NODE);

	void insertMapping(ValueType val, QString key) {
		if (!key.isEmpty())
			mapping.insert(val, key);
		else
			mapping.remove(val);
	}
	QString getMapping(ValueType val) const {
		return mapping.value(val, "");
	}
	bool hasMapping(ValueType val) const {
		return !mapping.value(val, "").isEmpty();
	}
	void setContentType(ContentType type) { contentType = type; }
	ContentType getContentType() const { return contentType; }

	static PropertyType getPropertyType(ValueType val);
	static QList<DataMapping::ContentType> getContentTypes();
	static QList<DataMapping::ValueType> getValueTypes();
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
	static QString propertyTypeToString(ValueType val) {
		switch (val) {
		case COLOR : return "Color";
		case LABEL : return "Label";
		case CONTENT : return "Content";
		case DIRECTION : return "Directed";
		default : return QString("%1").arg((int)val);
		}
	};
	static osg::Vec4f getDefaultColor(PropertyType type = NODE);
	static float getScale(PropertyType type = NODE);

private:
	ContentType contentType;
	QMap<ValueType, QString> mapping;

	QMap<QString, osg::Vec4f> colors;
	static QColor allColors[];
};
}

#endif /* DataMapping_H_ */
