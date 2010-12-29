/*
 * ImageContent.h
 *
 *  Created on: 29.12.2010
 *      Author: jakub
 */

#ifndef IMAGECONTENT_H_
#define IMAGECONTENT_H_

#include "Viewer/WidgetContent.h"

namespace Vwr {

class ImageContent: public Vwr::WidgetContent {
public:
	ImageContent(QString imagePath);
	~ImageContent();

	bool load();
private:
	QString imagePath;
	bool loaded;
};

}

#endif /* IMAGECONTENT_H_ */
