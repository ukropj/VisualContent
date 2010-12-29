/*
 * WidgetContent.cpp
 *
 *  Created on: 25.12.2010
 *      Author: jakub
 */

#include "Viewer/WidgetContent.h"
#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osg/Texture>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "OsgQtBrowser/QWidgetImage.h"

namespace Vwr {

WidgetContent::WidgetContent() : OsgContent() {
}

/*WidgetContent::WidgetContent(QString imagePath) : OsgContent(imagePath) {
//	QWidget* w = new QWidget;
//	w->setLayout(new QVBoxLayout);

	QString text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
	QLabel* l = new QLabel;
	QImage image;
	image.load("img/pic.jpg");
//	l->setPixmap(QPixmap(imagePath));
//	l->setText(text);
	l->setPixmap(QPixmap::fromImage(image));
//	w->layout()->addWidget(l);
	QRect g = l->geometry();
	qDebug() << g;
	qDebug() << image.rect();
//	w->setGeometry(0, 0, 400, 400);
//	l->setGeometry(1, 1, 500, 500);
	QSize size = image.size();
	l->setGeometry(QRect(QPoint(1, 1), size));

	setWidget(l, 0.2f);
}*/

WidgetContent::~WidgetContent() {
}

// TODO refactor
void WidgetContent::setWidget(QWidget* widget, float scale) {
	osg::ref_ptr<QWidgetImage> widgetImage = new QWidgetImage(widget);
	widgetImage->getQWidget()->setAttribute(Qt::WA_TranslucentBackground);
	widgetImage->getQGraphicsViewAdapter()->setBackgroundColor(QColor(0, 0, 0, 0));

	QSize size = widget->geometry().size();
	size *= scale;

	// determines size & ratio!
	osg::Geometry* quad = osg::createTexturedQuadGeometry(
			osg::Vec3(-size.width()/2.0f, -size.height()/2.0f, 0),
			osg::Vec3(size.width(), 0, 0), osg::Vec3(0, size.height(), 0), 1, 1);

	addDrawable(quad);
	osg::ref_ptr<osg::StateSet> stateSet = getOrCreateStateSet();

	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateSet->setAttribute(new osg::Program);

	osg::Texture2D* texture = new osg::Texture2D(widgetImage.get());
	texture->setResizeNonPowerOfTwoHint(false);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	stateSet->setTextureAttributeAndModes(0, texture,
			osg::StateAttribute::ON);

	// this enables interaction with the widget (buggy), use current camera
	// nothing is shown without this code!
	osg::Camera* camera = 0;
	osgViewer::InteractiveImageHandler* handler =
			new osgViewer::InteractiveImageHandler(widgetImage.get(), texture, camera);
	quad->setEventCallback(handler);
	quad->setCullCallback(handler);

	setStateSet(stateSet);
}

bool WidgetContent::load() {
	return false;
}

}
