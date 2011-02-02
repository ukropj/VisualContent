/*
 * ExpanderVisitor.h
 *
 *  Created on: 31.1.2011
 *      Author: jakub
 */

#ifndef EXPANDERVISITOR_H_
#define EXPANDERVISITOR_H_

#include "Viewer/AbstractVisitor.h"
#include <QSet>

namespace Vwr{
class AbstractNode;

class ExpanderVisitor: public AbstractVisitor {
public:
	ExpanderVisitor(bool isExpanding);
	~ExpanderVisitor();
	void visitNode(AbstractNode* node);

private:
	bool isExpanding;
	QSet<AbstractNode*> visited;
};
}
#endif /* EXPANDERVISITOR_H_ */
