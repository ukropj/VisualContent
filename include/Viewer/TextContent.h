/*
 * TextContent.h
 *
 *  Created on: 29.12.2010
 *      Author: jakub
 */

#ifndef TEXTCONTENT_H_
#define TEXTCONTENT_H_

#include "Viewer/WidgetContent.h"

namespace Vwr {

class TextContent: public Vwr::WidgetContent {
public:
	TextContent(QString text);
	~TextContent();
	bool load();
private:
	bool loaded;
	QString text;
};

}

#endif /* TEXTCONTENT_H_ */
