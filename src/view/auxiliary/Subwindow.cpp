/*
 * Subwindow.cpp
 *
 *  Created on: May 13, 2009
 */

#include "view/auxiliary/Subwindow.h"

#include <stdlib.h>
#include <assert.h>

#include "main/XmdvTool.h"
#include "operator/brush/Brush.h"
#include "datatype/RGBt.h"
#include "color/ColorManager.h"
#include "operator/sbb/HierDisplayInformation.h"

namespace
{
double samplingFactor;
};

Subwindow::Subwindow(int dim)
: m_box()
{
	m_dimension = dim;
	m_size = -1;
}

Subwindow::Subwindow(const Subwindow& from)
: m_box(from.m_box)
{
	m_dimension = from.m_dimension;
	m_size = from.m_size;
	m_highColor = from.m_highColor;
	m_lowColor = from.m_lowColor;
}

Subwindow::~Subwindow() {
}

Subwindow& Subwindow::operator = (const Subwindow& from)
{
	if (this == &from)
		return *this;

	m_box = from.m_box;
	m_dimension = from.m_dimension;
	m_size = from.m_size;
	m_highColor = from.m_highColor;
	m_lowColor = from.m_lowColor;

	return *this;
}

void Subwindow::Paint(Image *img,const PixelInfo& flat_pixel_info,
		int dim_size,
		double startSample /*=0*/,double endSample /*=1.0*/,
		DisplayType type, bool hilite /*=false*/)
{
	assert(m_box.IsValid());

	// hilite dimension if it's the current one
	if (hilite) {
		glLineWidth(5.0f);
		RGBt color(g_globals.colorManager->getColor(ColorManager::PIXEL_HIGHLIGHT));
		glColor3f((float) color.R, (float) color.G, (float) color.B);
		glBegin(GL_LINE_LOOP); {
			glVertex2d(m_box.Lowx(),  m_box.Lowy());
			glVertex2d(m_box.Highx(), m_box.Lowy());
			glVertex2d(m_box.Highx(), m_box.Highy());
			glVertex2d(m_box.Lowx(),  m_box.Highy());
		} glEnd();
		glLineWidth(1.0f);
	}

	// draw the spiral arrangement of pixels on the subwindow image
	m_size = img->Width()-1;
	sspiral(startSample, endSample, img, m_dimension, flat_pixel_info, dim_size, type);
	img->CreateTexture();
	img->Display((float) m_box.Lowx(), (float) m_box.Highy(), (float) m_box.Highx(), (float) m_box.Lowy());
	img->DeleteTexture();
}

void Subwindow::PaintHier(Image *img, const PixelInfo& flat_pixel_info,
		int dim_size, HierDisplayInformation *hier,
		DisplayType type, bool hilite)
{
	assert(m_box.IsValid());

	// hilite dimension if it's the current one
	if (hilite) {
		glLineWidth(5.0f);
		RGBt color(g_globals.colorManager->getColor(ColorManager::PIXEL_HIGHLIGHT));
		glColor3f((float) color.R, (float) color.G, (float) color.B);
		glBegin(GL_LINE_LOOP); {
			glVertex2d(m_box.Lowx(),  m_box.Lowy());
			glVertex2d(m_box.Highx(), m_box.Lowy());
			glVertex2d(m_box.Highx(), m_box.Highy());
			glVertex2d(m_box.Lowx(),  m_box.Highy());
		} glEnd();
		glLineWidth(1.0f);
	}

	// draw the spiral arrangement of pixels on the subwindow image
	m_size = img->Width()-1;
	sspiral(0, 0, img, m_dimension, flat_pixel_info, dim_size, type, hier);
	img->CreateTexture();
	img->Display((float) m_box.Lowx(), (float) m_box.Highy(), (float) m_box.Highx(), (float) m_box.Lowy());
	img->DeleteTexture();
}

bool Subwindow::Contains(float xx, float yy, int &n, Subwindow::DisplayType displayType, int &rx, int &ry)
{
	if (m_box.IsInside(xx, yy) && m_size >= 0)
	{
		double xRatio = (xx - m_box.Lowx()) / m_box.Width();
		double yRatio = (m_box.Highy() - yy) / m_box.Height();

		int x = (int) (m_size * xRatio), y = (int) (m_size * yRatio);
		x = x - m_size/2; y = y - m_size/2;




		if (displayType == Subwindow::PDT_SPIRAL) {

			rx = x;
			ry = y;
			// from: http://mcraefamily.com/MathHelp/CountingRationalsSquareSpiral1.htm
			//
			int shell = 0, leg = 0, elem = 0;

			//
			// Given (x,y),
			//
			// shell -- is the maximum of the distance in either dimension from the
			// origin, in other words: max(abs(x),abs(y))
			//
			//shell = max(abs(x), abs(y));
			shell = (abs(x) > abs(y)) ? abs(x) : abs(y);

			// leg -- if you think of the plane of ordered numbers as being inside
			// a big "X", then the leg is the quadrant in that x.  In other words,
			//	if (x,y) is closest to the positive x axis, then leg is 0
			//	if (x,y) is closest to the positive y axis, then leg is 1
			//	if (x,y) is closest to the negative x axis, then leg is 2
			//	if (x,y) is closest to the negative y axis, then leg is 3
			//
			//	and a "tie" goes to the lower leg number (a tie between legs 3 and
			//	0 goes to leg 3)
			//
			leg = ((y <= x) & (y > -x)) ? 0 : ((y >= -x) & (y > x)) ? 1 : ((y
					>= x) & (y < -x)) ? 2 : 3;

			// element is the directed distance from the axis, measured along the
			// spiral.
			elem = leg == 0 ? y : leg == 1 ? -x : leg == 2 ? -y : x;

			// then n is
			n = (4* shell * shell) + (2* leg - 3) * shell + elem;

			return true;
		} else if (displayType == Subwindow::PDT_HORIZONTAL){
			rx = x + m_size/2;
			ry = y + m_size/2;
			n = m_size * ry + rx;
			return true;
		}else if (displayType == Subwindow::PDT_VERTICAL){
			rx = x + m_size/2;
			ry = m_size/2 - y;
			n = m_size * rx + ry;
			return true;
		}
	}

	return false;
}

void Subwindow::sspiral(double startSample, double endSample, Image *rgb, int dim,
		const PixelInfo &pixel_info, int dim_size, DisplayType type, HierDisplayInformation *hier)
{
	double brushedOpacity = 1.0, unbrushedOpacity = 1.0;//, opacity = 1.0;
	if (hier) {
		samplingFactor = 1.0f;
		if (hier->fade_factor>0)
			unbrushedOpacity = 1-hier->fade_factor;
		else if (hier->fade_factor<0)
			brushedOpacity = 1+hier->fade_factor;
	}

	// two vector arrays for increased speed
	int vx[4] = {0,1,0,-1},
			vy[4] = {-1,0,1,0};

	if(type == PDT_HORIZONTAL) {
		vy[0] = 1;
	} else if(type == PDT_VERTICAL) {
		vx[0] = 1;
		vx[1] = 0;
		vx[2] = 1;
		vx[3] = 0;
		vy[0] = 0;
		vy[1] = 1;
		vy[2] = 0;
		vy[3] = -1;
	}

	int drawcount = 0, // number of drawn data points
			count = 0, // number of data points
			dir = 1,   // direction that the spiral will start out in
			len = 1;   // length of the first side to be drawn

	if(type == PDT_HORIZONTAL) {
		len = rgb->Width()-1;
	} else if(type == PDT_VERTICAL) {
		len = rgb->Height()-1;
	}

	int sz = rgb->Width()-1;
	int x = sz/2, y = sz/2;

	if(type == PDT_HORIZONTAL || type == PDT_VERTICAL) {
		x = 0;
		y = 0;
	}

	int curr_drawcount = 0;
	int tot_drawcount = 0;

	RGBt finalColor = 0xFFFFFF;

	rgb->clear(0, 0, 0, 255);

	// while there is data
	while (1)
	{
		for (int j=2; j>0; --j)
		{
			for (int i=len; i>0; --i)
			{
				++tot_drawcount;
				if (drawcount >= ((sz+1)*(sz+1)))
					return;

				if (curr_drawcount && curr_drawcount < pixel_info.sizes[count]) {
					// draw just whites
					rgb->WritePixel(x, y, finalColor.getColor());
					++drawcount;
					// get to next location
					x += vx[dir]; y += vy[dir];
					++curr_drawcount;
					continue;
				} else {
					if (curr_drawcount > 0) {
						curr_drawcount = 0;
						++count;
					}
				}

				// if reached at end of data
				if (count >= pixel_info.data_size) {
					return;
					// draw just whites
					RGBt bgColor = 0xFFFFFF;
					rgb->WritePixel(x, y, bgColor.getColor());
					++drawcount;
					// get to next location
					x += vx[dir]; y += vy[dir];
					++count;
					curr_drawcount = 0;
					continue;
				}

				int idx = count * dim_size + dim;
				double val = pixel_info.norm_values[idx];

				if(!hier){
					if (pixel_info.randomNums[count] < startSample ||
							pixel_info.randomNums[count] > endSample)
					{
						++i; ++count; continue;
					}
				}
				// color the pixel at the current location
				finalColor = g_globals.colorManager->getColor(ColorManager::BRUSH_UNSELECTED, val);
				rgb->WritePixel(x, y, finalColor.getColor());
				++drawcount;

				// get to next location
				x += vx[dir]; y += vy[dir];
				// go to next data point
				curr_drawcount = 1;
			}
			// should we change direction now?
			dir = (dir + 1) % 4;
			if(type == PDT_HORIZONTAL || type == PDT_VERTICAL) {
				if(len == 1) {
					if(type == PDT_HORIZONTAL)
						len = rgb->Width()-1;
					else
						len = rgb->Height()-1;
				} else
					len = 1;
			}
		}
		// advance spiral outwards on each half revolution by 1 pixel
		if(type == PDT_SPIRAL)
			++len;
	}
}
