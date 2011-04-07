#include "Util/TextureWrapper.h"
#include "Util/Config.h"
#include <osgDB/ReadFile>
#include <QDebug>

using namespace Util;

osg::ref_ptr<osg::Texture2D> TextureWrapper::nodeTexture = NULL;
osg::ref_ptr<osg::Texture2D> TextureWrapper::clusterTexture = NULL;
osg::ref_ptr<osg::Texture2D> TextureWrapper::frameTexture = NULL;
osg::ref_ptr<osg::Texture2D> TextureWrapper::edgeTexture = NULL;
osg::ref_ptr<osg::Texture2D> TextureWrapper::directedEdgeTexture = NULL;

osg::ref_ptr<osg::Texture2D> TextureWrapper::readTextureFromFile(QString path, bool repeat) {
	osg::ref_ptr<osg::Texture2D> texture = NULL;
	if (path != NULL) {
		osg::Image* img = osgDB::readImageFile(path.toStdString());
		texture = createTexture(img, repeat);
	}
	if (texture == NULL)
		qWarning() << "Unable to read texture \"" << path << "\" from file.";
	return texture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::createTexture(osg::Image* image, bool repeat) {
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
	texture->setResizeNonPowerOfTwoHint(false);
	texture->setDataVariance(osg::Object::STATIC);

	osg::Texture::WrapMode wrapMode;
	if (repeat) {
		wrapMode = osg::Texture::REPEAT;
	} else {
		wrapMode = osg::Texture::CLAMP_TO_EDGE;
	}
	texture->setWrap(osg::Texture::WRAP_S, wrapMode);
	texture->setWrap(osg::Texture::WRAP_T, wrapMode);
	return texture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getNodeTexture() {
	if (nodeTexture == NULL)
		nodeTexture = readTextureFromFile(Config::getValue(
				"Viewer.Textures.Node"));
	return nodeTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getClusterTexture() {
	if (clusterTexture == NULL)
		clusterTexture = readTextureFromFile(Config::getValue(
				"Viewer.Textures.Cluster"));
	return clusterTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getFrameTexture() {
	if (frameTexture == NULL)
		frameTexture = readTextureFromFile(Config::getValue(
				"Viewer.Textures.Frame"));
	return frameTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getEdgeTexture() {
	if (edgeTexture == NULL)
		edgeTexture = readTextureFromFile(Config::getValue(
				"Viewer.Textures.Edge"), true);
	return edgeTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getDirectedEdgeTexture() {
	if (directedEdgeTexture == NULL) {
		osg::ref_ptr<osg::ImageSequence> imageSequence = new osg::ImageSequence;
		imageSequence->setMode(osg::ImageSequence::PRE_LOAD_ALL_IMAGES);

		for (int x = 0; x < 15; x++) {
			std::stringstream os;
			os << Config::getValue("Viewer.Textures.DirectedEdgePrefix").toStdString();
			os << x;
			os << Config::getValue("Viewer.Textures.DirectedEdgeSuffix").toStdString();

			osg::ref_ptr<osg::Image> image = osgDB::readImageFile(os.str());
			if (image.valid()) {
				imageSequence->addImage(image.get());
			}
		}

		imageSequence->setLength(0.4);
		imageSequence->setLoopingMode(osg::ImageStream::LOOPING);
		imageSequence->play();

		directedEdgeTexture = new osg::Texture2D;
		directedEdgeTexture->setImage(imageSequence.get());

		directedEdgeTexture->setDataVariance(osg::Object::DYNAMIC);
		directedEdgeTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		directedEdgeTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	}

	return directedEdgeTexture;
}

void TextureWrapper::reloadTextures() {
	nodeTexture = NULL;
	clusterTexture = NULL;
	edgeTexture = NULL;
	directedEdgeTexture = NULL;
}

#include <noise.h>
#include <Noise/NoiseUtils.h>

using namespace noise;

//w and h speak for themselves, zoom wel zoom in and out on it, I usually  use 75. P stands for persistence, this controls the roughness of the picture, i use 1/2
osg::ref_ptr<osg::Texture2D> TextureWrapper::createCloudTexture(int w, int h,
		osg::Vec4s color1, osg::Vec4s color2) {
	module::Perlin perlinModule;

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderSphere heightMapBuilder;

	heightMapBuilder.SetSourceModule(perlinModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(w, h);
	heightMapBuilder.SetBounds(-90.0, 90.0, -180.0, 180.0);

	heightMapBuilder.Build();

	utils::RendererImage renderer;
	utils::Image image;

	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);

	renderer.ClearGradient();
	renderer.AddGradientPoint(-1.0000,
			utils::Color(color1.x(), color1.y(), color1.z(), color1.w()));
	renderer.AddGradientPoint(1.0000,
			utils::Color(color2.x(), color2.y(), color2.z(), color2.w()));

	renderer.Render();

	char * data = new char[w * h * 3];

	int index = 0;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			data[index++] = image.GetValue(x, y).red;
			data[index++] = image.GetValue(x, y).green;
			data[index++] = image.GetValue(x, y).blue;
		}
	}

	osg::ref_ptr<osg::Image> img = new (std::nothrow) osg::Image;
	img->setImage(w, h, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
			(unsigned char *) data, osg::Image::USE_NEW_DELETE);

	osg::ref_ptr<osg::Texture2D> tex = createTexture(img);

	return tex;
}
