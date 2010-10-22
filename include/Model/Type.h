/*!
 * Type.h
 * Projekt 3DVisual
 */

#ifndef DATA_TYPE_DEF
#define DATA_TYPE_DEF 1

#include <string>
#include <QString>
#include <QTextStream>
#include <QMap>

#include <osg/ref_ptr>
#include <osg/Texture2D>

#include "Model/Graph.h"
#include "Util/Config.h"

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

	/**
	 *  \fn inline public constructor  Type(qlonglong id, QString name, Graph* graph, QMap<QString, QString> * settings = 0)
	 *  \brief Creates new Type object
	 *  \param  id     ID of the Type
	 *  \param  name   name of the Type
	 *  \param  settings    settings of the Type
	 */
	Type(qlonglong id, QString name, Graph* graph,
			QMap<QString, QString> * settings = 0);

	/**
	 *  \fn public destructor  ~MetaType
	 *  \brief Destroys Type object
	 */
	~Type(void);

	/**
	 *  \fn inline public  getName
	 *  \brief Returns the name of the Type
	 *  \return QString name of the Type
	 */
	QString getName() const {
		return name;
	}

	/**
	 *  \fn inline public  setName
	 *  \brief Sets new name
	 *  \param [in]       name QString    new Name for the Type
	 *  \return QString resultant name of the Type
	 */
	void setName(QString val) {
		name = val;
	}

	/**
	 *  \fn inline public  getId
	 *  \brief Returns ID of the Type
	 *  \return qlonglong ID of the Type
	 */
	qlonglong getId() const {
		return id;
	}

	/**
	 *	\fn inline public isMeta
	 *	\brief Returns flag indicating if the Type is an MetaType or not
	 *	\return bool true, if the Type is an MetaType
	 */
	bool isMeta() const {
		return meta;
	}

	/**
	 *  \fn inline public constant  toString
	 *  \brief Returns human-readable string representing the Type
	 *  \return QString
	 */
	QString toString() const {
		QString str;
		QTextStream(&str) << "type id:" << id << " name:" << name << " meta:"
				<< this->isMeta();
		return str;
	}

	/**
	 *  \fn inline public constant  getSettings
	 *  \brief Returns settings of the Type
	 *  \return QMap<QString,QString> * settings
	 */
	QString getValue(QString key) const {
		return settings->value(key);
	}

	/**
	 *  \fn inline public  setSettings(QMap<QString, QString> * val)
	 *  \brief Sets settings of the Type to val
	 *
	 *	Overrides all previous settings
	 *
	 *  \param  val   new settings
	 */
	void setValue(QString key, QString value) {
		settings->insert(key, value);
	}

	/**
	 *  \fn inline public constant  getTypeTexture
	 *  \brief Returns type texture
	 *  \return osg::ref_ptr<osg::Texture2D> type texture
	 */
	osg::ref_ptr<osg::Texture2D> getTexture() const {
		return texture;
	}

	/**
	 *  \fn inline public constant  getScale
	 *  \brief Returns type scale
	 *  \return float type scale
	 */
	float getScale() const {
		return scale;
	}

	/**
	 *  \fn inline public  getGraph
	 *  \brief Returns Graph to which is the Type assigned
	 *  \return Graph *
	 */
	Graph* getGraph() {
		return graph;
	}
protected:

	/**
	 *  bool meta
	 *  \brief Flag if the Type is an MetaType or not
	 */
	bool meta;

	/**
	 *  Graph * graph
	 *  \brief Graph object to which the Type belongs
	 */
	Graph* graph;

	/**
	 *  qlonglong id
	 *  \brief ID of the Type
	 */
	qlonglong id;

	/**
	 *  QString name
	 *  \brief Name of the Type
	 */
	QString name;

	/**
	 *  QMap<QString,QString> * settings
	 *  \brief Settings of the Type
	 */
	QMap<QString, QString> * settings;

	/**
	 *  osg::ref_ptr typeTexture
	 *  \brief Type texture
	 */
	osg::ref_ptr<osg::Texture2D> texture;

	/**
	 *  float scale
	 *  \brief Type scale
	 */
	float scale;
};
}
#endif
