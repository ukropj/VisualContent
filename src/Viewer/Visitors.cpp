/*
 * ExpanderVisitor.cpp
 *
 *  Created on: 31.1.2011
 *      Author: jakub
 */

#include "Viewer/Visitors.h"
#include "Viewer/AbstractNode.h"
#include <QDebug>

using namespace Vwr;

ExpanderVisitor::ExpanderVisitor(bool isExpanding) {
	this->isExpanding = isExpanding;
}

void ExpanderVisitor::visitNode(AbstractNode* node) {
	visited.insert(node);
	if (node->isExpanded() != isExpanding) {
		node->setExpanded(isExpanding);
	} else {
		node->setExpanded(isExpanding);
		QSet<AbstractNode*> nghbrs = node->getIncidentNodes();
		QSet<AbstractNode*>::const_iterator i = nghbrs.begin();
		while (i != nghbrs.end()) {
			if (!visited.contains((*i))) {
				(*i)->acceptVisitor(this);
			}
			++i;
		}
	}
}
