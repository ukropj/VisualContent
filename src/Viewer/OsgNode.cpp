/*
 * OsgNode.cpp
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgNode.h"
#include "Viewer/OsgContent.h"
#include "Viewer/OsgCluster.h"
#include "Viewer/OsgNodeGroup.h"
#include "Viewer/AbstractVisitor.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"
#include "Util/CameraHelper.h"
#include "Model/Type.h"
#include "Model/Node.h"
#include "Model/Cluster.h"

#include <math.h>
#include <osgText/FadeText>
#include <osgText/Font>
#include <osg/LineWidth>
#include <qDebug>

using namespace Vwr;

float OsgNode::NODE_SIZE = 8;
float OsgNode::FRAME_WIDTH = 1;
osg::ref_ptr<osg::Geode> OsgNode::closedFrame = NULL;

OsgNode::OsgNode(Model::Node* modelNode, DataMapping* dataMapping) {
	if (modelNode == NULL) qWarning() << "NULL reference to Node in OsgNode!";
	this->node = modelNode;
	node->setOsgNode(this);
	setName("node" + node->getId());
	setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	this->mapping = (dataMapping != NULL) ? dataMapping : new DataMapping();
	selected = false;
	expanded = false;
	usingInterpolation = true;
	pickable = true;
	visible = true;
	parent = NULL;
	movingToCluster = false;
	maxScale = Util::Config::getValue("Viewer.Node.MaxScale").toFloat();
	size = osg::Vec3f(0, 0, 0);

	closedG = createTextureNode(node->isCluster() ? Util::TextureWrapper::getClusterTexture() :
			Util::TextureWrapper::getNodeTexture(),
			NODE_SIZE, NODE_SIZE);
	visualG = ContentFactory::createContent(mapping->getContentType(), getMappingValue(DataMapping::CONTENT));
	visualGBorder = initFrame();

	if (closedFrame == NULL) {
		closedFrame = createTextureNode(Util::TextureWrapper::getFrameTexture(),
				NODE_SIZE * 2, NODE_SIZE * 2);
		setDrawableColor(closedFrame, 0, Util::Config::getColorF("Viewer.Selected.Color"));
	}
	visualFrame = initFrame();
	setDrawableColor(visualFrame, 0, Util::Config::getColorF("Viewer.Selected.Color"));

	labelG = createLabel(getMappingValue(DataMapping::LABEL));
	fixedG = createFixed(NODE_SIZE);

	closedG->setName("closed_node");
	visualG->setName("visual_content");
	visualGBorder->setName("visual_content_border");
	closedG->setName("closed_frame");
	visualFrame->setName("visual_frame");
	labelG->setName("label");

	labelG->setNodeMask(false);
	fixedG->setNodeMask(false);

	contentSwitch = new osg::Switch();
	contentSwitch->setName("content");
	contentSwitch->addChild(closedG);
	contentSwitch->addChild(visualG);
	contentSwitch->addChild(visualGBorder);
	contentSwitch->addChild(closedFrame);
	contentSwitch->addChild(visualFrame);
	contentSwitch->setChildValue(closedG, true);
	contentSwitch->setChildValue(visualG, false);
	contentSwitch->setChildValue(visualGBorder, false);
	contentSwitch->setChildValue(closedFrame, false);
	contentSwitch->setChildValue(visualFrame, false);

	addChild(contentSwitch);
	addChild(labelG);
	addChild(fixedG);

	setSize(closedG->getBoundingBox());
	setColor(mapping->getColor(getMappingValue(DataMapping::COLOR)));

	setVisible(!node->isIgnored());
}

OsgNode::~OsgNode() {
	node->setOsgNode(NULL);
//	qDebug() << "OsgNode " << node->getId() << " deleted";
}

void OsgNode::setDataMapping(DataMapping* dataMapping) {
	this->mapping = (dataMapping != NULL) ? dataMapping : new DataMapping();
	// change label
	osgText::FadeText* ft = dynamic_cast<osgText::FadeText*>(labelG->getDrawable(0));
	ft->setText(getMappingValue(DataMapping::LABEL).toStdString());
	// change content
	bool f = isExpanded();
	setExpanded(false);
	contentSwitch->removeChild(visualG);
	visualG = ContentFactory::createContent(mapping->getContentType(), getMappingValue(DataMapping::CONTENT));
	contentSwitch->addChild(visualG);
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
void OsgNode::updateFrame(osg::ref_ptr<osg::Geode> frame, osg::BoundingBox box,
		float scale, float width, float offset) {
	osg::Geometry* geometry = dynamic_cast<osg::Geometry *> (frame->getDrawable(0));

	if (geometry != NULL) {
		float s = DataMapping::getScale(DataMapping::NODE);
		width *= s;
		offset *= s;
		osg::Vec3f corners[4];
		for (int i = 0; i < 4; i++) {
			corners[i] = box.corner(i) * scale;
		}

		osg::Vec3f mx(width + offset, 0, 0);
		osg::Vec3f my(0, width + offset, 0);
		osg::Vec3f offx(offset, 0, 0);
		osg::Vec3f offy(0, offset, 0);
		osg::Vec3 coords[] = {
				corners[0] - offx - offy, corners[0] - mx - my,
				corners[1] + offx - offy, corners[1] + mx - my,
				corners[3] + offx + offy, corners[3] + mx + my,
				corners[2] - offx + offy, corners[2] - mx + my,
				corners[0] - offx - offy, corners[0] - mx - my, };

		geometry->setVertexArray(new osg::Vec3Array(10, coords));
	}
}

osg::ref_ptr<osg::Geode> OsgNode::createTextureNode(
		osg::ref_ptr<osg::Texture2D> texture, float width, float height) {
	float scale = DataMapping::getScale(DataMapping::NODE);
	width *= scale / 2.0f;
	height *= scale / 2.0f;
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

osg::ref_ptr<osg::Geode> OsgNode::createFixed(float size) {
	size *= DataMapping::getScale(DataMapping::NODE);
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
	if (!expanded) // only visual part is affected by resize()
		return;
	osg::Vec3f oldSize = getSize();
	osg::Vec3d newScale = visualG->getScale() * factor;
	if (newScale.x() > maxScale)
		newScale.set(maxScale, maxScale, maxScale);
	visualG->setScale(newScale);
	updateFrame(visualFrame, visualG->getBoundingBox(),
			visualG->getScale().x(), FRAME_WIDTH, FRAME_WIDTH*1.5f);
	updateFrame(visualGBorder, visualG->getBoundingBox(),
			visualG->getScale().x(), FRAME_WIDTH);
	if (oldSize != getSize())
		emit changedSize(oldSize, getSize());
}

void OsgNode::setScale(float scale) {
	osg::Vec3f oldSize = getSize();
	osg::AutoTransform::setScale(scale);
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
	// size * scale of node (dependent on Node::weight) * scale of content (modifiable by user)
	return size * getScale().x() * (expanded ? visualG->getScale().x() : 1);
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
	setDrawableColor(closedG, 0, color);
	setDrawableColor(visualGBorder, 0, color);
}

void OsgNode::setDrawableColor(osg::ref_ptr<osg::Geode> geode, int drawablePos,
		osg::Vec4 color) {
	osg::Geometry* geometry =
			dynamic_cast<osg::Geometry*> (geode->getDrawable(drawablePos));

	if (geometry != NULL) {
		osg::Vec4Array* colorArray =
				dynamic_cast<osg::Vec4Array*> (geometry->getColorArray());
		colorArray->pop_back();
		colorArray->push_back(color);
	} else {
		qWarning() << "Cannot set color for geometry in " << QString::fromStdString(geode->getName());
	}
}

void OsgNode::showLabel(bool visible) {
	labelG->setNodeMask(visible);
}

void OsgNode::setExpanded(bool flag) {
	if (flag == isExpanded())
		return;

	expanded = flag;
	if (expanded) {
//		if(visualG->load())
		visualG->load(); // calling load() by CompositeContent would prevent frame initialization
		{
			updateFrame(visualGBorder, visualG->getBoundingBox(),
					visualG->getScale().x(), FRAME_WIDTH);
			updateFrame(visualFrame, visualG->getBoundingBox(),
					visualG->getScale().x(), FRAME_WIDTH, FRAME_WIDTH);
		}
		setSize(visualG->getBoundingBox());
	} else {
		setSize(closedG->getBoundingBox());
	}

	contentSwitch->setChildValue(visualG, expanded);
	contentSwitch->setChildValue(visualGBorder, expanded);
	contentSwitch->setChildValue(closedG, !expanded);
	if (isSelected()) {
		contentSwitch->setChildValue(visualFrame, expanded);
		contentSwitch->setChildValue(closedFrame, !expanded);
	}
}

bool OsgNode::isExpanded() const {
	return expanded;
}

void OsgNode::setSelected(bool flag) {
	if (flag == selected)
		return;

	selected = flag;
	if (isExpanded()) {
		contentSwitch->setChildValue(visualFrame, selected);
	} else {
		contentSwitch->setChildValue(closedFrame, selected);
	}
	if (parent != NULL)
		parent->allowAutocluster(!selected);

	qDebug() << size.x();
	qDebug() << getScale().x();
	qDebug() << visualG->getScale().x();
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
	fixedG->setNodeMask(flag);
}

void OsgNode::reloadConfig() {
	// TODO if needed at all
}

bool OsgNode::isPickable(osg::Geode* geode) const {
	if (!pickable)
		return false;
	if (geode->getName() == closedG->getName() ||
			geode->getName() == visualG->getGeodeName() ||
			geode->getName() == visualGBorder->getName())
		return true;
	else
		return false;
}

osg::Vec3f OsgNode::getPosition() const {
	if (!isVisible() && parent != NULL) {
		return parent->getPosition();
	}
	return osg::AutoTransform::getPosition();
}

void OsgNode::updatePosition(float interpolationSpeed) {
	osg::Vec3f currentPos = getPosition();
	osg::Vec3f targetPos;
	if (!isMovingToCluster()) {
		targetPos = node->getPosition();
	} else {
		targetPos = parent->getNode()->getPosition();
	}

	// TODO if moving to cluster, let parent set my position

	float eps = 1;
	if ((currentPos - targetPos).length() < eps) { // don't interpolate if distance is small
		if (isMovingToCluster()) {	// finish clustering
			setMovingToCluster(false);
			parent->moveChildIn();
			setVisible(false);
			emit changedVisibility(this, false);
		}
		return;
	}
	if (!usingInterpolation || interpolationSpeed == 1) {
		osg::AutoTransform::setPosition(targetPos);
	} else {
		osg::Vec3 directionVector = osg::Vec3(targetPos - currentPos);
		float step = 4;

		if (isMovingToCluster() && directionVector.length() * interpolationSpeed < step) {
			if (directionVector.length() > step) {
				directionVector.normalize();
				directionVector *= step;
			}
		} else {
			directionVector *= interpolationSpeed;
		}

		osg::AutoTransform::setPosition(currentPos + directionVector);
	}

	emit changedPosition(currentPos, getPosition());
}

void OsgNode::setPosition(osg::Vec3f pos) {
	node->setPosition(pos);
	updatePosition();
}

QSet<AbstractNode*> OsgNode::getIncidentNodes() {
	QSet<Model::Node*>nodes = node->getIncidentNodes();
	QSet<AbstractNode*> nghbrs;
	QSet<Model::Node*>::const_iterator i = nodes.begin();
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
	if (!u->isVisible() || !v->isVisible())
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
	return node->toString();
}

QString OsgNode::debugInfo() const {
	return toString();
}

bool OsgNode::equals(const AbstractNode* other) const {
	if (this == other) {
		return true;
	}
	if (other == NULL) {
		return false;
	}
	const OsgNode* otherNode = dynamic_cast<const OsgNode*>(other);
	if (otherNode == NULL)
		return false;
	if (!node->equals(otherNode->node)) {
		return false;
	}
	return true;
}

void OsgNode::acceptVisitor(AbstractVisitor* visitor) {
	visitor->visitNode(this);
}

void OsgNode::toggleContent(bool flag) {
	if (!isExpanded())
		return;
	contentSwitch->setChildValue(visualG, flag);
}

void OsgNode::setVisible(bool flag) {
	if (visible != flag) {
		visible = flag;
		setNodeMask(visible);
		if (!visible) {
			setFixed(false);
			setExpanded(false);
			setPickable(false);
		} else {
			setPickable(true);
		}
		if (node->isIgnored() == visible) {
			qWarning() << "inconsistnt view! " << toString() << "visible: " << visible;
		}
	}
}

bool OsgNode::isVisible() const {
	return visible;
}

bool OsgNode::updateClusterState(float maxClusterSize) {
	if (isSelected())
		return true; // selected is immune
	if (maxClusterSize < 0)
		return true; // no autoclustering
	if (isClustering())
		return true;
	if (parent != NULL && parent->canAutocluster() &&
			parent->getNode()->getWeight() <= maxClusterSize) {
		parent->cluster();
		return true;
	}
	return false;
}

bool OsgNode::isClusterable() const {
	if (!node->canBeClustered())
		return false;
	if (isClustering())
		return false;
	return true;
}

bool OsgNode::isClustering() const {
	if (isMovingToCluster())
		return true;
	if (parent != NULL && parent->isClustering())
		return true;
	return false;
}

AbstractNode* OsgNode::clusterToParent() {
	if (parent != NULL) {
		return parent->cluster();
	} else {
		return NULL;
	}
}

AbstractNode* OsgNode::uncluster(bool returnResult) {
	return this;
}

void OsgNode::resolveParent() {
	if (node->getParent() != NULL) {
		parent = dynamic_cast<OsgCluster*>(node->getParent()->getOsgNode());
	}
}
