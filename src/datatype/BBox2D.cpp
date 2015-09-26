/*
 * BBox2D.cpp
 *
 *  Created on: May 12, 2009
 */

#include "datatype/BBox2D.h"

//-----------------------------------------------------------------------------
bool BBox2D::IsInside(const BBox2D& box) const
{
	if ( (m_maxx <= box.m_maxx) && (m_minx >= box.m_minx)
		&& (m_maxy <= box.m_maxy) && (m_miny >= box.m_miny) )
		return true;
	return false;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool BBox2D::IsOverlapping(const BBox2D& box) const
{
	if ( (m_maxx < box.m_minx) || (m_minx > box.m_maxx)
		|| (m_maxy < box.m_miny) || (m_miny > box.m_maxy) )
		return false;
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void BBox2D::Add(double x, double y)
{
	if ( x < m_minx ) m_minx = x;
	if ( y < m_miny ) m_miny = y;
	if ( x > m_maxx ) m_maxx = x;
	if ( y > m_maxy ) m_maxy = y;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void BBox2D::Add(const BBox2D& box)
{
	if ( box.m_minx < m_minx ) m_minx = box.m_minx;
	if ( box.m_miny < m_miny ) m_miny = box.m_miny;
	if ( box.m_maxx > m_maxx ) m_maxx = box.m_maxx;
	if ( box.m_maxy > m_maxy ) m_maxy = box.m_maxy;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void BBox2D::SetScale(double scale)
{
	if (!IsValid()) return;
	if (scale < 1e-6) return;

	double width, height, diff;
	width = m_maxx-m_minx;
	height = m_maxy-m_miny;
	if ( scale > width/height ) {
		diff = (scale*height - width)/2.0;
		m_minx -= diff;
		m_maxx += diff;
	} else {
		diff = (width/scale - height)/2.0;
		m_miny -= diff;
		m_maxy += diff;
	}
}
