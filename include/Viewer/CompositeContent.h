/*
 * CompositeContent.h
 *
 *  Created on: 7.4.2011
 *      Author: jakub
 */

#ifndef COMPOSITECONTENT_H_
#define COMPOSITECONTENT_H_

#include "Viewer/WidgetContent.h"

namespace Vwr {
class OsgCluster;

class CompositeContent : public WidgetContent {
public:
	CompositeContent(OsgCluster* cluster);
	~CompositeContent();
	bool load();

private:
	bool loaded;
	OsgCluster* cluster;
};

}

#endif /* COMPOSITECONTENT_H_ */
