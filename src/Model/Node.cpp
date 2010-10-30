/*!
 * Node.cpp
 * Projekt 3DVisual
 *
 * TODO - reload configu sa da napisat aj efektivnejsie. Pri testoch na hranach priniesol vsak podobny prepis len male zvysenie vykonu. Teraz na to
 * nemam cas, takze sa raz k tomu vratim 8)
 */
#include "Model/Node.h"
#include "Model/Type.h"
#include "Model/Graph.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"

#include <osgWidget/Label>
#include <osgText/FadeText>

typedef osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>
		ColorIndexArray;
using namespace Model;

Node::Node(qlonglong id, QString name, Type* nodeType, Graph* graph,
		osg::Vec3f position) {
	this->id = id;
	setName(name);
	this->type = nodeType;
	this->targetPosition = position;
	this->currentPosition = position * Util::Config::getInstance()->getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();
	this->graph = graph;
	this->edges = new QMap<qlonglong, osg::ref_ptr<Edge> > ;

	int pos = 0;
	int cnt = 0;

	labelText = this->name;

	while ((pos = labelText.indexOf(QString(" "), pos + 1)) != -1) {
		if (++cnt % 3 == 0)
			labelText = labelText.replace(pos, 1, "\n");
	}

	this->force = osg::Vec3f();
	this->velocity = osg::Vec3f(0, 0, 0);
	this->fixed = false;
	this->ignore = false;
	this->frozen = false;
	this->selected = false;
	this->usingInterpolation = true;
	expanded = false;

	nodeSmall = createTextureNode(type->getTexture(), type->getScale());
	nodeLarge = createTextureNode(type->getDevil(), type->getScale() * 3);
	square = createSquare(type->getScale());
	label = createLabel(labelText, type->getScale());

	nodeSmall->setName("node");
	nodeLarge->setName("image");
	square->setName("square");
	label->setName("label");

	addChild(nodeSmall);
	addChild(nodeLarge);
	addChild(square);
	addChild(label);

	setAllChildrenOff();
	setChildValue(nodeSmall, true);

	float r = type->getValue("color.R").toFloat();
	float g = type->getValue("color.G").toFloat();
	float b = type->getValue("color.B").toFloat();
	float a = type->getValue("color.A").toFloat();
	this->setColor(osg::Vec4(r, g, b, a));
}

Node::~Node(void) {
	foreach(qlonglong i, edges->keys())
		{
			edges->value(i)->unlinkNodes();
		}
	edges->clear(); //staci to ?? netreba spravit delete/remove ???

	delete edges;
}

void Node::addEdge(osg::ref_ptr<Edge> edge) {
	edges->insert(edge->getId(), edge);
}

void Node::removeEdge(osg::ref_ptr<Edge> edge) {
	edges->remove(edge->getId());
}

void Node::removeAllEdges() {
	foreach(qlonglong i, edges->keys())
		{
			edges->value(i)->unlinkNodesAndRemoveFromGraph();
		}
	edges->clear();
}

osg::ref_ptr<osg::Geode> Node::createTextureNode(
		osg::ref_ptr<osg::Texture2D> texture, const float scale) {
	osg::ref_ptr<osg::StateSet> bbState = createStateSet();
	if (texture != NULL)
		bbState->setTextureAttributeAndModes(0, texture,
				osg::StateAttribute::ON);

	float width = 2.0f;
	float height = 2.0f;

	width *= scale;
	height *= scale;

	osg::ref_ptr<osg::Geometry> nodeQuad = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> nodeVerts = new osg::Vec3Array(4);

	(*nodeVerts)[0] = osg::Vec3(-width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[1] = osg::Vec3(width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[2] = osg::Vec3(width / 2.0f, height / 2.0f, 0);
	(*nodeVerts)[3] = osg::Vec3(-width / 2.0f, height / 2.0f, 0);

	nodeQuad->setUseDisplayList(false);

	nodeQuad->setVertexArray(nodeVerts);
	nodeQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,
			4));

	osg::ref_ptr<osg::Vec2Array> nodeTexCoords = new osg::Vec2Array(4);
	(*nodeTexCoords)[0].set(0.0f, 0.0f);
	(*nodeTexCoords)[1].set(1.0f, 0.0f);
	(*nodeTexCoords)[2].set(1.0f, 1.0f);
	(*nodeTexCoords)[3].set(0.0f, 1.0f);

	nodeQuad->setTexCoordArray(0, nodeTexCoords);

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	osg::ref_ptr<ColorIndexArray> colorIndexArray =
			new osg::TemplateIndexArray<unsigned int,
					osg::Array::UIntArrayType, 4, 1>;
	colorIndexArray->push_back(0);

	nodeQuad->setColorArray(colorArray);
	nodeQuad->setColorIndices(colorIndexArray);
	nodeQuad->setColorBinding(osg::Geometry::BIND_OVERALL);
	nodeQuad->setStateSet(bbState);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(nodeQuad);
	return geode;
}

osg::ref_ptr<osg::Geode> Node::createSquare(const float scale) {
	osg::ref_ptr<osg::StateSet> bbState = createStateSet();

	float width = 3.0f;
	float height = 3.0f;

	width *= scale;
	height *= scale;

	osg::ref_ptr<osg::Geometry> nodeRect = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> nodeVerts = new osg::Vec3Array(5);

	(*nodeVerts)[0] = osg::Vec3(-width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[1] = osg::Vec3(width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[2] = osg::Vec3(width / 2.0f, height / 2.0f, 0);
	(*nodeVerts)[3] = osg::Vec3(-width / 2.0f, height / 2.0f, 0);
	(*nodeVerts)[4] = osg::Vec3(-width / 2.0f, -height / 2.0f, 0);

	nodeRect->setVertexArray(nodeVerts);
	nodeRect->addPrimitiveSet(new osg::DrawArrays(
			osg::PrimitiveSet::LINE_STRIP, 0, 5));

	osg::ref_ptr<ColorIndexArray> colorIndexArray =
			new osg::TemplateIndexArray<unsigned int,
					osg::Array::UIntArrayType, 4, 1>;
	colorIndexArray->push_back(0);

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));

	nodeRect->setColorArray(colorArray);
	nodeRect->setColorIndices(colorIndexArray);

	nodeRect->setColorArray(colorArray);
	nodeRect->setColorBinding(osg::Geometry::BIND_OVERALL);

	nodeRect->setStateSet(bbState);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(nodeRect);
	return geode;
}

osg::ref_ptr<osg::Geode> Node::createLabel(QString text, const float scale) {
	osg::ref_ptr<osgText::FadeText> textDrawable = new osgText::FadeText();
	textDrawable->setFadeSpeed(0.03);

	QString fontPath = Util::Config::getInstance()->getValue(
			"Viewer.Labels.Font");

	// experimental value
	float newScale = 1.375f * scale;

	if (fontPath != NULL && !fontPath.isEmpty())
		textDrawable->setFont(fontPath.toStdString());

	textDrawable->setText(text.toStdString());
	textDrawable->setLineSpacing(0);
	textDrawable->setAxisAlignment(osgText::Text::SCREEN);
	textDrawable->setCharacterSize(newScale);
	textDrawable->setDrawMode(osgText::Text::TEXT);
	textDrawable->setAlignment(osgText::Text::CENTER_BOTTOM_BASE_LINE);
	textDrawable->setPosition(osg::Vec3(0, newScale, 0));
	textDrawable->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(textDrawable);
	return geode;
}

osg::ref_ptr<osg::StateSet> Node::createStateSet() {
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();

	stateSet->setDataVariance(osg::Object::DYNAMIC);
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	stateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

	return stateSet;
}

void Node::setNodeColor(int pos, osg::Vec4 color) {
	osg::Geometry * geometry1 =
			dynamic_cast<osg::Geometry *> (nodeSmall->getDrawable(pos));
	osg::Geometry * geometry2 =
			dynamic_cast<osg::Geometry *> (nodeLarge->getDrawable(pos));

	if (geometry1 != NULL) {
		osg::Vec4Array * colorArray =
				dynamic_cast<osg::Vec4Array *> (geometry1->getColorArray());
		colorArray->pop_back();
		colorArray->push_back(color);
	}
	if (geometry2 != NULL) {
		osg::Vec4Array * colorArray =
				dynamic_cast<osg::Vec4Array *> (geometry2->getColorArray());
		colorArray->pop_back();
		colorArray->push_back(color);
	}
}

bool Node::equals(Node* node) {
	if (this == node) {
		return true;
	}
	if (node == NULL) {
		return false;
	}
	if (id != node->getId()) {
		return false;
	}
	if ((graph == NULL && node->getGraph() != NULL) || (graph != NULL
			&& node->getGraph() == NULL)) {
		return false;
	}
	if (graph == NULL && node->getGraph() == NULL) {
		return true;
	}
	return true;
}

void Node::showLabel(bool visible) {
	setChildValue(label, visible);
}

bool Node::setExpanded(bool flag) {
	if (flag == isExpanded())
		return false;

	expanded = flag;
	setChildValue(nodeLarge, expanded);
	setChildValue(nodeSmall, !expanded);
	return true;
}

bool Node::setSelected(bool flag) {
	if (flag == isSelected())
		return false;

	selected = flag;
	if (selected)
		setNodeColor(0, osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f)); // red
	else {
		setNodeColor(0, color);
	}
}

bool Node::setFixed(bool flag) {
	if (flag == isFixed())
		return false;
	fixed = flag;
	setChildValue(square, fixed);
}

void Node::reloadConfig() {
	// TODO if needed at all
}

bool Node::isPickable(osg::Geode* geode) {
	if (geode->getName() == nodeSmall->getName() || geode->getName()
			== nodeLarge->getName())
		return true;
	else
		return false;
}

osg::Vec3f Node::getCurrentPosition(bool calculateNew, float interpolationSpeed) {
	if (calculateNew) {
		float graphScale = Util::Config::getInstance()->getValue(
				"Viewer.Display.NodeDistanceScale").toFloat();

		//		currentPosition = osg::Vec3(targetPosition * graphScale);
		osg::Vec3 directionVector = osg::Vec3(targetPosition.x(),
				targetPosition.y(), targetPosition.z()) * graphScale
				- currentPosition;
		currentPosition = osg::Vec3(directionVector
				* (usingInterpolation ? interpolationSpeed : 1)
				+ currentPosition);
	}

	return osg::Vec3(currentPosition);
}
