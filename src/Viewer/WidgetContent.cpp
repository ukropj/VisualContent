/*
 * WidgetContent.cpp
 *
 *  Created on: 25.12.2010
 *      Author: jakub
 */

#include "Viewer/WidgetContent.h"
#include <osg/Texture>
#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "OsgQtBrowser/QWidgetImage.h"
#include "Util/TextureWrapper.h"
#include <QTextEdit>
#include <QWebView>

using namespace Vwr;

WidgetContent::WidgetContent() : OsgContent() {
	loaded = false;
}

WidgetContent::~WidgetContent() {
}

// TODO refactor
void WidgetContent::setWidget(QWidget* widget, float scale) {
	osg::ref_ptr<QWidgetImage> widgetImage = new QWidgetImage(widget);
	widgetImage->getQWidget()->setAttribute(Qt::WA_TranslucentBackground);
	widgetImage->getQGraphicsViewAdapter()->setBackgroundColor(QColor(0, 0, 0, 0));

	QSize size = widget->geometry().size();
	float width = size.width()*150 / ((float)size.height());
	float height = 150; // XXX magic numbers

	width *= scale;
	height *= scale;

	// determines size & ratio!
	osg::Geometry* quad = osg::createTexturedQuadGeometry(
			osg::Vec3(-width/2.0f, -height/2.0f, 0),
			osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
	osg::ref_ptr<osg::StateSet> stateSet = getOrCreateStateSet();

	osg::ref_ptr<osg::Texture2D> texture = Util::TextureWrapper::createTexture(widgetImage);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
//	stateSet->setAttribute(new osg::Program);

	// this enables interaction with the widget (buggy), use current camera
	// nothing is shown without this code!
	osg::Camera* camera = 0;
	osgViewer::InteractiveImageHandler* handler =
			new osgViewer::InteractiveImageHandler(widgetImage.get(), texture, camera);
	quad->setEventCallback(handler);
	quad->setCullCallback(handler);

	contentGeode = new osg::Geode;
	contentGeode->addDrawable(quad);
	contentGeode->setStateSet(stateSet);
	contentGeode->setName("node_content_widget");
	addChild(contentGeode);
	loaded = true;
}

TextContent::TextContent(QString text) : text(text) {
	loaded = false;
}

TextContent::~TextContent() {
}

bool TextContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;

		QTextEdit* l = new QTextEdit();
		l->setText(text);
		QSize size(200, 150);	// XXX magic
		l->setGeometry(QRect(QPoint(1, 1), size));

//		QWebView* view = new QWebView();
//		view->load(QUrl("http://qt.nokia.com/"));


		setWidget(l, 0.2f); 	// XXX magic number

		contentGeode->setName("node_content_text");
		return true;
	}
}

WebContent::WebContent(QString url) : url(url) {
	loaded = false;
}

WebContent::~WebContent() {
}

bool WebContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;

		QSize size(200, 150); // XXX magic

		QWebView* view = new QWebView();
		view->setZoomFactor(0.2);
		view->setGeometry(QRect(QPoint(1, 1), size));
		view->load(QUrl(url));

		setWidget(view, 0.2f); 	// XXX magic number

		contentGeode->setName("node_content_web_page");
		return true;
	}
}
