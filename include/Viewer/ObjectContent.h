#ifndef TEXTCONTENT_H_
#define TEXTCONTENT_H_

#include "Viewer/WidgetContent.h"

namespace Vwr {

/// OsgContent implementation for displaying OSG geometry.
class ObjectContent : public OsgContent {
public:
	ObjectContent(QString osgPath);
	~ObjectContent() {}
	bool load();
private:
	bool loaded;
	QString osgPath;

};

}

#endif /* TEXTCONTENT_H_ */
