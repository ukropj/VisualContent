/*
 * ImageContent.cpp
 *
 *  Created on: 29.12.2010
 *      Author: jakub
 */

#include "Viewer/ImageContent.h"
#include <QDebug>
#include <QString>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>

using namespace Vwr;

ImageContent::ImageContent(QString imagePath) : imagePath(imagePath) {
	loaded = false;
}

ImageContent::~ImageContent() {
}

bool ImageContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;

		QLabel* l = new QLabel;
		QImage image;
		image.load(imagePath);
		l->setPixmap(QPixmap::fromImage(image));
		QSize size = image.size();
		l->setGeometry(QRect(QPoint(1, 1), size));
		setWidget(l, 0.2f);

		return true;
	}
}
