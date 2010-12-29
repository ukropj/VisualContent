/*
 * WidgetContent.h
 *
 *  Created on: 25.12.2010
 *      Author: jakub
 */

#ifndef WIDGETCONTENT_H_
#define WIDGETCONTENT_H_

#include "OsgContent.h"
#include <osg/Geode>
#include <QtGui/QWidget>

namespace Vwr {

class WidgetContent: public OsgContent {
public:
	WidgetContent();
//	WidgetContent(QString imagePath);
	~WidgetContent();

	void setWidget(QWidget* widget, float scale);

	virtual bool load();

};

}

#endif /* WIDGETCONTENT_H_ */
