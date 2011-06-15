#ifndef IMAGECONTENT_H_
#define IMAGECONTENT_H_

#include "Viewer/WidgetContent.h"

namespace Vwr {

/// Content implementation for images.
class ImageContent: public Vwr::OsgContent {
public:
	ImageContent(QString imagePath);
	~ImageContent();

	bool load();

private:
	osg::ref_ptr<osg::Geometry> createGeometry(QString imagePath);
	QString imagePath;
	bool loaded;
};

}

#endif /* IMAGECONTENT_H_ */
