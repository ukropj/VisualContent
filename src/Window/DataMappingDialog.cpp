/*
 * DataMappingDialog.cpp
 *
 *  Created on: 26.2.2011
 *      Author: jakub
 */

#include "Window/DataMappingDialog.h"
#include <qDebug>

using namespace Window;

DataMappingDialog::DataMappingDialog(QList<Model::Type::DataType> availableData,
		QList<Model::Type*> types, QWidget* parent) : QDialog(parent) {
	setWindowTitle(tr("Data Mapping"));
	setModal(true);
	this->dataTypes = availableData;
	this->types = types;
	createControls();
}

void DataMappingDialog::createControls() {
	QTabWidget* tabWidget = new QTabWidget;
	QListIterator<Model::Type*> ti(types);
	while (ti.hasNext()) {
		Model::Type* type = ti.next();
		QList<QString> keys = type->getKeys();
	QListIterator<QString> ki(keys);
	while (ki.hasNext()) qDebug() << ki.next();
		keys.insert(0, "(none)");

		QWidget* tab = new QWidget;
		QGridLayout* tabLayout = new QGridLayout;
		tabLayout->setSizeConstraint(QLayout::SetFixedSize);

		int i = 0;
		QListIterator<Model::Type::DataType> dti(dataTypes);
		while (dti.hasNext()) {
			Model::Type::DataType dataType = dti.next();
			QLabel* label = new QLabel(Model::Type::dataTypeToString(dataType));
			labels.append(label);
			tabLayout->addWidget(label, i, 0);
			QComboBox* keySelector = new QComboBox();
			keySelector->insertItems(0, QStringList(keys));
			keySelector->setFont(QFont("Courier"));
			connect(keySelector, SIGNAL(currentIndexChanged(int)), this, SLOT(keySelected(int)));
			keySelectors.append(keySelector);
			tabLayout->addWidget(keySelector, i, 1);
			i++;
		}
		tab->setLayout(tabLayout);
		tabWidget->addTab(tab, type->getName());
	}

	submitButton = new QPushButton(tr("&Close"));
	submitButton->setDefault(true);
	connect(submitButton, SIGNAL(clicked()), this, SLOT(accept()));

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(submitButton);

	setLayout(mainLayout);
}

void DataMappingDialog::keySelected(int index) {
	const QObject* sender = QObject::sender();
	QComboBox* comboBox = (QComboBox*)sender;
	int cbIndex = keySelectors.indexOf(comboBox);
	Model::Type* type = types.at(cbIndex / dataTypes.size());
	QString key = comboBox->itemText(index);
	Model::Type::DataType dataType = dataTypes.at(cbIndex % dataTypes.size());

	if (index > 0) {
		type->insertMapping(dataType, key);
	} else {
		type->insertMapping(dataType, "");
	}

	qDebug() << "Mapped " << type->getName() << ": " <<
			Model::Type::dataTypeToString(dataType) << " -> " << key;
}
