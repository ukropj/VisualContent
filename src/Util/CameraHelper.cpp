#include "Util/CameraHelper.h"

using namespace Util;

osg::Camera* CameraHelper::camera = new osg::Camera();

CameraHelper::CameraHelper() {
}

CameraHelper::~CameraHelper() {
}

void CameraHelper::setCamera(osg::Camera* camera) {
	CameraHelper::camera = camera;
}
