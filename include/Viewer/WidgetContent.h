#ifndef WIDGETCONTENT_H_
#define WIDGETCONTENT_H_

#include "Viewer/OsgContent.h"
#include <QtGui/QWidget>

namespace Vwr {

/// Content implementation for widgets.
class WidgetContent : public OsgContent {
public:
	WidgetContent();
	~WidgetContent();

	/// Used to set widget. Inserted widget is not interactive.
	void setWidget(QWidget* widget, float scale);

	virtual bool load() = 0;
private:
	QWidget* widget;
	bool loaded;
};

/// Content implementation for texts.
class TextContent: public WidgetContent {
public:
	TextContent(QString text, int width = 200, int height = 150);
	~TextContent();
	bool load();
private:
	bool loaded;
	QString text;
	int width;
	int height;
};

/// Content implementation for web pages. Contains simple browser.
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
