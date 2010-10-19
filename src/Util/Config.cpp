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
		if (data[i].simplified().length() > 0) {
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
