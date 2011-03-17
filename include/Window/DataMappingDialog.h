/*
 * DataMappingDialog.h
 *
 *  Created on: 26.2.2011
 *      Author: jakub
 */

#ifndef DATAMAPPINGDIALOG_H_
#define DATAMAPPINGDIALOG_H_

#include <QDialog>
#include <QtGui>

#include "Viewer/DataMapping.h"

namespace Model {
class Type;
}

namespace Window {

class DataMappingDialog : public QDialog {
	Q_OBJECT
private slots:
	void keySelected(int index);
	void typeSelected(int index);
public:
	DataMappingDialog(QList<Model::Type*> types, QMap<qlonglong, Vwr::DataMapping*>* propertyMap, QWidget* parent = 0);
private:
	void createControls();
	QString getTypeName(QObject* sender);

	QMap<qlonglong, Vwr::DataMapping*>* propertyMap;

	QList<QComboBox*> keySelectors;
	QList<QComboBox*> typeSelectors;
	QPushButton* submitButton;

	QList<Model::Type*> types;
	QList<Vwr::DataMapping::ContentType> contentTypes;
	QList<Vwr::DataMapping::ValueType> valueTypes;
};
}

#endif /* DATAMAPPINGDIALOG_H_ */
