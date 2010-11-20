/*
 * OsgNode.cpp
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgNode.h"
#include "Viewer/SceneGraph.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"
#include "Model/Type.h"
#include "Model/Node.h"

#include <math.h>
#include <osgText/FadeText>
#include <osgText/Font>
#include <qDebug>

using namespace Vwr;
typedef osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>
		ColorIndexArray;
//osg::ref_ptr<osg::Geode> OsgNode::square = NULL;
//osg::ref_ptr<osg::Geode> OsgNode::circle = NULL;

OsgNode::OsgNode(Model::Node* node, SceneGraph* sceneGraph, osg::ref_ptr<
		osg::AutoTransform> nodeTransform) {

	this->node = node;
	node->setOsgNode(this);
	setName(node->getName().toStdString());
	this->sceneGraph = sceneGraph;
	this->nodeTransform = nodeTransform;

	selected = false;
	expanded = false;
	usingInterpolation = true;
	float scale = node->getType()->getScale();

	nodeSmall = createTextureNode(node->getType()->getTexture(), scale);
	if (node->getId() % 2)
		nodeLarge = createTextureNode(node->getType()->getDevil(), scale * 2);
	else
		nodeLarge = createText(scale * 2);
	square = createSquare(scale);
	circle = createCircle(nodeLarge->getBound().radius());
	label = createLabel(node->getLabel(), scale);

	nodeSmall->setName("node");
	nodeLarge->setName("image");
	//	textSample->setName("text_sample");
	label->setName("label");
	square->setName("square");
	circle->setName("circle");

	addChild(nodeSmall);
	addChild(nodeLarge);
	//	addChild(textSample);
	addChild(square);
	addChild(circle);
	addChild(label);

	setAllChildrenOff();
	setChildValue(nodeSmall, true);

	setColor(node->getType()->getColor());
}

OsgNode::~OsgNode(void) {
	node->setOsgNode(NULL);
	sceneGraph = NULL;
}

osg::ref_ptr<osg::Geode> OsgNode::createTextureNode(
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
	osg::Vec3 coords[] = { osg::Vec3(-width / 2.0f, -height / 2.0f, 0),
			osg::Vec3(width / 2.0f, -height / 2.0f, 0),
			osg::Vec3(width / 2.0f,	height / 2.0f, 0),
			osg::Vec3(-width / 2.0f, height / 2.0f, 0) };

	nodeQuad->setUseDisplayList(false);

	nodeQuad->setVertexArray(new osg::Vec3Array(4, coords));
	nodeQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,
			4));

	osg::Vec2 texCoords[] = {
	        osg::Vec2(0,0),
	        osg::Vec2(1,0),
	        osg::Vec2(1,1),
	        osg::Vec2(0,1)
	    };
	nodeQuad->setTexCoordArray(0,new osg::Vec2Array(4, texCoords));

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

osg::ref_ptr<osg::Geode> OsgNode::createSquare(const float scale) {
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
	nodeRect->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(nodeRect);
	return geode;
}

osg::ref_ptr<osg::Geode> OsgNode::createCircle(const float radius) {
	float r = radius;

	int sides = 12;
	osg::ref_ptr<osg::Geometry> nodeCircle = new osg::Geometry;
	double alpha = 2 * osg::PI / (float) sides;

	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	for (int i = 0; i <= sides; i++) {
		float angle = i * alpha;
		coords->push_back(osg::Vec3f(r * sin(angle), r * cos(angle), 1.0f));
	}

	nodeCircle->setVertexArray(coords);
	nodeCircle->addPrimitiveSet(new osg::DrawArrays(
			osg::PrimitiveSet::LINE_STRIP, 0, sides + 1));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	nodeCircle->setColorArray(colorArray);
	nodeCircle->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(nodeCircle);
	return geode;
}

osg::ref_ptr<osg::Geode> OsgNode::createText(const float scale) {
	float width = 5.0f;
	float height = 5.0f;
	width *= scale;
	height *= scale;

	osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
	QString
			text(
					"2. If someone wants to bring Qt widgets inside their OSG scene (to do HUDs or \nan interface on a computer screen which is inside the 3D scene, or even \nfloating Qt widgets, for example). That's where QGraphicsViewAdapter +\nQWidgetImage will be useful.");
	text.append("A tento text sa tam uz nezmestil....");
	osg::ref_ptr<osgText::Text> textD = new osgText::Text();

	float charSize = 5.0f;
	float margin = 0.2f;

	textD->setFont(font);
	textD->setMaximumHeight(height);
	textD->setMaximumWidth(width);

	textD->setText(text.toStdString());
	textD->setLineSpacing(0.2);
	//	textDrawable->setAxisAlignment(osgText::Text::SCREEN);
	textD->setCharacterSize(charSize);
	textD->setDrawMode(osgText::Text::TEXT);// | osgText::Text::BOUNDINGBOX);
	textD->setAlignment(osgText::Text::LEFT_CENTER);
	textD->setPosition(osg::Vec3(-width / 2.0f, 0, 0));
	textD->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

	width += margin * 2;
	height += margin * 2;

	osg::ref_ptr<osg::Geometry> nodeRect = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> nodeVerts = new osg::Vec3Array(4);

	(*nodeVerts)[0] = osg::Vec3(-width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[1] = osg::Vec3(width / 2.0f, -height / 2.0f, 0);
	(*nodeVerts)[2] = osg::Vec3(width / 2.0f, height / 2.0f, 0);
	(*nodeVerts)[3] = osg::Vec3(-width / 2.0f, height / 2.0f, 0);

	nodeRect->setVertexArray(nodeVerts);
	nodeRect->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,
			4));
	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	nodeRect->setColorArray(colorArray);
	nodeRect->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(textD);
	geode->addDrawable(nodeRect);
	return geode;
}

osg::ref_ptr<osg::Geode> OsgNode::createLabel(QString text, const float scale) {
	osg::ref_ptr<osgText::FadeText> textDrawable = new osgText::FadeText();
	textDrawable->setFadeSpeed(0.04);

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

osg::ref_ptr<osg::StateSet> OsgNode::createStateSet() {
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();

	stateSet->setDataVariance(osg::Object::DYNAMIC);
	//	stateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	return stateSet;
}

void OsgNode::setColor(osg::Vec4 color) {
	this->color = color;
	if (!isSelected())
		setDrawableColor(color);
}

void OsgNode::setDrawableColor(osg::Vec4 color) {
	osg::Geometry * geometry1 =
			dynamic_cast<osg::Geometry *> (nodeSmall->getDrawable(0));
	osg::Geometry * geometry2 =
			dynamic_cast<osg::Geometry *> (circle->getDrawable(0));

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

void OsgNode::showLabel(bool visible) {
	setChildValue(label, visible);
}

bool OsgNode::setExpanded(bool flag) {
	if (flag == isExpanded())
		return false;

	expanded = flag;
	//	setChildValue(circle, expanded);
	setChildValue(nodeLarge, expanded);
	setChildValue(nodeSmall, !expanded);
	return true;
}

bool OsgNode::setSelected(bool flag) {
	if (flag == selected)
		return false;

	selected = flag;
	if (selected)
		setDrawableColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f)); // red
	else {
		setDrawableColor(color);
	}

	//	graph->setFrozen(false);
}

qlonglong OsgNode::getId() const {
	return node->getId();
}

QString OsgNode::getName() const {
	return node->getName();
}

void OsgNode::setFrozen(bool flag) {
	node->setFrozen(flag);
	setUsingInterpolation(!flag);
}

bool OsgNode::isFrozen() const {
	return node->isFrozen();
}

bool OsgNode::isFixed() const {
	return node->isFixed();
}

bool OsgNode::setFixed(bool flag) {
	if (flag == isFixed())
		return false;
	node->setFixed(flag);
	if (flag) {
		node->setPosition(getPosition());
	}
	setChildValue(square, flag);
}

float OsgNode::getRadius() const {
	if (getChildValue(nodeLarge)) {
		return nodeLarge->getBound().radius();
	} else {
		return nodeSmall->getBound().radius();
	}
}

void OsgNode::reloadConfig() {
	// TODO if needed at all
}

bool OsgNode::isPickable(osg::Geode* geode) const {
	if (geode->getName() == nodeSmall->getName() || geode->getName()
			== nodeLarge->getName())
		return true;
	else
		return false;
}

osg::Vec3f OsgNode::getPosition() const {
	if (nodeTransform == NULL)
		return osg::Vec3f(0, 0, 0);
	return nodeTransform->getPosition();
}

void OsgNode::updatePosition(float interpolationSpeed) {
	osg::Vec3f targetPos = node->getPosition();
	osg::Vec3f currentPos = getPosition();
	osg::Vec3 directionVector = osg::Vec3(targetPos - currentPos);

	if (usingInterpolation)
		directionVector *= interpolationSpeed;

	nodeTransform->setPosition(currentPos + directionVector);
}

void OsgNode::setPosition(osg::Vec3f pos) {
	node->setPosition(pos);
	updatePosition();
}

bool OsgNode::isOnScreen() const {
	osg::Vec3f pos = node->getPosition();
	pos = sceneGraph->byProjectionInv(sceneGraph->byView(pos));

	if (qAbs(pos.x()) > 1 || qAbs(pos.y()) > 1) {
		//		qDebug() << "not on screen";
		return false;
	}
	return true;
}

osg::Vec3f OsgNode::getEye() const {
	return sceneGraph->getEye();
}

osg::Vec3f OsgNode::getUpVector() const {
	return sceneGraph->getUpVector();
}

float OsgNode::getDistanceToEdge(double angle) {
	float w, h, d;

	if (isExpanded()) {
		if (node->getId() % 2) // todo size !!
			w = h = 2 * node->getType()->getScale() * 2;
		else
			w = h = 5 * node->getType()->getScale() * 2;
	} else {
		w = h = 2 * node->getType()->getScale();
	}

	if (tan(angle) < -w / h || tan(angle) > w / h) {
		d = qAbs(w / (2 * sin(angle)));
	} else {
		d = qAbs(h / (2 * cos(angle)));
	}
	//	qDebug() << angle * 360 / (osg::PI * 2.0f) << "\t-> " << d;
	return d;
}

