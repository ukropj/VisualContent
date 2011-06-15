#ifndef ABSTRACTVISITOR_H_
#define ABSTRACTVISITOR_H_

namespace Vwr {
class AbstractNode;

class AbstractVisitor {
public:
	AbstractVisitor() {}
	virtual ~AbstractVisitor() {}
	virtual void visitNode(AbstractNode* node) = 0;
};
}
#endif /* ABSTRACTVISITOR_H_ */
