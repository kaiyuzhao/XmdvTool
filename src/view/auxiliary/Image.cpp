/*
 * Image.cpp
 *
 *  Created on: May 12, 2009
 */

#include "view/auxiliary/Image.h"
#include "view/GLCanvas.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#include <cassert>

Image::Image(unsigned int w, unsigned int h)
	: m_width(w), m_height(h)
{
	m_hasTexture = false;
	unsigned int size = w * h * 4;
	m_pixels = new unsigned char[size];

}

Image::Image(GLenum texnum)
{
	m_hasTexture = true;
	m_texnum = texnum;
	m_pixels = NULL;
}

Image::Image(const Image& i)
{
	m_hasTexture = false;
	m_width = i.m_width;
	m_height = i.m_height;
	unsigned int size = m_width * m_height * 4 + 3;
	m_pixels = new unsigned char[size];
	memcpy(m_pixels, i.m_pixels, size*sizeof(unsigned char));
}

Image::~Image() {
	if (m_pixels)
			delete[] m_pixels;
		DeleteTexture();
}

Image *Image::ReadPixels(GLCanvas *canvas,
									unsigned int w, unsigned int h)
{
	//canvas->Refresh();
	canvas->flush();

	GLenum texnum;
	glGenTextures(1, &texnum);
	Image *img = new Image(texnum);
	img->EnableTexturing();
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, w, h, 0);
	img->m_hasTexture = true;

	GLenum err = glGetError();
	switch (err) {
	case GL_INVALID_ENUM: std::cerr << "Invalid enum" << std::endl; break;
	case GL_INVALID_VALUE: std::cerr << "Invalid value" << std::endl; break;
	case GL_INVALID_OPERATION: std::cerr << "Invalid operation" << std::endl; break;
	}

	return img;
}

Image& Image::operator = (const Image& i)
{
	if (this == &i)
		return *this;

	delete [] m_pixels;
	m_width = i.m_width;
	m_height = i.m_height;
	unsigned int size = m_width * m_height * 4;
	m_pixels = new unsigned char[size];
	memcpy(m_pixels, i.m_pixels, size*sizeof(unsigned char));

	return *this;
}

void Image::clear(unsigned char r, unsigned char g, unsigned char b,
				  unsigned char a)
{
	// zip over the image and set each color to the clear color
	for (unsigned int i = 0; i < m_width*m_height; i++)
	{
		m_pixels[i * 4 + 0] = r;
		m_pixels[i * 4 + 1] = g;
		m_pixels[i * 4 + 2] = b;
		m_pixels[i * 4 + 3] = a;
	}
}

void Image::WritePixel(unsigned int  x, unsigned int  y, unsigned char r,
					   unsigned char g, unsigned char b, unsigned char a)
{
	// make sure the pixel is on the image
	unsigned int idx	= y *m_width + x;
	assert( (x < m_width) && (y < m_height) );
	assert (idx*4+3 <= 4*m_width*m_height+3);

	// store the pixel color
	m_pixels[idx * 4 + 0] = r;
	m_pixels[idx * 4 + 1] = g;
	m_pixels[idx * 4 + 2] = b;
	m_pixels[idx * 4 + 3] = a;
}

void Image::WritePixel(unsigned int x, unsigned int y, unsigned long color)
{
	WritePixel(x, y, (unsigned char) (color >> 16) & 0xff,
					 (unsigned char) (color >> 8) & 0xff,
					 (unsigned char) color & 0xff);
}

void Image::Draw(int xOffset /*=0*/, int yOffset /*=0*/) const
{
	// Disable stuff that's likely to slow down glDrawPixels.
	// (Omit as much of this as possible, when you know in advance
	// that the OpenGL state will already be set correctly.)
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
	glPixelTransferi(GL_RED_SCALE, 1);
	glPixelTransferi(GL_RED_BIAS, 0);
	glPixelTransferi(GL_GREEN_SCALE, 1);
	glPixelTransferi(GL_GREEN_BIAS, 0);
	glPixelTransferi(GL_BLUE_SCALE, 1);
	glPixelTransferi(GL_BLUE_BIAS, 0);
	glPixelTransferi(GL_ALPHA_SCALE, 1);
	glPixelTransferi(GL_ALPHA_BIAS, 0);

	// Disable extensions that could slow down glDrawPixels.
	/*const GLubyte* extString = glGetString(GL_EXTENSIONS);

	if (extString != NULL) {
		if (strstr((char*) extString, "GL_EXT_convolution") != NULL) {
			glDisable(GL_CONVOLUTION_1D_EXT);
			glDisable(GL_CONVOLUTION_2D_EXT);
			glDisable(GL_SEPARABLE_2D_EXT);
		}
		if (strstr((char*) extString, "GL_EXT_histogram") != NULL) {
			glDisable(GL_HISTOGRAM_EXT);
			glDisable(GL_MINMAX_EXT);
		}
		if (strstr((char*) extString, "GL_EXT_texture3D") != NULL) {
			glDisable(GL_TEXTURE_3D_EXT);
		}
	}*/

	// make sure we are packed properly
	glPixelStorei(GL_UNPACK_ALIGNMENT, sizeof(unsigned char));

	// make sure the corner is set up correctly
	glRasterPos2i(xOffset, yOffset);

	// draw the pixels using the openGL command
	glDrawPixels(m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE,
				 static_cast<void*>(m_pixels));
}

void Image::EnableTexturing()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texnum);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
}

void Image::DisableTexturing()
{
	glDisable(GL_TEXTURE_2D);
}

void Image::CreateTexture()
{
	DeleteTexture();
	glGenTextures(1, &m_texnum);
	glBindTexture(GL_TEXTURE_2D, m_texnum);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_width, m_height, GL_RGBA,
					  GL_UNSIGNED_BYTE, m_pixels);

	m_hasTexture = true;
}

void Image::DeleteTexture()
{
	if (m_hasTexture)
		glDeleteTextures(1, &m_texnum);
	m_hasTexture = false;
}

void Image::Display(float x1, float y1, float x2, float y2,
					float u /*= 1.0f*/, float v /*= 1.0f*/)
{
	glPushMatrix();
	EnableTexturing();
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x1, y1);
	glTexCoord2f(u, 0.0f); glVertex2f(x2, y1);
	glTexCoord2f(u, v); glVertex2f(x2, y2);
	glTexCoord2f(0.0f, v); glVertex2f(x1, y2);
	glEnd();
	glPopMatrix();
}
