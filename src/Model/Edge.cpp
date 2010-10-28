/*!
 * Edge.cpp
 * Projekt 3DVisual
 */
#include "Model/Edge.h"
#include "Model/Graph.h"
#include "Model/Type.h"
#include "Util/Config.h"

using namespace Model;

Edge::Edge(qlonglong id, QString name, Graph* graph,
		osg::ref_ptr<Node> srcNode, osg::ref_ptr<Node> dstNode,
		Model::Type* type, bool isOriented, int pos) :
	osg::DrawArrays(osg::PrimitiveSet::QUADS, pos, 4) {

	this->id = id;
	this->name = name;
	this->graph = graph;
	this->srcNode = srcNode;
	this->dstNode = dstNode;
	this->type = type;
	this->oriented = isOriented;
	this->selected = false;

	float r = type->getValue("color.R").toFloat();
	float g = type->getValue("color.G").toFloat();
	float b = type->getValue("color.B").toFloat();
	float a = type->getValue("color.A").toFloat();

	this->edgeColor = osg::Vec4(r, g, b, a);

	coordinates = new osg::Vec3Array();
	edgeTexCoords = new osg::Vec2Array();

	updateCoordinates(getSrcNode()->getTargetPosition(),
			getDstNode()->getTargetPosition());
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

void Edge::updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstPos, osg::Vec3 viewVec) {
	coordinates->clear();
	edgeTexCoords->clear();

	osg::Vec3d up;

	float graphScale = Util::Config::getValue(
			"Viewer.Display.NodeDistanceScale").toFloat();

	osg::Vec3 x, y;
	x.set(srcPos);
	y.set(dstPos);

	osg::Vec3d edgeDir = x - y;
	length = edgeDir.length();

	up = edgeDir ^ viewVec;
	up.normalize();

	up *= Util::Config::getValue("Viewer.Textures.EdgeScale").toFloat();

	coordinates->push_back(osg::Vec3(x.x() + up.x(), x.y() + up.y(), x.z()
			+ up.z()));
	coordinates->push_back(osg::Vec3(x.x() - up.x(), x.y() - up.y(), x.z()
			- up.z()));
	coordinates->push_back(osg::Vec3(y.x() - up.x(), y.y() - up.y(), y.z()
			- up.z()));
	coordinates->push_back(osg::Vec3(y.x() + up.x(), y.y() + up.y(), y.z()
			+ up.z()));

	/*std::cout << "Edge coord 1: " << x.x() + up.x() << " " << x.y() + up.y() << " " << x.z() + up.z() << "\n";
	 std::cout << "Edge coord 2: " << x.x() - up.x() << " " << x.y() - up.y() << " " << x.z() - up.z() << "\n";
	 std::cout << "Edge coord 3: " << y.x() - up.x() << " " << y.y() - up.y() << " " << y.z() - up.z() << "\n";
	 std::cout << "Edge coord 4: " << y.x() + up.x() << " " << y.y() + up.y() << " " << y.z() + up.z() << "\n";*/

	int repeatCnt = length / (2 * Util::Config::getValue(
			"Viewer.Textures.EdgeScale").toFloat());

	edgeTexCoords->push_back(osg::Vec2(0, 1.0f));
	edgeTexCoords->push_back(osg::Vec2(0, 0.0f));
	edgeTexCoords->push_back(osg::Vec2(repeatCnt, 0.0f));
	edgeTexCoords->push_back(osg::Vec2(repeatCnt, 1.0f));

	if (label != NULL)
		label->setPosition((srcPos + dstPos) / 2);
}

osg::ref_ptr<osg::Drawable> Edge::createLabel(QString name) {
	label = new osgText::FadeText;
	label->setFadeSpeed(0.03);

	QString fontPath = Util::Config::getInstance()->getValue(
			"Viewer.Labels.Font");

	// experimental value
	float scale = 1.375f * this->type->getScale();

	if (fontPath != NULL && !fontPath.isEmpty())
		label->setFont(fontPath.toStdString());

	label->setText(name.toStdString());
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

