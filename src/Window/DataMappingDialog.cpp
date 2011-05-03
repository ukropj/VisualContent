/*
 * DataMappingDialog.cpp
 *
 *  Created on: 26.2.2011
 *      Author: jakub
 */

#include "Window/DataMappingDialog.h"
#include "Model/Type.h"
#include <qDebug>

using namespace Window;
using namespace Vwr;

DataMappingDialog::DataMappingDialog(QList<Model::Type*> types, QMap<qlonglong, DataMapping*>* mappings,
		QWidget* parent) : QDialog(parent) {
	setWindowTitle(tr("Data Mapping"));
	setModal(true);
	this->types = types;
	this->mappings = mappings;
	contentTypes = DataMapping::getContentTypes();
	valueTypes = DataMapping::getValueTypes();
	createControls();
}

void DataMappingDialog::createControls() {
	QStringList typeNames;
	QListIterator<DataMapping::ContentType> cti(contentTypes);
	while (cti.hasNext()) {
		typeNames.append(DataMapping::contentTypeToString(cti.next()));
//		qDebug() << DataMapping::contentTypeToString(cti.next());
	}
	QTabWidget* tabWidget = new QTabWidget;

	QListIterator<Model::Type*> ti(types);
	while (ti.hasNext()) {
		Model::Type* type = ti.next();
		DataMapping* mapping = mappings->value(type->getId());
		QList<QString> keys = type->getKeys();
		keys.insert(0, " - ");

		QWidget* tab = new QWidget;
		QGridLayout* tabLayout = new QGridLayout;
		tabLayout->setSizeConstraint(QLayout::SetFixedSize);

		QListIterator<DataMapping::ValueType> vi(valueTypes);
		int i = 0;
		while (vi.hasNext()) {
			DataMapping::ValueType val = vi.next();
			DataMapping::PropertyType propType = DataMapping::getPropertyType(val);
			if (propType == DataMapping::ALL ||
					(type->isEdgeType() && propType == DataMapping::EDGE) ||
					(!type->isEdgeType() && propType == DataMapping::NODE)) {
				QLabel* keyLabel = new QLabel(DataMapping::propertyTypeToString(val));
				QComboBox* keySelector = new QComboBox();
				keySelector->insertItems(0, QStringList(keys));
				int index = keys.indexOf(mapping->getMapping(val));
				keySelector->setCurrentIndex(qMax(0, index));
				keySelector->setFont(QFont("Courier"));
				connect(keySelector, SIGNAL(currentIndexChanged(int)), this, SLOT(keySelected(int)));
				tabLayout->addWidget(keyLabel, i, 0);
				tabLayout->addWidget(keySelector, i, 1);
				keySelectors.append(keySelector);
			} else {
				keySelectors.append(NULL);
			}
			i++;
		}

		if (!type->isEdgeType()) {
			QLabel* typeLabel = new QLabel("Content type");
			QComboBox* typeSelector = new QComboBox();
			typeSelector->insertItems(0, typeNames);
			int index = typeNames.indexOf(DataMapping::contentTypeToString(mapping->getContentType()));
			typeSelector->setCurrentIndex(index);
			connect(typeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSelected(int)));
			tabLayout->addWidget(typeLabel, i, 0);
			tabLayout->addWidget(typeSelector, i, 1);
			typeSelectors.append(typeSelector);
		} else {
			typeSelectors.append(NULL);
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
	Model::Type* type = types.at(cbIndex / valueTypes.size());
	QString key = comboBox->itemText(index);

	DataMapping::ValueType prop = valueTypes.at(cbIndex % valueTypes.size());

	DataMapping* p = mappings->value(type->getId());

	if (index > 0) {
		p->insertMapping(prop, key);
	} else {
		p->insertMapping(prop, "");
	}
	qDebug() << "Mapped " << type->getName() << ": " <<
			DataMapping::propertyTypeToString(prop) << " -> " << key;
}

void DataMappingDialog::typeSelected(int index) {
	const QObject* sender = QObject::sender();
	QComboBox* comboBox = (QComboBox*)sender;
	int cbIndex = typeSelectors.indexOf(comboBox);
	Model::Type* type = types.at(cbIndex);

	DataMapping::ContentType contentType = contentTypes.at(index);

	DataMapping* p = mappings->value(type->getId());

	p->setContentType(contentType);

	qDebug() << "Mapped " << type->getName() << ": " <<
			"Content Type" << " -> " << DataMapping::contentTypeToString(contentType);
}
