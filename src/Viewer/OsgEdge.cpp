/*
 * OsgOsgEdge.cpp
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgEdge.h"
#include "Viewer/SceneGraph.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"
#include "Model/Node.h"
#include "Model/Type.h"

using namespace Vwr;

osg::ref_ptr<osg::StateSet> OsgEdge::stateSet = NULL;
osg::ref_ptr<osg::StateSet> OsgEdge::stateSetOriented = NULL;

OsgEdge::OsgEdge(Model::Edge* edge, SceneGraph* sceneGraph) {

	this->edge = edge;
	this->sceneGraph = sceneGraph;

	geometry = createGeometry();
	//	updateGeometry();

	label = createLabel(edge->getName());
	addDrawable(geometry);

	float r = edge->getType()->getValue("color.R").toFloat();
	float g = edge->getType()->getValue("color.G").toFloat();
	float b = edge->getType()->getValue("color.B").toFloat();
	float a = edge->getType()->getValue("color.A").toFloat();
	setColor(osg::Vec4(r, g, b, a));
}

OsgEdge::~OsgEdge() {
}

void OsgEdge::updateGeometry() {
	float graphScale = Util::Config::getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	osg::Vec3f x, y;
	x.set(edge->getSrcNode()->getCurrentPosition());
	y.set(edge->getDstNode()->getCurrentPosition());

	float rx = edge->getSrcNode()->getRadius() * 0.5f;
	float ry = edge->getDstNode()->getRadius() * 0.5f;

	osg::Vec3f offset = x - y;
	float origLength = offset.normalize();
	if (origLength > rx + ry) {
		x += -offset * rx;
		y += offset * ry;
	} else {
		y = x; // TODO
	}

	osg::Vec3f edgeDir = x - y;
	float length = edgeDir.length();

	osg::Vec3f eye = sceneGraph->getViewCoords()[0]; // eye position
	osg::Vec3f viewVec = eye - (x + y) / 2;
	viewVec.normalize();

	osg::Vec3f up = edgeDir ^ viewVec;
	up.normalize();

	up *= Util::Config::getValue("Viewer.Textures.EdgeScale").toFloat();

	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	coordinates->push_back(x + up);
	coordinates->push_back(x - up);
	coordinates->push_back(y - up);
	coordinates->push_back(y + up);

	int repeatCnt = length / (2 * Util::Config::getValue(
			"Viewer.Textures.EdgeScale").toFloat());

	osg::ref_ptr<osg::Vec2Array> edgeTexCoords = new osg::Vec2Array;
	edgeTexCoords->push_back(osg::Vec2f(0, 1.0f));
	edgeTexCoords->push_back(osg::Vec2f(0, 0.0f));
	edgeTexCoords->push_back(osg::Vec2f(repeatCnt, 0.0f));
	edgeTexCoords->push_back(osg::Vec2f(repeatCnt, 1.0f));

	if (label != NULL)
		label->setPosition((x + y) / 2);

	geometry->setVertexArray(coordinates);
	geometry->setTexCoordArray(0, edgeTexCoords);
}

osg::ref_ptr<osg::Geometry> OsgEdge::createGeometry() {
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,
			4));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	geometry->setColorArray(colorArray);
	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	return geometry;
}

osg::ref_ptr<osgText::FadeText> OsgEdge::createLabel(QString text) {
	osg::ref_ptr<osgText::FadeText> label = new osgText::FadeText;
	label->setFadeSpeed(0.03);

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
	label->setPosition((edge->getDstNode()->getTargetPosition()
			+ edge->getSrcNode()->getTargetPosition()) / 2);
	label->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	return label;
}

osg::ref_ptr<osg::StateSet> OsgEdge::getStateSetInstance(bool oriented) {
	if (!oriented) {
		if (stateSet == NULL)
			stateSet = createStateSet(false);
		return stateSet;
	} else {
		if (stateSetOriented == NULL)
			stateSetOriented = createStateSet(true);
		return stateSetOriented;
	}
}

osg::ref_ptr<osg::StateSet> OsgEdge::createStateSet(bool oriented) {
	osg::ref_ptr<osg::StateSet> edgeStateSet = new osg::StateSet;

	if (!oriented) {
		edgeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		edgeStateSet->setTextureAttributeAndModes(0,
				Util::TextureWrapper::getEdgeTexture(), osg::StateAttribute::ON);
		edgeStateSet->setAttributeAndModes(new osg::BlendFunc,
				osg::StateAttribute::ON);
	} else {
		edgeStateSet->setTextureAttributeAndModes(0,
				Util::TextureWrapper::getOrientedEdgeTexture(),
				osg::StateAttribute::ON);
		edgeStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	}

	edgeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	edgeStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	edgeStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	edgeStateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);

	return edgeStateSet;
}
