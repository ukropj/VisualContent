#ifndef VIEWER_TEXTURE_WRAPPER_DEF
#define VIEWER_TEXTURE_WRAPPER_DEF 1

#include <sstream>

#include <QString>

#include <osg/ImageSequence>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/ref_ptr>


namespace Util {
/**
 *  \class TextureWrapper
 *  \brief Texture operations wrapper
 *  \author Michal Paprcka
 */
class TextureWrapper {
public:
	/// Reads textures in PNG format.
	static osg::ref_ptr<osg::Texture2D> readTextureFromFile(QString path, bool repeat = false);

	/// Creates texture from image
	static osg::ref_ptr<osg::Texture2D> createTexture(osg::Image* image, bool repeat = false);

	static osg::ref_ptr<osg::Texture2D> getNodeTexture();
	static osg::ref_ptr<osg::Texture2D> getClusterTexture();
	static osg::ref_ptr<osg::Texture2D> getFrameTexture();
	static osg::ref_ptr<osg::Texture2D> getDirectedEdgeTexture();
	static osg::ref_ptr<osg::Texture2D> getEdgeTexture();

	/// Reloads all textures
	static void reloadTextures();

	/**
	 *  \fn public static  getCoudTexture(int w, int h, int r, int g, int b, int alpha)
	 *  \brief Generates perlin noise with given parameters and creates a texture from it
	 *  \param        w     texture width
	 *  \param        h     texture height
	 *  \param        r     red amount
	 *  \param        g     green amount
	 *  \param        b     blue amount
	 *  \param        alpha    alpha amount
	 *  \return osg::ref_ptr texture
	 */
	static osg::ref_ptr<osg::Texture2D> createCloudTexture(int w, int h, osg::Vec4s color1, osg::Vec4s color2);

private:

	static osg::ref_ptr<osg::Texture2D> nodeTexture;
	static osg::ref_ptr<osg::Texture2D> clusterTexture;
	static osg::ref_ptr<osg::Texture2D> frameTexture;
	static osg::ref_ptr<osg::Texture2D> edgeTexture;
	static osg::ref_ptr<osg::Texture2D> directedEdgeTexture;

};
}

#endif
