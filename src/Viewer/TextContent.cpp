/*
 * TextContent.cpp
 *
 *  Created on: 29.12.2010
 *      Author: jakub
 */

#include "Viewer/TextContent.h"
#include <QtGui/QTextEdit>

using namespace Vwr;

TextContent::TextContent(QString text) {
	QTextEdit* l = new QTextEdit;
	l->setText(text);
	QSize size(200, 150);
	l->setGeometry(QRect(QPoint(1, 1), size));
	setWidget(l, 0.2f);

	loaded = true;
}

TextContent::~TextContent() {
	// TODO Auto-generated destructor stub
}

bool TextContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;

		//TODO

		return true;
	}
}
