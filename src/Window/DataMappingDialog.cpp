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

DataMappingDialog::DataMappingDialog(QList<Model::Type*> types, QMap<qlonglong, OsgProperty*>* propertyMap,
		QWidget* parent) : QDialog(parent) {
	setWindowTitle(tr("Data Mapping"));
	setModal(true);
	this->types = types;
	this->propertyMap = propertyMap;

	contentTypes = OsgProperty::getContentTypes();
	valueTypes = OsgProperty::getValueTypes();
	createControls();
}

void DataMappingDialog::createControls() {
	QStringList typeNames;
	QListIterator<OsgProperty::ContentType> cti(contentTypes);
	while (cti.hasNext()) {
		typeNames.append(OsgProperty::contentTypeToString(cti.next()));
	}
	QTabWidget* tabWidget = new QTabWidget;

	QListIterator<Model::Type*> ti(types);
	while (ti.hasNext()) {
		Model::Type* type = ti.next();
		QList<QString> keys = type->getKeys();
		keys.insert(0, " - ");

		QWidget* tab = new QWidget;
		QGridLayout* tabLayout = new QGridLayout;
		tabLayout->setSizeConstraint(QLayout::SetFixedSize);

		QListIterator<OsgProperty::ValueType> vi(valueTypes);
		int i = 0;
		while (vi.hasNext()) {
			OsgProperty::ValueType val = vi.next();
			OsgProperty::PropertyType propType = OsgProperty::getPropertyType(val);
			if (propType == OsgProperty::ALL ||
					(type->isEdgeType() && propType == OsgProperty::EDGE) ||
					(!type->isEdgeType() && propType == OsgProperty::NODE)) {
				QLabel* keyLabel = new QLabel(OsgProperty::propertyTypeToString(val));
				QComboBox* keySelector = new QComboBox();
				keySelector->insertItems(0, QStringList(keys));
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
			connect(typeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSelected(int)));
			if (!type->isEdgeType()) {
				tabLayout->addWidget(typeLabel, i, 1);
				tabLayout->addWidget(typeSelector, i, 2);
			}
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

	OsgProperty::ValueType prop = valueTypes.at(cbIndex % valueTypes.size());

	OsgProperty* p = propertyMap->value(type->getId());

	if (index > 0) {
		p->insertMapping(prop, key);
	} else {
		p->insertMapping(prop, "");
	}
	qDebug() << "Mapped " << type->getName() << ": " <<
			OsgProperty::propertyTypeToString(prop) << " -> " << key;
}

void DataMappingDialog::typeSelected(int index) {
	const QObject* sender = QObject::sender();
	QComboBox* comboBox = (QComboBox*)sender;
	int cbIndex = typeSelectors.indexOf(comboBox);
	Model::Type* type = types.at(cbIndex);

	OsgProperty::ContentType contentType = contentTypes.at(index);

	OsgProperty* p = propertyMap->value(type->getId());

	p->setContentType(contentType);

	qDebug() << "Mapped " << type->getName() << ": " <<
			"Content Type" << " -> " << OsgProperty::contentTypeToString(contentType);
}
