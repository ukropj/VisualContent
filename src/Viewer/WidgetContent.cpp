/*
 * WidgetContent.cpp
 *
 *  Created on: 25.12.2010
 *      Author: jakub
 */

#include "Viewer/WidgetContent.h"
#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osg/Texture>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "OsgQtBrowser/QWidgetImage.h"
#include "Util/TextureWrapper.h"

using namespace Vwr;

WidgetContent::WidgetContent() : OsgContent() {
}

WidgetContent::~WidgetContent() {
}

// TODO refactor
void WidgetContent::setWidget(QWidget* widget, float scale) {
	osg::ref_ptr<QWidgetImage> widgetImage = new QWidgetImage(widget);
	widgetImage->getQWidget()->setAttribute(Qt::WA_TranslucentBackground);
	widgetImage->getQGraphicsViewAdapter()->setBackgroundColor(QColor(0, 0, 0, 0));

	QSize size = widget->geometry().size();
	float width = size.width()*150 / ((float)size.height());
	float height = 150;

	width *= scale;
	height *= scale;

	// determines size & ratio!
	osg::Geometry* quad = osg::createTexturedQuadGeometry(
			osg::Vec3(-width/2.0f, -height/2.0f, 0),
			osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0), 1, 1);
	addDrawable(quad);
	osg::ref_ptr<osg::StateSet> stateSet = getOrCreateStateSet();

	osg::ref_ptr<osg::Texture2D> texture = Util::TextureWrapper::createTexture(widgetImage);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
//	stateSet->setAttribute(new osg::Program);

	// this enables interaction with the widget (buggy), use current camera
	// nothing is shown without this code!
	osg::Camera* camera = 0;
	osgViewer::InteractiveImageHandler* handler =
			new osgViewer::InteractiveImageHandler(widgetImage.get(), texture, camera);
	quad->setEventCallback(handler);
	quad->setCullCallback(handler);

	setStateSet(stateSet);
}

bool WidgetContent::load() {
	return false;
}

