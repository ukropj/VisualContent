/*!
 * Graph.cpp
 * Projekt 3DVisual
 */
#include "Model/Graph.h"

using namespace Model;

Graph::Graph(QString name, qlonglong ele_id_counter) {
	this->name = name;
	this->ele_id_counter = ele_id_counter;

	//pre Misa
	this->nodes = new QMap<qlonglong, osg::ref_ptr<Node> > ();
	this->edges = new QMap<qlonglong, osg::ref_ptr<Edge> > ();
	this->types = new QMap<qlonglong, Type*> ();
	this->metaEdges = new QMap<qlonglong, osg::ref_ptr<Edge> > ();
	this->metaNodes = new QMap<qlonglong, osg::ref_ptr<Node> > ();
	this->frozen = false;
	this->typesByName = new QMultiMap<QString, Type*> ();
}

Graph::~Graph(void) {
	//uvolnime vsetky Nodes, Edges, metaNodes, metaEdges... su cez osg::ref_ptr takze staci clearnut
	this->nodes->clear();
	delete this->nodes;
	this->nodes = NULL;

	this->metaNodes->clear();
	delete this->metaNodes;
	this->metaNodes = NULL;

	this->edges->clear();
	delete this->edges;
	this->edges = NULL;

	this->metaEdges->clear();
	delete this->metaEdges;
	this->metaEdges = NULL;

	this->newTypes.clear();
	this->newNodes.clear();
	this->newEdges.clear();
	this->metaEdgesByType.clear();
	this->metaNodesByType.clear();
	this->nodesByType.clear();
	this->edgesByType.clear();

	this->typesByName->clear();
	delete this->typesByName;
	this->typesByName = NULL;

	//uvolnime types - treba iterovat a kazde jedno deletnut samostatne
	QMap<qlonglong, Type*>::iterator it = this->types->begin();

	Type* type;
	while (it != this->types->end()) {
		type = it.value();
		it = this->types->erase(it);
		delete type;
		type = NULL;
	}

	this->types->clear();
	delete this->types;
	this->types = NULL;
}

QString Graph::setName(QString name) {
	QString newName = name;

	if (newName != NULL) {
		this->name = newName;
	}

	return this->name;
}

osg::ref_ptr<Node> Graph::addNode(QString name, Type* type,
		osg::Vec3f position) {
	osg::ref_ptr<Node> node = new Node(this->incEleIdCounter(),
			name, type, this, position);

	this->newNodes.insert(node->getId(), node);
	if (type != NULL && type->isMeta()) {
		this->metaNodes->insert(node->getId(), node);
		this->metaNodesByType.insert(type->getId(), node);
	} else {
		this->nodes->insert(node->getId(), node);
		this->nodesByType.insert(type->getId(), node);
	}

	return node;
}

osg::ref_ptr<Edge> Graph::addEdge(QString name,
		osg::ref_ptr<Node> srcNode, osg::ref_ptr<Node> dstNode,
		Type* type, bool isOriented) {
	osg::ref_ptr<Edge> edge = new Edge(this->incEleIdCounter(),
			name, this, srcNode, dstNode, type, isOriented);

	edge->linkNodes(&this->newEdges);
	if ((type != NULL && type->isMeta()) || ((srcNode->getType() != NULL
			&& srcNode->getType()->isMeta()) || (dstNode->getType() != NULL
			&& dstNode->getType()->isMeta()))) {
		//ak je type meta, alebo je meta jeden z uzlov (ma type meta)
		edge->linkNodes(this->metaEdges);
		this->metaEdgesByType.insert(type->getId(), edge);
	} else {
		edge->linkNodes(this->edges);
		this->edgesByType.insert(type->getId(), edge);
	}

	return edge;
}

Type* Graph::addType(QString name, QMap<QString, QString> *settings) {
	Type* type = new Type(this->incEleIdCounter(), name, this,
			settings);

	this->newTypes.insert(type->getId(), type);
	this->types->insert(type->getId(), type);
	this->typesByName->insert(type->getName(), type);

	return type;
}

qlonglong Graph::getMaxEleIdFromElements() {
	qlonglong max = 0;

	if (this->nodes != NULL && !this->nodes->isEmpty()) {
		QMap<qlonglong, osg::ref_ptr<Node> >::iterator iNodes =
				this->nodes->end();
		iNodes--;

		if (iNodes.key() > max)
			max = iNodes.key();
	}

	if (this->nodes != NULL && !this->types->isEmpty()) {
		QMap<qlonglong, Type*>::iterator iTypes = this->types->end();
		iTypes--;

		if (iTypes.key() > max)
			max = iTypes.key();
	}

	if (this->nodes != NULL && !this->edges->isEmpty()) {
		QMap<qlonglong, osg::ref_ptr<Edge> >::iterator iEdges =
				this->edges->end();
		iEdges--;

		if (iEdges.key() > max)
			max = iEdges.key();
	}

	if (this->nodes != NULL && !this->metaNodes->isEmpty()) {
		QMap<qlonglong, osg::ref_ptr<Node> >::iterator iMetaNodes =
				this->metaNodes->end();
		iMetaNodes--;

		if (iMetaNodes.key() > max)
			max = iMetaNodes.key();
	}

	if (this->nodes != NULL && !this->metaEdges->isEmpty()) {
		QMap<qlonglong, osg::ref_ptr<Edge> >::iterator iMetaEdges =
				this->metaEdges->end();
		iMetaEdges--;

		if (iMetaEdges.key() > max)
			max = iMetaEdges.key();
	}

	return max;
}

QString Graph::toString() const {
	QString str;
	QTextStream(&str) << " name:" << name << " max current ele ID:"
			<< ele_id_counter;
	return str;
}

QList<Type*> Graph::getTypesByName(QString name) {
	return this->typesByName->values(name);
}

Type* Graph::getNodeMetaType() {
	return NULL;
	/*
	 if(this->selectedLayout->getMetaSetting(Graph::GraphLayout::META_NODE_TYPE) == NULL)
	 {
	 QMap<QString, QString> *settings = new QMap<QString, QString>;

	 settings->insert("scale", Util::Config::getValue("Viewer.Textures.DefaultNodeScale"));
	 settings->insert("textureFile", Util::Config::getValue("Viewer.Textures.MetaNode"));
	 settings->insert("color.R", "0.8");
	 settings->insert("color.G", "0.1");
	 settings->insert("color.B", "0.1");
	 settings->insert("color.A", "0.8");

	 Graph::MetaType* type = this->addMetaType(Graph::GraphLayout::META_NODE_TYPE, settings);
	 this->selectedLayout->setMetaSetting(Graph::GraphLayout::META_NODE_TYPE,QString::number(type->getId()));
	 return type;
	 } else {
	 qlonglong typeId = this->selectedLayout->getMetaSetting(Graph::GraphLayout::META_NODE_TYPE).toLongLong();
	 if(this->types->contains(typeId)) return this->types->value(typeId);
	 return NULL;
	 }
	 */
}

Type* Graph::getEdgeMetaType() {
	return NULL;

	/*
	 if(this->selectedLayout->getMetaSetting(Graph::GraphLayout::META_EDGE_TYPE)==NULL) {
	 Graph::MetaType* type = this->addMetaType(Graph::GraphLayout::META_EDGE_TYPE);
	 this->selectedLayout->setMetaSetting(Graph::GraphLayout::META_EDGE_TYPE,QString::number(type->getId()));
	 return type;
	 } else {
	 qlonglong typeId =  this->selectedLayout->getMetaSetting(Graph::GraphLayout::META_EDGE_TYPE).toLongLong();
	 if(this->types->contains(typeId)) return this->types->value(typeId);
	 return NULL;
	 }
	 */
}

void Graph::removeType(Type* type) {
	if (type != NULL && type->getGraph() == this) {
		this->types->remove(type->getId());
		this->newTypes.remove(type->getId());
		this->typesByName->remove(type->getName());

		/*
		 if(type->isMeta()) {
		 if(this->getNodeMetaType()==type) { //type je MetaTypom uzlov layoutu
		 this->selectedLayout->removeMetaSetting(Graph::GraphLayout::META_NODE_TYPE);
		 }
		 if(this->getEdgeMetaType()==type) { //type je MetaTypom hran layoutu
		 this->selectedLayout->removeMetaSetting(Graph::GraphLayout::META_EDGE_TYPE);
		 }
		 }*/

		//vymazeme vsetky hrany resp. metahrany daneho typu
		this->removeAllEdgesOfType(type);

		//vymazeme vsetky uzly daneho typu
		this->removeAllNodesOfType(type);

	}
}

void Graph::removeAllEdgesOfType(Type* type) {
	QList<osg::ref_ptr<Edge> > edgesToKill;
	if (type->isMeta())
		edgesToKill = this->metaEdgesByType.values(type->getId());
	else
		edgesToKill = this->edgesByType.values(type->getId());
	if (!edgesToKill.isEmpty()) {
		for (qlonglong i = 0; i < edgesToKill.size(); i++) {
			this->removeEdge(edgesToKill.at(i));
		}

		edgesToKill.clear();
	}
}

void Graph::removeAllNodesOfType(Type* type) {
	QList<osg::ref_ptr<Node> > nodesToKill;
	if (type->isMeta())
		nodesToKill = this->metaNodesByType.values(type->getId()); //vyberieme vsetky uzly daneho typu
	else
		nodesToKill = this->nodesByType.values(type->getId());

	if (!nodesToKill.isEmpty()) {
		for (qlonglong i = 0; i < nodesToKill.size(); i++) { //prejdeme kazdy jeden uzol
			this->removeNode(nodesToKill.at(i));
		}

		nodesToKill.clear();
	}
}

void Graph::removeEdge(osg::ref_ptr<Edge> edge) {
	if (edge != NULL && edge->getGraph() == this) {
		this->edges->remove(edge->getId());
		this->metaEdges->remove(edge->getId());
		this->newEdges.remove(edge->getId());
		this->edgesByType.remove(edge->getType()->getId(), edge);
		this->metaEdgesByType.remove(edge->getType()->getId(), edge);

		edge->unlinkNodes();
	}

}

void Graph::removeNode(osg::ref_ptr<Node> node) {
	if (node != NULL && node->getGraph() == this) {

		this->nodes->remove(node->getId());
		this->metaNodes->remove(node->getId());
		this->newNodes.remove(node->getId());
		this->nodesByType.remove(node->getType()->getId(), node);
		this->metaNodesByType.remove(node->getType()->getId(), node);

		node->removeAllEdges();

		//zistime ci nahodou dany uzol nie je aj typom a osetrime specialny pripad ked uzol je sam sebe typom (v DB to znamena, ze uzol je ROOT uzlom/typom, teda uz nemoze mat ziaden iny typ)
		if (this->types->contains(node->getId())) {
			this->removeType(this->types->value(node->getId()));
		}

	}
}
