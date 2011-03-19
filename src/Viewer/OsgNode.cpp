/*
 * OsgNode.cpp
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgNode.h"
#include "Viewer/OsgContent.h"
#include "Viewer/OsgCluster.h"
#include "Viewer/AbstractVisitor.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"
#include "Util/CameraHelper.h"
#include "Model/Type.h"
#include "Model/Node.h"

#include <math.h>
#include <osgText/FadeText>
#include <osgText/Font>
#include <osg/LineWidth>
#include <qDebug>

using namespace Vwr;

uint OsgNode::MASK_ON = 0xffffffff;
uint OsgNode::MASK_OFF = 0x0;

OsgNode::OsgNode(Model::Node* node, DataMapping* dataMapping) {
	if (node == NULL) qWarning() << "NULL reference to Node in OsgNode!";
	this->node = node;
	node->setOsgNode(this);
	setName("node" + node->getId());
	setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	this->mapping = (dataMapping != NULL) ? dataMapping : new DataMapping();

	selected = false;
	expanded = false;
	usingInterpolation = true;
	pickable = true;
	visible = true;
	maxScale = Util::Config::getValue("Viewer.Node.MaxScale").toFloat();
	float scale = node->getType()->getScale();

	closedG = createTextureNode(Util::TextureWrapper::getNodeTexture(),
			2*scale, 2*scale); // TODO scale :((
	visualContent = ContentFactory::createContent(mapping->getContentType(), getMappingValue(DataMapping::CONTENT));

	size = osg::Vec3f(0, 0, 0);

	frameG = initFrame();
	labelG = createLabel(getMappingValue(DataMapping::LABEL));
	fixedG = createFixed();

	closedG->setName("closed_node");
	visualContent->setName("visual_content");
	frameG->setName("frame");
	labelG->setName("label");

	frameG->setNodeMask(MASK_OFF);
	labelG->setNodeMask(MASK_OFF);
	fixedG->setNodeMask(MASK_OFF);

	contentSwitch = new osg::Switch();
	contentSwitch->setName("content");
	contentSwitch->addChild(closedG);
	contentSwitch->addChild(visualContent);
	contentSwitch->setChildValue(closedG, true);
	contentSwitch->setChildValue(visualContent, false);

	addChild(contentSwitch);
	addChild(labelG);
	addChild(frameG);
	addChild(fixedG);

	setSize(closedG->getBoundingBox());
	setColor(mapping->getColor(getMappingValue(DataMapping::COLOR)));

	if (node->isIgnored())
		setVisible(false);
}

OsgNode::~OsgNode() {
	node->setOsgNode(NULL);
//	qDebug() << "OsgNode deleted";
}

void OsgNode::setDataMapping(DataMapping* dataMapping) {
	this->mapping = (dataMapping != NULL) ? dataMapping : new DataMapping();
	// change label
	osgText::FadeText* ft = dynamic_cast<osgText::FadeText*>(labelG->getDrawable(0));
	ft->setText(getMappingValue(DataMapping::LABEL).toStdString());
	// change content
	bool f = isExpanded();
	setExpanded(false);
	contentSwitch->removeChild(visualContent);
	visualContent = ContentFactory::createContent(mapping->getContentType(), getMappingValue(DataMapping::CONTENT));
	contentSwitch->addChild(visualContent);
	setExpanded(f);
	// change color
	setColor(mapping->getColor(getMappingValue(DataMapping::COLOR)));
}

QString OsgNode::getMappingValue(DataMapping::ValueType prop) {
	return node->data(mapping->getMapping(prop));
}

osg::ref_ptr<osg::Geode> OsgNode::initFrame() {
	osg::Vec3 v(0,0,0);
	osg::Vec3 coords[] = {
			v,v,v,v,v,
			v,v,v,v,v};

	osg::ref_ptr<osg::Geometry> frameQuad =
			createCustomGeometry(coords, 10, osg::PrimitiveSet::QUAD_STRIP, osg::Vec4f(0,0,0,1));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->setStateSet(getOrCreateStateSet());
	geode->addDrawable(frameQuad);
	return geode;
}

// TODO refactor
void OsgNode::updateFrame(osg::ref_ptr<osg::Geode> frame, osg::BoundingBox box, float scale, float margin) {
	osg::Geometry* geometry =
			dynamic_cast<osg::Geometry *> (frame->getDrawable(0));

	if (geometry != NULL) {
		margin *= node->getType()->getScale();
		osg::Vec3f mx(margin, 0, 0);
		osg::Vec3f my(0, margin, 0);
		osg::Vec3 coords[] = {
				box.corner(0)*scale, box.corner(0)*scale - mx - my,
				box.corner(1)*scale, box.corner(1)*scale + mx - my,
				box.corner(3)*scale, box.corner(3)*scale + mx + my,
				box.corner(2)*scale, box.corner(2)*scale - mx + my,
				box.corner(0)*scale, box.corner(0)*scale - mx - my, };

		geometry->setVertexArray(new osg::Vec3Array(10, coords));
	}
}

osg::ref_ptr<osg::Geode> OsgNode::createTextureNode(
		osg::ref_ptr<osg::Texture2D> texture, float width, float height) {
	//	osg::Geometry* nodeQuad = osg::createTexturedQuadGeometry(
	//			osg::Vec3(-width/2.0f, -height/2.0f, 0),
	//			osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);

	width /= 2.0f;
	height /= 2.0f;
	osg::Vec3 coords[] = { osg::Vec3(-width, -height, 0),
			osg::Vec3(width, -height, 0),
			osg::Vec3(width, height, 0),
			osg::Vec3(-width, height, 0) };


	osg::ref_ptr<osg::Geometry> nodeQuad =
			createCustomGeometry(coords, 4, osg::PrimitiveSet::QUADS, osg::Vec4f(1,1,1,0));

	osg::Vec2 texCoords[] = { osg::Vec2(0, 0), osg::Vec2(1, 0),
			osg::Vec2(1, 1), osg::Vec2(0, 1) };
	nodeQuad->setTexCoordArray(0, new osg::Vec2Array(4, texCoords));

	osg::ref_ptr<osg::StateSet> stateSet = createStateSet();
	if (texture != NULL)
		stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
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

osg::ref_ptr<osg::Geode> OsgNode::createLabel(QString text) {
	if (text.isEmpty()) text = "slon";

	osg::ref_ptr<osgText::FadeText> textDrawable = new osgText::FadeText();
	textDrawable->setFadeSpeed(0.04);

	QString fontPath = Util::Config::getInstance()->getValue(
			"Viewer.Labels.Font");
	float size = Util::Config::getInstance()->getValue(
			"Viewer.Labels.Size").toFloat();

	if (fontPath != NULL && !fontPath.isEmpty())
		textDrawable->setFont(fontPath.toStdString());

	textDrawable->setText(text.toStdString());
	textDrawable->setLineSpacing(0);
	textDrawable->setAxisAlignment(osgText::Text::SCREEN);
	textDrawable->setCharacterSize(size);
	textDrawable->setDrawMode(osgText::Text::TEXT);
	textDrawable->setAlignment(osgText::Text::CENTER_BOTTOM_BASE_LINE);
	textDrawable->setPosition(osg::Vec3(0, size, 0)); // TODO label positioning
	textDrawable->setColor(DataMapping::getDefaultColor(DataMapping::NODE));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(textDrawable);
	return geode;
}

osg::ref_ptr<osg::StateSet> OsgNode::createStateSet() {
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();

	stateSet->setDataVariance(osg::Object::STATIC);
	//	stateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	return stateSet;
}

void OsgNode::resize(float factor) {
	if (factor == 1)
		return;
	osg::Vec3f oldSize = getSize();
	osg::Vec3d newScale = visualContent->getScale() * factor;
	if (newScale.x() > maxScale)
		newScale.set(maxScale, maxScale, maxScale);
	visualContent->setScale(newScale);
	updateFrame(frameG, visualContent->getBoundingBox(), visualContent->getScale().x(), 0.2f); // XXX magic num
	if (oldSize != getSize())
		emit changedSize(oldSize, getSize());
}

void OsgNode::setSize(osg::BoundingBox box) {
	setSize(box.xMax() - box.xMin(), box.yMax() - box.yMin(), box.zMax() - box.zMin());
}

void OsgNode::setSize(float width, float height, float depth) {
	size = osg::Vec3f(width, height, depth);
}

osg::Vec3f OsgNode::getSize() const {
	return size * (expanded ? visualContent->getScale().x() : 1);
}

float OsgNode::getRadius() const {
	return getSize().length()/2.0f;
}

void OsgNode::getProjRect(float &xMin, float &yMin, float &xMax, float &yMax) {
	osg::Vec3f pos = Util::CameraHelper::byView(getPosition());
	osg::Vec3f size = getSize() / 2.0f;
	size.z() = 0;

	osg::Vec3f maxPos = Util::CameraHelper::byWindow(
			Util::CameraHelper::byProjection(pos + size));
	xMax = maxPos.x();
	yMax = maxPos.y();

	osg::Vec3f minPos = Util::CameraHelper::byWindow(
			Util::CameraHelper::byProjection(pos - size));
	xMin = minPos.x();
	yMin = minPos.y();
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
	} else {
		qWarning() << "Cannot set color for geometry in " << QString::fromStdString(geode->getName());
	}
}

void OsgNode::showLabel(bool visible) {
//	qDebug() << visible ? MASK_ON : MASK_OFF;
	labelG->setNodeMask(visible ? MASK_ON : MASK_OFF);
}

void OsgNode::setExpanded(bool flag) {
	if (flag == isExpanded())
		return;

	expanded = flag;
	if (expanded) {
		if (visualContent->load()) {
//			qDebug() << node->getId() << ": content loaded";
			updateFrame(frameG, visualContent->getBoundingBox(), visualContent->getScale().x(), 0.2f);
		}
		setSize(visualContent->getBoundingBox());
	} else {
		setSize(closedG->getBoundingBox());
	}

	frameG->setNodeMask(expanded ? MASK_ON : MASK_OFF);
	contentSwitch->setChildValue(visualContent, expanded);
	contentSwitch->setChildValue(closedG, !expanded);
}

bool OsgNode::isExpanded() const {
	return expanded;
}

void OsgNode::setSelected(bool flag) {
	if (flag == selected)
		return;

	selected = flag;
	if (selected) {
		setDrawableColor(frameG, 0, Util::Config::getColorF("Viewer.Selected.Color"));
		setDrawableColor(closedG, 0, Util::Config::getColorF("Viewer.Selected.Color"));
	} else {
		setDrawableColor(frameG, 0, color);
		setDrawableColor(closedG, 0, color);
	}
}

bool OsgNode::isSelected() const {
	return selected;
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

void OsgNode::setFixed(bool flag) {
	if (flag == isFixed())
		return;

	node->setFixed(flag);
	if (flag) {
		node->setPosition(getPosition());
	}
	fixedG->setNodeMask(flag ? MASK_ON : MASK_OFF);
}

void OsgNode::reloadConfig() {
	// TODO if needed at all
}

bool OsgNode::isPickable(osg::Geode* geode) const {
	if (!pickable)
		return false;
	if (geode->getName() == closedG->getName() ||
			geode->getName() == visualContent->getGeodeName())
		return true;
	else
		return false;
}

osg::Vec3f OsgNode::getPosition() const {
	return osg::AutoTransform::getPosition();
}

void OsgNode::updatePosition(float interpolationSpeed) {
	osg::Vec3f currentPos = getPosition();
	osg::Vec3f targetPos = node->getPosition();

	float eps = 1;
	if ((currentPos - targetPos).length() < eps)
		return;

	if (!usingInterpolation || interpolationSpeed == 1) {
		osg::AutoTransform::setPosition(targetPos);
	} else {
		osg::Vec3 directionVector = osg::Vec3(targetPos - currentPos);
		directionVector *= interpolationSpeed;
		osg::AutoTransform::setPosition(currentPos + directionVector);
	}

	emit changedPosition(currentPos, getPosition());
}

void OsgNode::setPosition(osg::Vec3f pos) {
	node->setPosition(pos);
	updatePosition();
}

QSet<AbstractNode*> OsgNode::getIncidentNodes() {
	QList<Model::Node*>nodes = node->getIncidentNodes();
	QSet<AbstractNode*> nghbrs;
	QList<Model::Node*>::const_iterator i = nodes.begin();
	while (i != nodes.end()) {
		OsgNode* n;
		if ((n = (*i)->getOsgNode()) != NULL)
			nghbrs.insert(n);
		++i;
	}
	return nghbrs;
}

bool OsgNode::isOnScreen() const {
	osg::Vec3f pos = node->getPosition();
	pos = Util::CameraHelper::byProjection(Util::CameraHelper::byView(pos));

	if (qAbs(pos.x()) > 1 || qAbs(pos.y()) > 1) {
		return false;
	}
	return true;
}

bool OsgNode::mayOverlap(OsgNode* u, OsgNode* v) {
	if (u == NULL || v == NULL)
		return false;
	if (!u->isExpanded() && !v->isExpanded())
		return false;
	float udist = (u->getPosition() - Util::CameraHelper::getEye()).length();
	float vdist = (v->getPosition() - Util::CameraHelper::getEye()).length();
	//	qDebug() << u->getName().c_str() << ": " << udist;
	//	qDebug() << v->getName().c_str() << ": " << vdist;
	if (u->isExpanded() && udist >= vdist)
		if (u->isOnScreen())
			return true;
	if (v->isExpanded() && vdist >= udist)
		if (v->isOnScreen())
			return true;
	return false;
}

float OsgNode::getDistanceToEdge(double angle) const {
	float w, h, d;

	w = getSize().x();
	h = getSize().y();

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
	QTextStream(&str) << "N" << node->getId() << " " << "["
			<< pos.x() << "," << pos.y() << "," << pos.z() << "]"
			<< (isFixed() ? "fixed" : "");
	return str;
}

bool OsgNode::equals(OsgNode* other) const {
	if (this == other) {
		return true;
	}
	if (other == NULL) {
		return false;
	}
	if (!node->equals(other->node)) {
		return false;
	}
	return true;
}

void OsgNode::acceptVisitor(AbstractVisitor* visitor) {
	visitor->visitNode(this);
}

void OsgNode::setVisible(bool flag) {
	visible = flag;
	setNodeMask(visible ? MASK_ON : MASK_OFF);
}

bool OsgNode::isVisible() const {
	return visible;
}
