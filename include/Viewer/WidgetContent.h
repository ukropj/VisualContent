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

}

#endif /* WIDGETCONTENT_H_ */
