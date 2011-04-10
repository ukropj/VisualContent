/*
 * TextContent.cpp
 *
 *  Created on: 29.12.2010
 *      Author: jakub
 */

#include "Viewer/TextContent.h"
#include <QTextEdit>
#include <QWebView>

using namespace Vwr;

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
		view->setGeometry(QRect(QPoint(1, 1), size));
		view->load(QUrl(url));

		setWidget(view, 0.2f); 	// XXX magic number

		contentGeode->setName("node_content_web_page");
		return true;
	}
}
