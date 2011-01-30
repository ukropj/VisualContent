/*
 * OsgOsgEdge.cpp
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgEdge.h"
#include "Viewer/OsgNode.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"
#include "Util/CameraHelper.h"
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Model/Type.h"

using namespace Vwr;

OsgEdge::OsgEdge(Model::Edge* edge) {

	this->edge = edge;

	edgeCoords = new osg::Vec3Array(4);
	edgeTexCoords = new osg::Vec2Array(4);
//	endPointCoords = new osg::Vec3Array(8);
	selected = false;

	label = createLabel(edge->getName());

	setColor(edge->getType()->getColor());
}

OsgEdge::~OsgEdge() {
	edge = NULL;
}

void OsgEdge::updateGeometry() {
	OsgNode* srcNode = edge->getSrcNode()->getOsgNode();
	OsgNode* dstNode = edge->getDstNode()->getOsgNode();
	if (srcNode == NULL || dstNode == NULL) // nodes are not visualized yet
		return;

	osg::Vec3f x, y;
	x.set(srcNode->getPosition());
	y.set(dstNode->getPosition());

	float scale = Util::Config::getValue("Viewer.Textures.EdgeScale").toFloat();

	osg::Vec3f edgeDir = x - y;
	osg::Vec3f viewVec = Util::CameraHelper::getEye() - (x + y) / 2;

	osg::Vec3f up = edgeDir ^ viewVec;
	up.normalize();
	up *= scale;

	(*edgeCoords)[0].set(x + up);
	(*edgeCoords)[1].set(x - up);
	(*edgeCoords)[2].set(y - up);
	(*edgeCoords)[3].set(y + up);

	int repeatCnt = edgeDir.length() / (2 * scale);
	if (!edge->isOriented())
			repeatCnt = 1;

	(*edgeTexCoords)[0].set(0.0f, 1.0f);
	(*edgeTexCoords)[1].set(0.0f, 0.0f);
	(*edgeTexCoords)[2].set(repeatCnt, 0.0f);
	(*edgeTexCoords)[3].set(repeatCnt, 1.0f);

	label->setPosition((x + y) / 2);

	return;
}

void OsgEdge::getEdgeData(osg::ref_ptr<osg::Vec3Array> coords, osg::ref_ptr<
		osg::Vec2Array> texCoords, osg::ref_ptr<osg::Vec4Array> colors) {

	updateGeometry();

	for (int i = 0; i < 4; i++) {
		coords->push_back((*edgeCoords)[i]);
		texCoords->push_back((*edgeTexCoords)[i]);
	}

	osg::Vec4f defaultColor = getEdgeColor();
	osg::Vec4f selectedColor = Util::Config::getColorF("Viewer.Selected.Color");

	if (edge->getSrcNode()->getOsgNode()->isSelected()) {
		colors->push_back(selectedColor);
		colors->push_back(selectedColor);
	} else {
		colors->push_back(defaultColor);
		colors->push_back(defaultColor);
	}
	if (edge->getDstNode()->getOsgNode()->isSelected()) {
		colors->push_back(selectedColor);
		colors->push_back(selectedColor);
	} else {
		colors->push_back(defaultColor);
		colors->push_back(defaultColor);
	}
}

osg::ref_ptr<osgText::FadeText> OsgEdge::createLabel(QString text) {
	osg::ref_ptr<osgText::FadeText> label = new osgText::FadeText;
	label->setFadeSpeed(0.04);

	QString fontPath = Util::Config::getInstance()->getValue(
			"Viewer.Labels.Font");

	// experimental value
	float scale = 1.375f * edge->getType()->getScale();

	if (fontPath != NULL && !fontPath.isEmpty())
		label->setFont(fontPath.toStdString());

	label->setText(text.toStdString());
	label->setLineSpacing(0);
	label->setAxisAlignment(osgText::Text::SCREEN);
	label->setCharacterSize(scale);
	label->setDrawMode(osgText::Text::TEXT);
	label->setAlignment(osgText::Text::CENTER_BOTTOM_BASE_LINE);
	label->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	return label;
}

osg::ref_ptr<osg::StateSet> OsgEdge::createStateSet(StateSetType type) {
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
	stateSet->setAttributeAndModes(new osg::BlendFunc,
			osg::StateAttribute::ON);
	switch (type) {
		case UNORIENTED:
				stateSet->setTextureAttributeAndModes(0,
						Util::TextureWrapper::getEdgeTexture(),
						osg::StateAttribute::ON);
				break;
		case ORIENTED:
				stateSet->setTextureAttributeAndModes(0,
						Util::TextureWrapper::getOrientedEdgeTexture(),
						osg::StateAttribute::ON);
				break;
		case ENDPOINT:
		default:
			break;
	}
	return stateSet;
}

void OsgEdge::showLabel(bool visible) { // FIXME not working
//	if (this->containsDrawable(label) && !visible)
//		removeDrawable(label);
//	if (!this->containsDrawable(label) && visible)
//		addDrawable(label);
}

bool OsgEdge::isOriented() {
	return edge->isOriented();
}

QString OsgEdge::toString() const {
	QString str;
	QTextStream(&str) << "edge id:" << edge->getId() << " name:"
			<< edge->getName();
	return str;
}
