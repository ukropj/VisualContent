/*
 * WidgetContent.h
 *
 *  Created on: 25.12.2010
 *      Author: jakub
 */

#ifndef WIDGETCONTENT_H_
#define WIDGETCONTENT_H_

#include "Viewer/OsgContent.h"
#include <QtGui/QWidget>

namespace Vwr {

class WidgetContent : public OsgContent {
public:
	WidgetContent();
//	WidgetContent(QString imagePath);
	~WidgetContent();

	void setWidget(QWidget* widget, float scale);

	virtual bool load() = 0;
private:
	bool loaded;
};

class TextContent: public WidgetContent {
public:
	TextContent(QString text);
	~TextContent();
	bool load();
private:
	bool loaded;
	QString text;
};

class WebContent: public WidgetContent {
public:
	WebContent(QString text);
	~WebContent();
	bool load();
private:
	bool loaded;
	QString url;
};

}

#endif /* WIDGETCONTENT_H_ */
