/*!
 * Type.h
 * Projekt 3DVisual
 */

#ifndef MODEL_TYPE_DEF
#define MODEL_TYPE_DEF 1

#include <string>
#include <QString>
#include <QTextStream>
#include <QSet>
#include <QMap>
#include <QColor>

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
	Graph* getGraph() const {
		return graph;
	}

	void addKey(QString key, QString defaultValue="") {
		dataKeys.insert(key, defaultValue);
	}

	QList<QString> getKeys() const {
		return dataKeys.keys();
	}

	QString getDefaultValue(QString key) const {
		return dataKeys.value(key, "");
	}

	void setEdgeType(bool flag) {
		edgeType = flag;
	}
	bool isEdgeType() const {
		return edgeType;
	}

private:
	Graph* graph;
	qlonglong id;
	QString name;
	bool edgeType;
	QMap<QString, QString> dataKeys;
	QMap<QString, QString>* settings;
};

}
#endif
