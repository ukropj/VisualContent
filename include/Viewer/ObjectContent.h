/*
 * ObjectContent.h
 *
 *  Created on: 29.12.2010
 *      Author: jakub
 */

#ifndef TEXTCONTENT_H_
#define TEXTCONTENT_H_

#include "Viewer/WidgetContent.h"

namespace Vwr {

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
