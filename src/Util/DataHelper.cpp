#include "Util/DataHelper.h"
#include <QLinkedList>
#include "Model/Node.h"
#include "Model/Edge.h"
#include "Model/Type.h"

using namespace Util;
using namespace Model;

osg::ref_ptr<osg::Geode> DataHelper::getSphereGeode(osg::Vec3 center,
		float radius) {
	osg::ref_ptr<osg::Geode> sphereGeode = new osg::Geode;

	osg::ref_ptr<osg::Sphere> unitSphere = new osg::Sphere(center, radius);
	osg::ref_ptr<osg::ShapeDrawable> unitSphereDrawable =
			new osg::ShapeDrawable(unitSphere);

	sphereGeode->addDrawable(unitSphereDrawable);

	return sphereGeode.get();
}

osg::ref_ptr<osg::Vec3Array> DataHelper::getInitialVectors(int count) {
	int x, y, z;
	int lowest = 1;
	int highest = 100;
	int range = (highest - lowest) + 1;

	osg::ref_ptr<osg::Vec3Array> positions = new osg::Vec3Array();

	srand((unsigned) time(0));

	for (int index = 0; index < count; index++) {
		x = lowest + int(range * rand() / (RAND_MAX + 1.0));
		y = lowest + int(range * rand() / (RAND_MAX + 1.0));
		z = lowest + int(range * rand() / (RAND_MAX + 1.0));

		positions->push_back(osg::Vec3(x, y, z));
	}

	return positions;
}

int DataHelper::getRandomNumber(int lowest, int highest) {
	int range = (highest - lowest) + 1;

	return lowest + int(range * rand() / (RAND_MAX + 1.0));
}

osg::ref_ptr<osg::Vec3Array> DataHelper::getEdgeVectors(
		osg::ref_ptr<Node> inNode, osg::ref_ptr<Node> outNode) {
	osg::ref_ptr<osg::Vec3Array> edgeVectors = new osg::Vec3Array;

	edgeVectors->push_back(osg::Vec3(inNode->getTargetPosition()));
	edgeVectors->push_back(osg::Vec3(inNode->getTargetPosition().x() - 0.5,
			inNode->getTargetPosition().y(), inNode->getTargetPosition().z()));
	edgeVectors->push_back(osg::Vec3(outNode->getTargetPosition().x() - 0.5,
			outNode->getTargetPosition().y(), outNode->getTargetPosition().z()));
	edgeVectors->push_back(osg::Vec3(outNode->getTargetPosition()));

	return edgeVectors;
}

osg::Vec3f DataHelper::getMassCenter(osg::ref_ptr<osg::Vec3Array> coordinates) {
	float x, y, z;
	x = y = z = 0;

	int num = coordinates->size();

	for (int i = 0; i < num; i++) {
		x += coordinates->at(i).x();
		y += coordinates->at(i).y();
		z += coordinates->at(i).z();
	}

	return osg::Vec3f(x / num, y / num, z / num);
}
