/*
 * OsgFrame.cpp
 *
 *  Created on: 29.11.2010
 *      Author: jakub
 */

//using namespace Vwr;

/*// TODO: subclass for each content type
OsgContent::OsgContent(QString imagePath) {
//	this->imagePath = imagePath;
//	texture = NULL;
//	addDrawable(createGeometry(20, 20));
}

OsgContent::~OsgContent() {
}

osg::ref_ptr<osg::Geometry> OsgContent::createGeometry(float width, float height) {
//	osg::ref_ptr<osg::StateSet> bbState = createStateSet();
	osg::ref_ptr<osg::Geometry> g = new osg::Geometry;

	// coordinates
	width /= 2.0f;
	height /= 2.0f;
	osg::Vec3 coords[] = {
			osg::Vec3(-width, -height, 0),
			osg::Vec3(width, -height, 0),
			osg::Vec3(width, height, 0),
			osg::Vec3(-width, height, 0)};
	g->setUseDisplayList(false);
	g->setVertexArray(new osg::Vec3Array(4, coords));
	g->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	// colors
	osg::Vec4Array* colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

//	ColorIndexArray* colorIndexArray =
//			new osg::TemplateIndexArray<unsigned int,
//					osg::Array::UIntArrayType, 4, 1>;
//	colorIndexArray->push_back(0);
	g->setColorArray(colorArray);
//	g->setColorIndices(colorIndexArray);
	g->setColorBinding(osg::Geometry::BIND_OVERALL);

	return g;
}

void OsgContent::load() {
//	if (texture == NULL) {
//		texture = Util::TextureWrapper::readTextureFromFile(imagePath);
//
//		osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
//		stateSet->setTextureAttributeAndModes(0, texture,
//				osg::StateAttribute::ON);
//
//		osg::Vec2 texCoords[] = { osg::Vec2(0, 0), osg::Vec2(1, 0),
//				osg::Vec2(1, 1), osg::Vec2(0, 1) };
//		osg::Geometry* g = dynamic_cast<osg::Geometry *>(getDrawable(0));
//		g->setTexCoordArray(0, new osg::Vec2Array(4, texCoords));
//		g->setStateSet(stateSet);
//	}
}*/
