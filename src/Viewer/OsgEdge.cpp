/*
 * OsgOsgEdge.cpp
 *
 *  Created on: 12.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgEdge.h"
#include "Viewer/OsgNode.h"
#include "Viewer/SceneGraph.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Model/Type.h"

using namespace Vwr;

osg::ref_ptr<osg::StateSet> OsgEdge::stateSet = NULL;
osg::ref_ptr<osg::StateSet> OsgEdge::stateSetOriented = NULL;
osg::ref_ptr<osg::StateSet> OsgEdge::stateSetEndpoint = NULL;

OsgEdge::OsgEdge(Model::Edge* edge, SceneGraph* sceneGraph) {

	this->edge = edge;
	this->sceneGraph = sceneGraph;

	edgeCoords = new osg::Vec3Array(4);
	edgeTexCoords = new osg::Vec2Array(4);
	endPointCoords = new osg::Vec3Array(8);

	geometry = createGeometry();
	endPoints = createEndpointGeometry();
	label = createLabel(edge->getName());

	addDrawable(geometry);
	addDrawable(endPoints);

	setColor(edge->getType()->getColor());
}

OsgEdge::~OsgEdge() {
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
	osg::Vec3f viewVec = sceneGraph->getEye() - (x + y) / 2;

	osg::Vec3f up = edgeDir ^ viewVec;
	up.normalize();
	up *= scale;

	(*edgeCoords)[0].set(x + up);
	(*edgeCoords)[1].set(x - up);
	(*edgeCoords)[2].set(y - up);
	(*edgeCoords)[3].set(y + up);
	geometry->setVertexArray(edgeCoords);

	int repeatCnt = edgeDir.length() / (2 * scale);
	//	if (!edge->isOriented())
	//		repeatCnt = 1;

	(*edgeTexCoords)[0].set(0.0f, 1.0f);
	(*edgeTexCoords)[1].set(0.0f, 0.0f);
	(*edgeTexCoords)[2].set(repeatCnt, 0.0f);
	(*edgeTexCoords)[3].set(repeatCnt, 1.0f);
	geometry->setTexCoordArray(0, edgeTexCoords);

	label->setPosition((x + y) / 2);

	osg::Vec3f a, b;
	a.set(up);
	b.set(edgeDir);
	b.normalize();
	b *= scale;
	a *= 2;
	b *= 2;

	osg::Vec3f offset = x - y;
//	offset = sceneGraph->byView(offset, true);
//	offset.z() = 0;
//	offset = sceneGraph->byViewInv(offset, true);

//	float origLength =
			offset.normalize();

//	float rx = srcNode->getRadius();// * 0.7f;
//	float ry = dstNode->getRadius();// * 0.7f;

//	if (origLength > rx + ry) {
//		x += -offset * rx;
//		y += offset * ry;
//	} else {
//		y = x; // TODO something usefull
//	}

	if (srcNode->isExpanded()) {
		x += -offset * srcNode->getRadius();;
		(*endPointCoords)[3].set(x - a);
		(*endPointCoords)[2].set(x + b);
		(*endPointCoords)[1].set(x + a);
		(*endPointCoords)[0].set(x + b/2.0);
	} else {
		for (int i = 0; i < 4; i++)
			(*endPointCoords)[i].set(0, 0, 0);
	}
	if (dstNode->isExpanded()) {
		y += offset * dstNode->getRadius();
		(*endPointCoords)[4].set(y + a);
		(*endPointCoords)[5].set(y - b/2.0);
		(*endPointCoords)[6].set(y - a);
		(*endPointCoords)[7].set(y - b);
	} else {
		for (int i = 4; i < 8; i++)
			(*endPointCoords)[i].set(0, 0, 0);
	}
	endPoints->setVertexArray(endPointCoords);
}

osg::ref_ptr<osg::Geometry> OsgEdge::createGeometry() {
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,
			4));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	geometry->setColorArray(colorArray);
	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);

	if (!edge->isOriented())
		geometry->setStateSet(getStateSetInstance(NONORIENTED));
	else
		geometry->setStateSet(getStateSetInstance(ORIENTED));
	return geometry;
}

osg::ref_ptr<osg::Geometry> OsgEdge::createEndpointGeometry() {
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,
			2 * 4));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(0.0f, 0.5f, 0.3f, 1.0f));
	geometry->setColorArray(colorArray);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	geometry->setStateSet(getStateSetInstance(ENDPOINT));
	return geometry;
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

osg::ref_ptr<osg::StateSet> OsgEdge::getStateSetInstance(StateSetType type) {
	switch (type) {
	case NONORIENTED:
		if (stateSet == NULL) {
			stateSet = createStateSet(NONORIENTED);
			stateSet->setTextureAttributeAndModes(0,
					Util::TextureWrapper::getEdgeTexture(),
					osg::StateAttribute::ON);
			stateSet->setAttributeAndModes(new osg::BlendFunc,
					osg::StateAttribute::ON);
		}
		return stateSet;
	case ORIENTED:
		if (stateSetOriented == NULL) {
			stateSetOriented = createStateSet(ORIENTED);
			stateSetOriented->setTextureAttributeAndModes(0,
					Util::TextureWrapper::getOrientedEdgeTexture(),
					osg::StateAttribute::ON);
			stateSetOriented->setMode(GL_BLEND, osg::StateAttribute::ON);
		}
		return stateSetOriented;
	case ENDPOINT:
		if (stateSetEndpoint == NULL) {
			stateSetEndpoint = createStateSet(ENDPOINT);
			//	edgeStateSet->setTextureAttributeAndModes(0, edge->getType()->getTexture(),
			//			osg::StateAttribute::ON);
		}
		return stateSetEndpoint;
	}
}

osg::ref_ptr<osg::StateSet> OsgEdge::createStateSet(StateSetType type) const {
	osg::ref_ptr<osg::StateSet> edgeStateSet = new osg::StateSet();
	edgeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
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

QString OsgEdge::toString() const {
	QString str;
	QTextStream(&str) << "edge id:" << edge->getId() << " name:"
			<< edge->getName();
	return str;
}
