#include "Util/Config.h"

using namespace Util;

Config * Config::_instance;
OpenThreads::Mutex Config::_mutex;

Config::Config() {
	//otvorenie suboru na nacitanie dat
	QFile file("config/config");
	file.open(QIODevice::ReadOnly);

	//rozdelenie dat podla riadkov
	QList<QString> data = QString(file.readAll()).split("\n");
	file.close();

	//sparsovanie dat a ich ulozenie do mapy
	for (int i = 0; i < data.length(); i++) {
		if (data[i].simplified().length() > 0 &&
				!data[i].startsWith("//")) {
			QList<QString> row = data[i].split("=");
			add(row[0].simplified(), row[row.length() - 1].simplified());
		}
	}
}

Config::~Config() {
}

void Config::add(QString key, QString value) {
	_map.insert(key, value);
}

QString Config::getValue(QString key) {
	return getInstance()->_map.value(key);
}

osg::Vec4s Config::getColorI(QString key) {
	QString val = getInstance()->_map.value(key);
	QList<QString> data = val.split(",");
	while (data.size() < 4)
		data.append("255");
	osg::Vec4s color(data[0].toInt(), data[1].toInt(), data[2].toInt(), data[3].toInt());
	return color;
}

osg::Vec4f Config::getColorF(QString key) {
	QString val = getInstance()->_map.value(key);
	QList<QString> data = val.split(",");
	while (data.size() < 4)
		data.append("255");
	osg::Vec4f color(data[0].toFloat()/255.0f, data[1].toFloat()/255.0f, data[2].toFloat()/255.0f, data[3].toFloat()/255.0f);
	return color;
}

Config* Config::getInstance() {
	_mutex.lock();

	if (_instance == NULL) {
		_instance = new Config();
	}
	_mutex.unlock();

	return _instance;
}

QStringList Config::getList() {
	MapSS::iterator i;
	QStringList list;
	QString item;

	//iteruje postupne cez vsetky polozky a uklada ich do zoznamu retazov 
	for (i = _map.begin(); i != _map.end(); ++i) {
		item = QString("%1\t%2").arg(i.key()).arg(i.value());
		list.append(item);
	}
	list.sort();
	return list;
}

void Config::saveConfig() {
	MapSS::iterator i;
	QStringList list;
	QString item;
	QFile file("config/config");
	QByteArray ba;
	file.open(QIODevice::WriteOnly);
	//postupne ukladanie do suboru
	for (i = _map.begin(); i != _map.end(); ++i) {
		item = QString("%1=%2\n").arg(i.key()).arg(i.value());
		ba = item.toLatin1();
		file.write(ba.data());
	}
	file.close();
}
