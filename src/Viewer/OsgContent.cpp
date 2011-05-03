/*
 * ControlFrame.cpp
 *
 *  Created on: 29.11.2010
 *      Author: jakub
 */

#include "Viewer/OsgContent.h"
#include "Viewer/ObjectContent.h"
#include "Viewer/ImageContent.h"
#include "Model/Node.h"
#include "Model/Type.h"
#include "Model/Graph.h"
#include <QDebug>

using namespace Vwr;

OsgContent::OsgContent() {
	contentGeode = NULL;
}

OsgContent::~OsgContent() {
}

std::string OsgContent::getGeodeName() const {
	if (contentGeode != NULL) {
		return contentGeode->getName();
	} else {
		return "?";
	}
}

const osg::BoundingBox& OsgContent::getBoundingBox() const {
	if (contentGeode != NULL)
		return contentGeode->getBoundingBox();
	else
		return (new osg::Geode)->getBoundingBox();
}

OsgContent* ContentFactory::createContent(DataMapping::ContentType type, int id, QString data) {
	switch (type) {
	case DataMapping::IMAGE :
		return new ImageContent(data);
	case DataMapping::TEXT :
		return new TextContent(data);
		// TODO other content, validation ?
	case DataMapping::OSG :
		return new ObjectContent(data);
	case DataMapping::WEB :
		return new WebContent(data);
	case DataMapping::COMPOSITE :
	case DataMapping::NO_CONTENT :
		return new EmptyContent();
	case DataMapping::GEN_ALPHABET :
		return createAlphabetContent(id);
	case DataMapping::GEN_IMAGES :
		return new ImageContent(
				QString("img/sample data/pic%1.jpg").arg((qrand() % 9)));
	case DataMapping::GEN_RANDOM :
	default :
		return createRandomContent(id);
	}
}

OsgContent* ContentFactory::createRandomContent(int id) {
	switch (id % 8) {
	case 0 :
		return new ImageContent("img/sample data/devil.jpg");
	case 1 :
		return new ImageContent("img/sample data/house.jpg");
	case 2 :
		return new ImageContent("img/sample data/elephant.jpg");
	case 3 :
		return new TextContent(
   "Lorem ipsum dolor sit amet, consectetaur adipisicing elit," \
   "sed do eiusmod tempor incididunt ut labore et dolore magna" \
   "aliqua. Ut enim ad minim veniam, quis nostrud exercitation" \
   "ullamco laboris nisi ut aliquip ex ea commodo consequat.  Duis" \
   "aute irure dolor in reprehenderit in voluptate velit esse" \
   "cillum dolore eu fugiat nulla pariatur. Excepteur sint" \
   "occaecat cupidatat non proident, sunt in culpa qui officia" \
   "deserunt mollit anim id est laborum.\n\n" \
   "Sed ut perspiciatis unde omnis iste natus error sit voluptatem" \
   "accusantium doloremque laudantium, totam rem aperiam, eaque" \
   "ipsa quae ab illo inventore veritatis et quasi architecto" \
   "beatae vitae dicta sunt explicabo.  Nemo enim ipsam voluptatem" \
   "quia voluptas sit aspernatur aut odit aut fugit, sed quia" \
   "consequuntur magni dolores eos qui ratione voluptatem sequi" \
   "nesciunt.  Neque porro quisquam est, qui dolorem ipsum quia" \
   "dolor sit amet, consectetur, adipisci velit, sed quia non" \
   "numquam eius modi tempora incidunt ut labore et dolore magnam" \
   "aliquam quaerat voluptatem.  Ut enim ad minima veniam, quis" \
   "nostrum exercitationem ullam corporis suscipit laboriosam," \
   "nisi ut aliquid ex ea commodi consequatur?  Quis autem vel eum" \
   "iure reprehenderit qui in ea voluptate velit esse quam nihil" \
   "molestiae consequatur, vel illum qui dolorem eum fugiat quo" \
   "voluptas nulla pariatur?", 300, 350);
	case 4 :
		return new TextContent(
	"But the raven, sitting lonely on the placid bust, spoke only,\n" \
	"That one word, as if his soul in that one word he did outpour.\n" \
	"Nothing further then he uttered - not a feather then he fluttered -\n" \
	"Till I scarcely more than muttered `Other friends have flown before -\n" \
	"On the morrow he will leave me, as my hopes have flown before.'\n" \
	"Then the bird said, `Nevermore.'", 300, 150);
	case 5 :
		return new ObjectContent("img/sample data/cow.osg");
	case 6 :
		return new WebContent("http://www.fiit.sk");
	case 7 :
		return new WebContent("http://www.google.com");
	}
}

OsgContent* ContentFactory::createAlphabetContent(int id) {
	char ch = (id % 26) + 97;
	QString path = QString("img/sample data/alphabet/%1.png").arg(QChar(ch));
	return new ImageContent(path);
}

EmptyContent::EmptyContent() {
	loaded = false;
}

bool EmptyContent::load() {
	if (loaded) {
		return false;
	} else {
		loaded = true;
		contentGeode = new osg::Geode;
		float width = 10, height = 10;
		osg::Geometry* g = osg::createTexturedQuadGeometry(
				osg::Vec3(-width/2.0f, -height/2.0f, 0),
				osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
		g->setUseDisplayList(false);
		contentGeode->addDrawable(g);
		contentGeode->setName("node_content_empty");
		addChild(contentGeode);
		return true;
	}
}
