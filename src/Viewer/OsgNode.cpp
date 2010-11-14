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
	usingInterpolation = true;
	expanded = false;

	float scale = node->getType()->getScale();
	nodeSmall = createTextureNode(node->getType()->getTexture(), scale);
	nodeLarge = createTextureNode(node->getType()->getDevil(), scale * 2);
	square = createSquare(scale);
	circle = createCircle(scale * 2);
	label = createLabel(node->getLabel(), scale);

	nodeSmall->setName("node");
	nodeLarge->setName("image");
	label->setName("label");
	square->setName("square");
	circle->setName("circle");

	addChild(nodeSmall);
	addChild(nodeLarge);
	addChild(square);
	addChild(circle);
	addChild(label);

	setAllChildrenOff();
	setChildValue(nodeSmall, true);

	setColor(node->getType()->getColor());
}

OsgNode::~OsgNode(void) {
	// TODO
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

osg::ref_ptr<osg::Geode> OsgNode::createSquare(const float scale) {
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

osg::ref_ptr<osg::Geode> OsgNode::createCircle(const float scale) {
	osg::ref_ptr<osg::StateSet> bbState = createStateSet();

	float r = sqrt(2);
	r *= scale;

	int sides = 20;
	osg::ref_ptr<osg::Geometry> nodeCircle = new osg::Geometry;
	double alpha = 2 * M_PI / (float)sides;

	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	for (int i = 0; i <= sides; i++) {
		coords->push_back(osg::Vec3f(r * sin(i * alpha), r * cos(i * alpha), 0));
	}

	nodeCircle->setVertexArray(coords);
	nodeCircle->addPrimitiveSet(new osg::DrawArrays(
			osg::PrimitiveSet::LINE_STRIP, 0, sides + 1));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	nodeCircle->setColorArray(colorArray);
	nodeCircle->setColorBinding(osg::Geometry::BIND_OVERALL);

	nodeCircle->setStateSet(bbState);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(nodeCircle);
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
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	stateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

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
	setChildValue(circle, expanded);
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
	setChildValue(square, flag);
}

float OsgNode::getRadius() const {
	if (getChildValue(nodeLarge)) {
		return nodeLarge->getBound().radius();
	} else {
		return 0;//nodeSmall->getBound().radius(); // XXX temp magic
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
	if (nodeTransform == NULL)
		return;

	float graphScale = Util::Config::getInstance()->getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	osg::Vec3f targetPos = node->getPosition();
	osg::Vec3f currentPos = getPosition();
	osg::Vec3 directionVector = osg::Vec3(targetPos.x(), targetPos.y(),
			targetPos.z()) * graphScale - currentPos;

	if (usingInterpolation)
		directionVector *= interpolationSpeed;

	nodeTransform->setPosition(currentPos + directionVector);
}

void OsgNode::setPosition(osg::Vec3f pos) {
	float graphScale = Util::Config::getInstance()->getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();
	node->setPosition(pos / graphScale);
	updatePosition();
}

bool OsgNode::isObscuredBy(OsgNode* other) {
	bool uExp = this->isExpanded();
	bool vExp = other->isExpanded();
	// return false if no node is expanded
	if (!uExp && !vExp)
		return false;

	osg::Camera* camera = sceneGraph->getCamera();

	osg::Matrixd viewM = camera->getViewMatrix();
	osg::Matrixd projM = camera->getProjectionMatrix();
	osg::Matrixd windM = camera->getViewport()->computeWindowMatrix();

	osg::Vec3f up = this->getPosition();
	osg::Vec3f vp = other->getPosition();

	// test if the expanded node is befind the other node
	osg::Vec3f upRot = viewM.getRotate() * up;
	osg::Vec3f vpRot = viewM.getRotate() * vp;
	if (!(uExp && upRot.z() <= vpRot.z()) && !(vExp && vpRot.z() <= upRot.z()))
		return false;
	// NOTE: z decreases when going away from camera

	// return false if any of nodes is not on screen
	upRot = up * (viewM * projM);
	vpRot = vp * (viewM * projM);
	if (qAbs(upRot.x()) > 1 || qAbs(upRot.y()) > 1 || qAbs(vpRot.x()) > 1
			|| qAbs(vpRot.y()) > 1)
		return false;

	return true;
}

