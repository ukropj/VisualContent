/*!
 * Edge.cpp
 * Projekt 3DVisual
 */
#include "Model/Edge.h"
#include "Model/Graph.h"
#include "Model/Type.h"
#include "Util/Config.h"
#include "Util/TextureWrapper.h"

using namespace Model;

Edge::Edge(qlonglong id, QString name, Graph* graph,
		osg::ref_ptr<Model::Node> srcNode, osg::ref_ptr<Model::Node> dstNode,
		Model::Type* type, bool isOriented, int pos) {

	this->id = id;
	this->name = name;
	this->graph = graph;
	this->srcNode = srcNode;
	this->dstNode = dstNode;
	this->type = type;
	this->oriented = isOriented;
	this->selected = false;

	geometry = createGeometry();
	//	updateGeometry();

	label = createLabel(name);
	addDrawable(geometry);

	float r = type->getValue("color.R").toFloat();
	float g = type->getValue("color.G").toFloat();
	float b = type->getValue("color.B").toFloat();
	float a = type->getValue("color.A").toFloat();
	setEdgeColor(osg::Vec4(r, g, b, a));
}

Edge::~Edge(void) {
	this->graph = NULL;
	if (this->srcNode != NULL) {
		this->srcNode->removeEdge(this);
		this->srcNode = NULL;
	}

	if (this->dstNode != NULL) {
		this->dstNode->removeEdge(this);
		this->dstNode = NULL;
	}

	this->type = NULL;
}

void Edge::linkNodes(QMap<qlonglong, osg::ref_ptr<Edge> > *edges) {
	edges->insert(this->id, this);
	this->dstNode->addEdge(this);
	this->srcNode->addEdge(this);
}

void Edge::unlinkNodes() {
	this->dstNode->removeEdge(this);
	this->srcNode->removeEdge(this);
	this->srcNode = NULL;
	this->dstNode = NULL;
}

void Edge::unlinkNodesAndRemoveFromGraph() {
	//unlinkNodes will be called from graph->removeEdge !!
	this->graph->removeEdge(this);
}

void Edge::updateGeometry(osg::Vec3 viewVec) {
	float graphScale = Util::Config::getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	osg::Vec3 x, y;
	x.set(srcNode->getCurrentPosition());
	y.set(dstNode->getCurrentPosition());

	float rx = srcNode->getRadius() * 0.5f;
	float ry = dstNode->getRadius() * 0.5f;

	osg::Vec3d offset = x - y;
	float origLength = offset.normalize();
	if (origLength > rx + ry) {
		x += -offset * rx;
		y += offset * ry;
	} else {
		y = x; // TODO
	}

	osg::Vec3d edgeDir = x - y;
	length = edgeDir.length();

	osg::Vec3d up = edgeDir ^ viewVec;
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
	edgeTexCoords->push_back(osg::Vec2(0, 1.0f));
	edgeTexCoords->push_back(osg::Vec2(0, 0.0f));
	edgeTexCoords->push_back(osg::Vec2(repeatCnt, 0.0f));
	edgeTexCoords->push_back(osg::Vec2(repeatCnt, 1.0f));

	if (label != NULL)
		label->setPosition((x + y) / 2);

	geometry->setVertexArray(coordinates);
	geometry->setTexCoordArray(0, edgeTexCoords);
}

osg::ref_ptr<osg::Geometry> Edge::createGeometry() {
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,
			4));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	geometry->setColorArray(colorArray);
	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	return geometry;
}

osg::ref_ptr<osgText::FadeText> Edge::createLabel(QString text) {
	osg::ref_ptr<osgText::FadeText> label = new osgText::FadeText;
	label->setFadeSpeed(0.03);

	QString fontPath = Util::Config::getInstance()->getValue(
			"Viewer.Labels.Font");

	// experimental value
	float scale = 1.375f * this->type->getScale();

	if (fontPath != NULL && !fontPath.isEmpty())
		label->setFont(fontPath.toStdString());

	label->setText(text.toStdString());
	label->setLineSpacing(0);
	label->setAxisAlignment(osgText::Text::SCREEN);
	label->setCharacterSize(scale);
	label->setDrawMode(osgText::Text::TEXT);
	label->setAlignment(osgText::Text::CENTER_BOTTOM_BASE_LINE);
	label->setPosition((this->dstNode->getTargetPosition()
			+ this->srcNode->getTargetPosition()) / 2);
	label->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	return label;
}

osg::ref_ptr<osg::StateSet> Edge::createStateSet(bool oriented) {
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

osg::ref_ptr<osg::StateSet> Edge::stateSet = NULL;
osg::ref_ptr<osg::StateSet> Edge::stateSetOriented = NULL;

osg::ref_ptr<osg::StateSet> Edge::getStateSetInstance(bool oriented) {
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
