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
#include "Model/Type.h"

namespace Window {

class DataMappingDialog : public QDialog {
	Q_OBJECT
private slots:
	void keySelected(int index);
public:
	DataMappingDialog(QList<Model::Type::DataType> availableData,
			QList<Model::Type*> keys, QWidget* parent = 0);
private:
	void createControls();

	QList<QLabel*> labels;
	QList<QComboBox*> keySelectors;
	QPushButton* submitButton;

	QList<Model::Type*> types;
	QList<Model::Type::DataType> dataTypes;
};
}

#endif /* DATAMAPPINGDIALOG_H_ */
