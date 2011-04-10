/*
 * ProgressManager.h
 *
 *  Created on: 10.4.2011
 *      Author: jakub
 */

#ifndef PROGRESSMANAGER_H_
#define PROGRESSMANAGER_H_

#include<QProgressDialog>
#include<QObject>

namespace Window {

class ProgressManager : public QObject {
	Q_OBJECT
public:
	ProgressManager(QWidget* parent);
	void set(QString label, int maximum);
	void close();
	bool wasCanceled();
	void increment();
	void setValue(int progress);

	void setDialog(QProgressDialog* d) {
		dialog = d;
	}
private:
	QProgressDialog* dialog;
	int progress;
};

}

#endif /* PROGRESSMANAGER_H_ */

