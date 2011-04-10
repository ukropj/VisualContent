/*
 * ProgressManager.cpp
 *
 *  Created on: 10.4.2011
 *      Author: jakub
 */

#include "Window/ProgressManager.h"
#include <QDebug>

using namespace Window;

ProgressManager::ProgressManager(QWidget* parent) {
	dialog = new QProgressDialog("", "", 0, 10, parent, Qt::Dialog);
	dialog->setWindowTitle("Loading");
//	dialog->setCancelButton(NULL);
	dialog->setCancelButtonText("Abort");
	Qt::WindowFlags flags = dialog->windowFlags();
	flags = flags & (~Qt::WindowContextHelpButtonHint);
	dialog->setWindowFlags(flags);
	dialog->setModal(true); // TODO make this work
	dialog->setMinimumDuration(100);

	progress = 0;
}

void ProgressManager::set(QString label, int maximum) {
	progress = 0;
	dialog->reset();
	dialog->setLabelText(label);
	dialog->setMaximum(maximum);
}

void ProgressManager::setValue(int progress) {
	dialog->setValue(progress);
}

bool ProgressManager::wasCanceled() {
	return dialog->wasCanceled();
}

void ProgressManager::close() {
	dialog->reset();
}

void ProgressManager::increment() {
	dialog->setValue(++progress);
}
