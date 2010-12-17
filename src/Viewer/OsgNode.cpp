/*
 * OsgNode.cpp
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgNode.h"
#include "Viewer/OsgContent.h"
#include "Viewer/SceneGraph.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"
#include "Model/Type.h"
#include "Model/Node.h"

#include <math.h>
#include <osgText/FadeText>
#include <osgText/Font>
#include <osg/LineWidth>
#include <qDebug>

using namespace Vwr;
typedef osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>
		ColorIndexArray;

osg::Vec4 OsgNode::selectedColor = osg::Vec4(0.0, 1.0, 0.0, 0.9);
osg::ref_ptr<osg::Geode> OsgNode::fixedG = NULL;

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
	pickable = true;
	float scale = node->getType()->getScale();

	closedG = createTextureNode(node->getType()->getTexture(), 2*scale, 2*scale);
	contentG = new OsgContent("img/devil.jpg");

	size = osg::Vec2f(0, 0);

	frameG = createFrame(contentG->getBoundingBox(), 0.2);
	label = createLabel(node->getLabel(), scale);
	if (fixedG == NULL)
		fixedG = createFixed();

	closedG->setName("node");
	contentG->setName("content");
	frameG->setName("frame");
	label->setName("label");

	addChild(closedG);
	addChild(contentG);
	addChild(label);
	addChild(frameG);
	addChild(fixedG);

	setAllChildrenOff();
	setChildValue(closedG, true);

	setSize(closedG->getBoundingBox());
	setColor(node->getType()->getColor());
}

OsgNode::~OsgNode(void) {
	node->setOsgNode(NULL);
	sceneGraph = NULL;
	delete contentG;
}

osg::ref_ptr<osg::Geode> OsgNode::createFrame(osg::BoundingBox box,
		float margin) {
	margin *= node->getType()->getScale();
	osg::Vec3f mx(margin, 0, 0);
	osg::Vec3f my(0, margin, 0);
	osg::Vec3 coords[] = { box.corner(0), box.corner(0) - mx - my,
			box.corner(1), box.corner(1) + mx - my, box.corner(3),
			box.corner(3) + mx + my, box.corner(2), box.corner(2) - mx + my,
			box.corner(0), box.corner(0) - mx - my, };

	osg::ref_ptr<osg::Geometry> frameQuad =
				createCustomGeometry(coords, 10, osg::PrimitiveSet::QUAD_STRIP, osg::Vec4f(1, 1, 1, 1));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->setStateSet(getOrCreateStateSet());
	geode->addDrawable(frameQuad);
	return geode;
}

osg::ref_ptr<osg::Geode> OsgNode::createContent() {
//	float scale = node->getType()->getScale();
//
//	int i = node->getId() % 10;
//	if (i == 9)
//		return createText(scale * 2);
//	if (i == 3) scale *= 1.5f;
//	if (i == 5) scale *= 1.8f;
//	return createTextureNode(node->getType()->getPiano(i),
//			2.0f*scale*2, 2.0f*scale*2);
	return NULL;
}

osg::ref_ptr<osg::Geode> OsgNode::createTextureNode(
		osg::ref_ptr<osg::Texture2D> texture, float width, float height) {
	width /= 2.0f;
	height /= 2.0f;
	osg::Vec3 coords[] = { osg::Vec3(-width, -height, 0),
			osg::Vec3(width, -height, 0),
			osg::Vec3(width, height, 0),
			osg::Vec3(-width, height, 0) };

	osg::ref_ptr<osg::Geometry> nodeQuad =
				createCustomGeometry(coords, 4, osg::PrimitiveSet::QUADS, osg::Vec4f(1, 1, 1, 1));

	osg::Vec2 texCoords[] = { osg::Vec2(0, 0), osg::Vec2(1, 0),
			osg::Vec2(1, 1), osg::Vec2(0, 1) };
	nodeQuad->setTexCoordArray(0, new osg::Vec2Array(4, texCoords));

	osg::ref_ptr<osg::StateSet> stateSet = createStateSet();
	if (texture != NULL)
		stateSet->setTextureAttributeAndModes(0, texture,
				osg::StateAttribute::ON);
	nodeQuad->setStateSet(stateSet);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(nodeQuad);
	return geode;
}

osg::ref_ptr<osg::Drawable> OsgNode::createRect(float width, float height,
		osg::Vec4f color) {
	width /= 2.0f;
	height /= 2.0f;
	osg::Vec3 coords[] = { osg::Vec3(-width, -height, 0), osg::Vec3(width,
			-height, 0), osg::Vec3(width, height, 0), osg::Vec3(-width, height,
			0), osg::Vec3(-width, -height, 0) };

	osg::ref_ptr<osg::Geometry> rectLine =
			createCustomGeometry(coords, 5, osg::PrimitiveSet::LINE_STRIP, color);

	return rectLine;
}

osg::ref_ptr<osg::Geode> OsgNode::createFixed() {
	float size = 2 * node->getType()->getScale();
	osg::Vec3 coords[] = { osg::Vec3(-size, -size, 0.1), osg::Vec3(size, size,
			0.1), osg::Vec3(-size, size, 0.1), osg::Vec3(size, -size, 0.1)};
	osg::ref_ptr<osg::Geometry> crossLine =
			createCustomGeometry(coords, 4, osg::PrimitiveSet::LINES, osg::Vec4f(0, 0, 0, 0.6));

	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(3.0f);
	osg::StateSet* stateSet = new osg::StateSet;
	stateSet->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
	crossLine->setStateSet(stateSet);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(crossLine);
	geode->setName("fixed");
	return geode;
}

osg::ref_ptr<osg::Geometry> OsgNode::createCustomGeometry(
		osg::Vec3 coords[], const int vertNum, GLenum mode, osg::Vec4 color) {
	osg::ref_ptr<osg::Geometry> g = new osg::Geometry;

	g->setVertexArray(new osg::Vec3Array(vertNum, coords));
	g->addPrimitiveSet(new osg::DrawArrays(mode, 0, vertNum));
	g->setUseDisplayList(false);

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(color);
	g->setColorArray(colorArray);
	g->setColorBinding(osg::Geometry::BIND_OVERALL);

	return g;
}

osg::ref_ptr<osg::Geode> OsgNode::createText(const float scale) {
	float width = 5.0f;
	float height = 5.0f;
	width *= scale;
	height *= scale;

	osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
	QString text("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce ut eros id augue ullamcorper fringilla at id est. Donec egestas congue pretium. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. ");
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
	geode->setStateSet(getOrCreateStateSet());
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

void OsgNode::setSize(osg::BoundingBox box) {
	setSize(box.xMax() - box.xMin(), box.yMax() - box.yMin());
}

void OsgNode::setSize(float width, float height) {
	size = osg::Vec2f(width, height);// * nodeTransform->getScale().x();
	node->setRadius(getRadius());
}

void OsgNode::setColor(osg::Vec4 color) {
	this->color = color;
	if (!isSelected()) {
		setDrawableColor(closedG, 0, color);
		setDrawableColor(frameG, 0, color);
	}
}

void OsgNode::setDrawableColor(osg::ref_ptr<osg::Geode> geode, int drawablePos,
		osg::Vec4 color) {
	osg::Geometry* geometry =
			dynamic_cast<osg::Geometry *> (geode->getDrawable(drawablePos));

	if (geometry != NULL) {
		osg::Vec4Array* colorArray =
				dynamic_cast<osg::Vec4Array *> (geometry->getColorArray());
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
	if (expanded) {
		setSize(frameG->getBoundingBox());
	} else {
		setSize(closedG->getBoundingBox());
	}
	setChildValue(frameG, expanded);
	contentG->load();
	setChildValue(contentG, expanded);

	setChildValue(closedG, !expanded);
	return true;
}

bool OsgNode::setSelected(bool flag) {
	if (flag == selected)
		return false;

	selected = flag;
	if (selected) {
		setDrawableColor(frameG, 0, selectedColor);
		setDrawableColor(closedG, 0, selectedColor);
	} else {
		setDrawableColor(frameG, 0, color);
		setDrawableColor(closedG, 0, color);
	}

	return true;
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
	setChildValue(fixedG, flag);
}

float OsgNode::getRadius() const {
	return size.length() / 2;
}

void OsgNode::reloadConfig() {
	// TODO if needed at all
}

bool OsgNode::isPickable(osg::Geode* geode) const {
	if (!pickable)
		return false;
	if (geode->getName() == closedG->getName() || geode->getName()
			== contentG->getName())
		return true;
	else
		return false;
}

bool OsgNode::isResizable(osg::Geode* geode) const {
	if (!pickable)
		return false;
	if (geode->getName() == frameG->getName())
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

	// if top left and botom right points are out of screen, node is not visible at all
	if (qAbs(pos.x()) > 1 || qAbs(pos.y()) > 1) {
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

	w = size.x();
	h = size.y();

	if (tan(angle) < -w / h || tan(angle) > w / h) {
		d = qAbs(w / (2 * sin(angle)));
	} else {
		d = qAbs(h / (2 * cos(angle)));
	}
	//	qDebug() << angle * 360 / (osg::PI * 2.0f) << "\t-> " << d;
	return d;
}

QString OsgNode::toString() const {
	QString str;
	osg::Vec3f pos = getPosition();
	QTextStream(&str) << "N" << node->getId() << " " << node->getName() << "["
			<< pos.x() << "," << pos.y() << "," << pos.z() << "]"
			<< (isFixed() ? "fixed" : "");
	return str;
}

