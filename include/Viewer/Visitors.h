#ifndef EXPANDERVISITOR_H_
#define EXPANDERVISITOR_H_

#include "Viewer/AbstractVisitor.h"
#include <QSet>

namespace Vwr{
class AbstractNode;

class ExpanderVisitor: public AbstractVisitor {
public:
	ExpanderVisitor(bool isExpanding);
	void visitNode(AbstractNode* node);

private:
	bool isExpanding;
	QSet<AbstractNode*> visited;
};

class ClusteringVisitor: public AbstractVisitor {
public:
	ClusteringVisitor(bool isExpanding);
	void visitNode(AbstractNode* node);

private:
	bool isExpanding;
};

}
#endif /* EXPANDERVISITOR_H_ */
