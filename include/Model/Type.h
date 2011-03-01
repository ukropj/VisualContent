/*!
 * Type.h
 * Projekt 3DVisual
 */

#ifndef DATA_TYPE_DEF
#define DATA_TYPE_DEF 1

#include <string>
#include <QString>
#include <QTextStream>
#include <QSet>
#include <QMap>
#include <QColor>
#include <osg/ref_ptr>
#include <osg/Texture2D>

namespace Model  {
class Graph;

/**
 *  \class Type
 *  \brief Type object represents type of an Node or Edge in a Graph
 *  \author Aurel Paulovic
 *  \date 29. 4. 2010
 */
class Type {
public:
	Type(qlonglong id, QString name, Graph* graph,
			QMap<QString, QString>* settings = 0);
	~Type();
	QString getName() const {
		return name;
	}
	qlonglong getId() const {
		return id;
	}
	QString toString() const {
		QString str;
		QTextStream(&str) << "type id:" << id << " name:" << name;
//		foreach(QString key, dataKeys.keys()) {
//			QTextStream(&str) << key << "=" << dataKeys.value(key) <<", ";
//		}
		return str;
	}
	QString getValue(QString key) const {
		return settings->value(key);
	}
	void setValue(QString key, QString value) {
		settings->insert(key, value);
	}
	osg::ref_ptr<osg::Texture2D> getTexture() const {
		return texture;
	}
	float getScale() const {
		return scale;
	}
	Graph* getGraph() const {
		return graph;
	}
	osg::Vec4f getColor(QString key = "");

	void addKey(QString key, QString defaultValue="") {
		dataKeys.insert(key, defaultValue);
	}

	QList<QString> getKeys() const {
		return dataKeys.keys();
	}

	QString getDefaultValue(QString key) const {
		return dataKeys.value(key, "");
	}

	enum DataType {
		COLOR, LABEL, TEXT, IMAGE, OSG, IS_ORIENTED,
	};

	void insertMapping(DataType key, QString value) {
		if (!value.isEmpty())
			mapping.insert(key, value);
		else
			mapping.remove(key);
	}
	QString getMapping(DataType key) const {
		return mapping.value(key, "");
	}
	bool hasMapping(DataType key) const {
		return !mapping.value(key, "").isEmpty();
	}

	static QString dataTypeToString(DataType dt) {
		switch (dt) {
		case COLOR : return "Color";
		case LABEL : return "Label";
		case TEXT : return "Text";
		case IMAGE : return "Image";
		case OSG : return "3D Object";
		case IS_ORIENTED : return "Orientation";
		default : return QString("%1").arg((int)dt);
		}
	};

private:
	Graph* graph;
	qlonglong id;
	QString name;
	QMap<QString, QString> dataKeys;
	QMap<QString, QString>* settings;
	osg::ref_ptr<osg::Texture2D> texture;
	float scale;

	QMap<QString, osg::Vec4f> colors;
	QMap<DataType, QString> mapping;

	static QColor qColors[];
};

class Data {
public:
	Data(){};
	~Data(){};
	void insert(QString key, QString value) {
		data.insert(key, value);
	}
	QString value(QString key) const {
		return data.value(key, "");
	}
private:
	QMap<QString, QString> data;
};

}
#endif
