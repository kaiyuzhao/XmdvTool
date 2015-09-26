/*
 * Image.h
 *
 *  Created on: May 12, 2009
 *
 *  Used by Subwindow
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

class GLCanvas;

///
/// Image class definition
///
/// USAGE:
/// 	Image *img = new Image(640, 480);
///		img->clear(255,255,255);
///		img->WritePixel(0,0,127,127,127);
///
/// \date 02-05-2004
/// \author anilkpatro
/// \version 2.0 - Added texturing capabilities
///

class Image {
public:

	///
	///	Constructor - initialize the image with the specified height
	/// and width
	///
	/// \param w width of the image
	/// \param h height of the image
	///
	Image(unsigned int w, unsigned int h);

	Image(GLenum texnum);

	///
	/// Copy Constructor
	///
	/// \param img object to be cloned
	///
	Image(const Image& from);

	///
	/// Destructor
	///
	virtual ~Image();

	///
	/// Create the image from the contents of the screen
	///
	/// \param canvas canvas from which to read the pixels
	/// \param w width of the image
	/// \param h height of the image
	/// \return the read Image
	///
	/// NOTE: not 100% sure if working, never called anywhere?
	static Image *ReadPixels(GLCanvas *canvas, unsigned int w, unsigned int h);

	///
	/// Assignment operator
	///
	/// \param from the value to assign to this object
	/// \return A reference to this object
	///
	Image& operator = (const Image& from);

	///
	/// Clears the image
	///
	/// \param r Red component of color (0 to 255)
	/// \param g Green component of color (0 to 255)
	/// \param b Blue component of color (0 to 255)
	/// \param a Alpha component of color (0 to 255)
	///
	void	clear(unsigned char r, unsigned char g,
			unsigned char b, unsigned char a = 255);

	///
	/// Set pixel value at a particular point in the image
	///
	/// \param x X position of the point (0 to width)
	/// \param y Y position of the point (0 to height)
	/// \param r Red component of color (0 to 255)
	/// \param g Green component of color (0 to 255)
	/// \param b Blue component of color (0 to 255)
	/// \param a Alpha component of color (0 to 255)
	///
	void	WritePixel(unsigned int x, unsigned int y, unsigned char r,
			unsigned char g, unsigned char b, unsigned char a = 255);
	void	WritePixel(unsigned int x, unsigned int y, unsigned long color);

	///
	/// Draw the image to the screen starting from a particular offset.
	/// So image will be drawn on a rectangle specified by
	/// (xOffset, yOffset, width, height)
	///
	/// \param xOffset starting point in x direction
	/// \param yOffset starting point in y direction
	///
	void	Draw(int xOffset = 0, int yOffset = 0) const;

	///
	/// Enable texturing mode if you want to use the image as a texture
	///
	void	EnableTexturing();

	///
	/// Disable texturing mode
	///
	void	DisableTexturing();

	///
	/// Create an opengl texture using the image
	///
	void	CreateTexture();

	///
	/// Delete the created texture
	///
	void	DeleteTexture();

	///
	/// Display the image by using texturing rather than directly on the
	/// display. To use direct drawing, use the Draw method
	///
	/// \param x X position of the quad
	/// \param y Y position of the quad
	/// \param width width of the quad
	/// \param height height of the quad
	/// \param u texture coordinate
	/// \param v texture coordinate
	///
	void	Display(float x, float y, float width, float height,
			float u = 1.0f, float v = 1.0f);

	/// ACCESS
	int		Width() const;
	int		Height() const;

private:	// members

	unsigned int	m_width,		///< image width
					m_height;		///< image height

	unsigned char	*m_pixels;		///< image data

	GLuint	m_texnum;				///< texture data
	bool	m_hasTexture;			///< has the texture been assigned
};

// INLINE METHODS
//-----------------------------------------------------------------------------
inline int Image::Width() const
{
	// return the requested data
	return m_width;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline int Image::Height() const
{
	// return the requested data
	return m_height;
}
//-----------------------------------------------------------------------------

#endif /* IMAGE_H_ */
