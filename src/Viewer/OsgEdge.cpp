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
#include <math.h>

using namespace Vwr;

float OsgEdge::EDGE_VOLUME = -1;

OsgEdge::OsgEdge(Model::Edge* modelNode, DataMapping* dataMapping) {
	if (modelNode == NULL) qWarning() << "NULL reference to Edge in OsgEdge!";
	this->edge = modelNode;
	this->mapping = (dataMapping != NULL) ? dataMapping :
		new DataMapping(edge->getType()->getKeys());
	visible = true;

	edgeCoords = new osg::Vec3Array(4);
	edgeTexCoords = new osg::Vec2Array(4);
//	endPointCoords = new osg::Vec3Array(8);
	selected = false;
	if (Util::Config::getValue("Viewer.Edge.ShowDirected").toFloat() != 0)
		directed = getMappingValue(DataMapping::DIRECTION) == "true";
	else
		directed = false;

	label = createLabel(getMappingValue(DataMapping::LABEL));

	setColor(mapping->getColor(getMappingValue(DataMapping::COLOR)));
	selectedColor = Util::Config::getColorF("Viewer.Selected.Color");

	if (EDGE_VOLUME < 0)
		EDGE_VOLUME = Util::Config::getValue("Viewer.Edge.Volume").toFloat() *
		DataMapping::getScale(DataMapping::EDGE);
}

OsgEdge::~OsgEdge() {
	edge = NULL;
}

void OsgEdge::setDataMapping(DataMapping* dataMapping) {
	this->mapping = (dataMapping != NULL) ? dataMapping :
		new DataMapping(edge->getType()->getKeys());
	// change label
	label->setText(getMappingValue(DataMapping::LABEL).toStdString());
	// change color
	setColor(mapping->getColor(getMappingValue(DataMapping::COLOR)));
}

QString OsgEdge::getMappingValue(DataMapping::ValueType prop) {
	return edge->data(mapping->getMapping(prop));
}

void OsgEdge::updateGeometry() {
	OsgNode* srcNode = edge->getSrcNode()->getOsgNode();
	OsgNode* dstNode = edge->getDstNode()->getOsgNode();
	if (srcNode == NULL || dstNode == NULL) // nodes are not visualized yet
		return;

	osg::Vec3f x, y;
	x.set(srcNode->getPosition());
	y.set(dstNode->getPosition());

	osg::Vec3f edgeDir = x - y;
	osg::Vec3f viewVec = Util::CameraHelper::getEye() - (x + y) / 2;
	float width = sqrt(EDGE_VOLUME / qMax(0.1f, edgeDir.length()));

	osg::Vec3f up = edgeDir ^ viewVec;
	up.normalize();
	up *= width;

	(*edgeCoords)[0].set(x + up);
	(*edgeCoords)[1].set(x - up);
	(*edgeCoords)[2].set(y - up);
	(*edgeCoords)[3].set(y + up);

	int repeatCnt = 1;
	if (directed)
		repeatCnt = edgeDir.length() / (2 * width); // TODO width == 0

	(*edgeTexCoords)[0].set(0.0f, 1.0f);
	(*edgeTexCoords)[1].set(0.0f, 0.0f);
	(*edgeTexCoords)[2].set(repeatCnt, 0.0f);
	(*edgeTexCoords)[3].set(repeatCnt, 1.0f);

	label->setPosition((x + y) / 2);
}

void OsgEdge::getEdgeData(osg::ref_ptr<osg::Vec3Array> coords, osg::ref_ptr<
		osg::Vec2Array> texCoords, osg::ref_ptr<osg::Vec4Array> colors) {

	if (isVisible()) {
		updateGeometry();
		for (int i = 0; i < 4; i++) {
			coords->push_back((*edgeCoords)[i]);
			texCoords->push_back((*edgeTexCoords)[i]);
		}
	} else {
		for (int i = 0; i < 4; i++) {
			coords->push_back(osg::Vec3f(0,0,0));
			texCoords->push_back(osg::Vec2f(0,0));
		}
	}

	if (edge->getSrcNode()->getOsgNode()->isSelected()) {
		colors->push_back(selectedColor);
		colors->push_back(selectedColor);
	} else {
		colors->push_back(edgeColor);
		colors->push_back(edgeColor);
	}
	if (edge->getDstNode()->getOsgNode()->isSelected()) {
		colors->push_back(selectedColor);
		colors->push_back(selectedColor);
	} else {
		colors->push_back(edgeColor);
		colors->push_back(edgeColor);
	}
}

osg::ref_ptr<osgText::FadeText> OsgEdge::createLabel(QString text) {
	osg::ref_ptr<osgText::FadeText> label = new osgText::FadeText;
	label->setFadeSpeed(0.04);

	QString fontPath = Util::Config::getInstance()->getValue(
			"Viewer.Labels.Font");
	float size = Util::Config::getInstance()->getValue(
			"Viewer.Labels.Size").toFloat();
	size *= DataMapping::getScale(DataMapping::EDGE);

	if (fontPath != NULL && !fontPath.isEmpty())
		label->setFont(fontPath.toStdString());

	label->setText(text.toStdString());
	label->setLineSpacing(0);
	label->setAxisAlignment(osgText::Text::SCREEN);
	label->setCharacterSize(size);
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
		case UNDIRECTED:
				stateSet->setTextureAttributeAndModes(0,
						Util::TextureWrapper::getEdgeTexture(),
						osg::StateAttribute::ON);
				break;
		case DIRECTED:
				stateSet->setTextureAttributeAndModes(0,
						Util::TextureWrapper::getDirectedEdgeTexture(),
						osg::StateAttribute::ON);
				break;
		default:
			break;
	}
	return stateSet;
}

bool OsgEdge::isDirected() {
	return directed;
}

QString OsgEdge::toString() const {
	QString str;
	QTextStream(&str) << "OggEdge " << edge->toString();
	return str;
}
