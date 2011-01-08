/**
 *  TextureWrapper.h
 *  Projekt 3DVisual
 */
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
 *  \date 29. 4. 2010
 */
class TextureWrapper {
public:
	/**
	 * Nacita textury vo formate PNG.
	 *
	 **/
	static osg::ref_ptr<osg::Texture2D> readTextureFromFile(QString path, bool repeat = false);

	/**
	 *  \fn public static  createTexture
	 *  \brief Creates texture from image
	 *  \param      image    texture image
	 *  \return osg::ref_ptr texture
	 */
	static osg::ref_ptr<osg::Texture2D> createTexture(osg::Image* image, bool repeat = false);

	/**
	 *  \fn public static  getNodeTexture
	 *  \brief Returns default node texture
	 *  \return osg::ref_ptr node texture
	 */
	static osg::ref_ptr<osg::Texture2D> getNodeTexture();

	/**
	 *  \fn public static  getOrientedEdgeTexture
	 *  \brief Returns default oriented edge texture
	 *  \return osg::ref_ptr<osg::Texture2D> oriented edge texture
	 */
	static osg::ref_ptr<osg::Texture2D> getOrientedEdgeTexture();

	/**
	 *  \fn public static  getEdgeTexture
	 *  \brief Returns default edge texture
	 *  \return osg::ref_ptr edge texture
	 */
	static osg::ref_ptr<osg::Texture2D> getEdgeTexture();

	/**
	 *  \fn public static  reloadTextures
	 *  \brief Reloads all textures
	 */
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
	static osg::ref_ptr<osg::Texture2D> createCloudTexture(int w, int h, int r,
			int g, int b, int alpha);

private:

	/**
	 *  osg::ref_ptr nodeTexture
	 *  \brief default node texture
	 */
	static osg::ref_ptr<osg::Texture2D> nodeTexture;

	/**
	 *  osg::ref_ptr<osg::Texture2D> edgeTexture
	 *  \brief default edge texture
	 */
	static osg::ref_ptr<osg::Texture2D> edgeTexture;

	/**
	 *  osg::ref_ptr orientedEdgeTexture
	 *  \brief default oriented edge texture
	 */
	static osg::ref_ptr<osg::Texture2D> orientedEdgeTexture;

};
}

#endif
