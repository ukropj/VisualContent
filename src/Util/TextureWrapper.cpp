#include "Util/TextureWrapper.h"
#include "Util/Config.h"

using namespace Util;

osg::ref_ptr<osg::Texture2D> TextureWrapper::nodeTexture;
osg::ref_ptr<osg::Texture2D> TextureWrapper::edgeTexture;
osg::ref_ptr<osg::Texture2D> TextureWrapper::orientedEdgeTexture;


osg::ref_ptr<osg::Texture2D> TextureWrapper::readTextureFromFile(QString path) {
	osg::ref_ptr<osg::Texture2D> texture = NULL;

	if (path != NULL) {
		texture = new osg::Texture2D;
		texture->setImage(osgDB::readImageFile(path.toStdString()));

		texture->setDataVariance(osg::Object::DYNAMIC);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	}

	if (path == NULL || texture == NULL)
		qWarning("Unable to read texture from file.");

	return texture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::createTexture(// todo private?
		osg::ref_ptr<osg::Image> image) {
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;

	texture->setImage(image);

	texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

	return texture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getNodeTexture()
{
	if(nodeTexture == NULL)
		nodeTexture = readTextureFromFile(Config::getValue("Viewer.Textures.Node"));
	return nodeTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getOrientedEdgeTexture()
{
	if(orientedEdgeTexture == NULL)
	{
		osg::ref_ptr<osg::ImageSequence> imageSequence = new osg::ImageSequence;
		imageSequence->setMode(osg::ImageSequence::PRE_LOAD_ALL_IMAGES);

		for (int x = 0; x < 15; x++)
		{
			std::stringstream os;
			os << Config::getValue("Viewer.Textures.OrientedEdgePrefix").toStdString();
			os << x;
			os << Config::getValue("Viewer.Textures.OrientedEdgeSuffix").toStdString();

			osg::ref_ptr<osg::Image> image = osgDB::readImageFile(os.str());
			if (image.valid())
			{
				imageSequence->addImage(image.get());
			}
		}

		imageSequence->setLength(0.4);
		imageSequence->setLoopingMode(osg::ImageStream::LOOPING);
		imageSequence->play();

		orientedEdgeTexture = new osg::Texture2D;
		orientedEdgeTexture->setImage(imageSequence.get());

		orientedEdgeTexture->setDataVariance(osg::Object::DYNAMIC);
		orientedEdgeTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		orientedEdgeTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	}

	return orientedEdgeTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getEdgeTexture()
{
	if(edgeTexture == NULL)
		edgeTexture = readTextureFromFile(Config::getValue("Viewer.Textures.Edge"));
	return edgeTexture;
}

void TextureWrapper::reloadTextures()
{
	nodeTexture = NULL;
	edgeTexture = NULL;
	orientedEdgeTexture = NULL;
}

#include <noise.h>
#include <Noise/NoiseUtils.h>

using namespace noise;

osg::ref_ptr<osg::Texture2D> TextureWrapper::getCoudTexture(int w, int h, int r, int g, int b, int alpha)//w and h speak for themselves, zoom wel zoom in and out on it, I usually  use 75. P stands for persistence, this controls the roughness of the picture, i use 1/2
{
	module::Perlin perlinModule;

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderSphere heightMapBuilder;

	heightMapBuilder.SetSourceModule (perlinModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (w, h);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);

	heightMapBuilder.Build ();

	utils::RendererImage renderer;
	utils::Image image;

	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);

	renderer.ClearGradient ();
	renderer.AddGradientPoint (-1.0000, utils::Color (r, g, b, alpha));
	renderer.AddGradientPoint ( 1.0000, utils::Color (0, 0, 0, 255));

	renderer.Render ();

	char * data = new char[w * h * 3];

	int index = 0;

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			data[index++] = image.GetValue(x, y).red;
			data[index++] = image.GetValue(x, y).green;
			data[index++] = image.GetValue(x, y).blue;
		}
	}

	osg::ref_ptr<osg::Image> i = new(std::nothrow) osg::Image;
	i->setImage(w, h, 1,  GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *) data, osg::Image::USE_NEW_DELETE);

	osg::ref_ptr<osg::Texture2D> tex = createTexture(i);

	return tex;
}
