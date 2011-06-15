#ifndef UTIL_HASHMAPSINGLETON_DEF
#define UTIL_HASHMAPSINGLETON_DEF 1

#include <iostream>
#include <QMap>
#include <OpenThreads/Mutex>
#include <QString>
#include <QStringList>
#include <QFile>

#include <osg/Vec4s>
#include <osg/Vec4f>

typedef QMap<QString, QString> MapSS;

/// Utilities
namespace Util
{
	/*!
	 * \brief Singleton class for configuration storage and I/O
	 * \author 
	 * Adam Pazitnaj, Michal Paprcka
	 */
	class Config
	{
		public:

			static Config* getInstance();
			void add(QString key, QString value);
			static QString getValue(QString key);
			static osg::Vec4s getColorI(QString key);
			static osg::Vec4f getColorF(QString key);

			QString getMapValue(QString key) {
				return _map.value(key);
			}
			QStringList getList();
			void saveConfig();

		private:
			Config();
			~Config();
			static OpenThreads::Mutex _mutex;
			static Config* _instance;
			MapSS _map;
	};
}
#endif
