#include  "Window/MessageWindows.h"

using namespace Window;

MessageWindows::MessageWindows(void) {
}

MessageWindows::~MessageWindows(void) {
}

void MessageWindows::showMessageBox(QString title, QString message,
		bool isError) {
	QMessageBox msgBox;
	msgBox.setText(message);
	msgBox.setWindowTitle(title);
	if (isError) {
		msgBox.setIcon(QMessageBox::Warning);
	} else {
		msgBox.setIcon(QMessageBox::Information);
	}
	msgBox.exec();
}
