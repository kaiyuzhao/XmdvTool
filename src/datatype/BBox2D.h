/*
 * BBox2D.h
 *
 *  Created on: May 12, 2009
 *
 *  Used by Subwindow
 */

#ifndef BBOX2D_H_
#define BBOX2D_H_

#include "datatype/Vec2.h"

///
/// A bounding box is used to calculate the axis-aligned space
/// represented by a minimum and maximum extents of the box.
///
/// USAGE:
/// <pre>
///		BBox2D box;
///		box.Add(-1.0, -1.0);
///		box.Add(1.0, 1.0);
///		assert(box.IsInside(0.0, 0.0) == true);
/// </pre>
/// \date 7/14/2004
/// \author anilkpatro
///

class BBox2D {
public:
	///
	/// Default Constructor - extents of the box are invalid
	///
	BBox2D();

	///
	/// Constructor taking a point
	///
	/// \param x X coordinate of point
	/// \param y Y coordinate of point
	BBox2D(double x, double y);

	///
	/// Constructor taking a box
	///
	/// \param minx Min x extent of box
	/// \param miny Min y extent of box
	/// \param maxx Max x extent of box
	/// \param maxy Max y extent of box
	BBox2D(double minx, double miny, double maxx, double maxy);

	///
	/// Copy constructor
	///
	/// \param from the value to copy to this object
	///
	BBox2D(const BBox2D& from);

	///
	/// Destructor
	///
	virtual ~BBox2D();

	///
	/// Assignment operator
	///
	/// \param box the value to assign to this object
	/// \return reference to this object
	BBox2D& operator = (const BBox2D& box);

	///
	/// Comparison operator
	///
	/// \param box the value to compare to this object
	/// \return comparison status
	bool operator == (const BBox2D& box);

	///
	/// Invalidate (reset) the extents of the box to 1,1,-1,-1
	///
	void Invalidate();

	///
	/// Add a point to this bounding box, growing it if necessary
	///
	/// \param x X coordinate of the point
	/// \param y Y coordinate of the point
	void Add(double x, double y);

	///
	/// Add another bounding box to this bounding box, growing it if necessary
	///
	/// \param box A bounding box to add to this box
	void Add(const BBox2D& box);

	///
	/// Translate the boounding box
	///
	/// \param tx
	/// \param ty
	void Shift(double tx, double ty);

	///
	/// Scale the bounding box by a delta factor
	///
	/// \param d delta factor
	void Inflate(double d);

	/// ACCESS
	double Lowx() const;
	double Lowy() const;
	double Highx() const;
	double Highy() const;

	double Width() const;
	double Height() const;
	Vec2 Center() const;
	void SetScale(double scale);

	///
	/// Status whether the extents of the box are valid (false if min > max)
	///
	/// \return true if box is valid
	bool IsValid() const;

	///
	/// Check whether the given point is inside the bounding box
	///
	/// \param x X coordinate of the point
	/// \param y Y coordinate of the point
	/// \return true if point is inside
	bool IsInside(double x, double y) const;

	///
	/// Check whether the given box is contained within this bounding box
	///
	/// \param box Box extents to check
	/// \return true if box is contained
	bool IsInside(const BBox2D& box) const;

	///
	/// Check if the given bounding box is intersecting with this bounding
	/// box
	///
	/// \param box Box extents to check
	/// \return true if the given box is intersecting
	bool IsOverlapping(const BBox2D& box) const;

protected:	// members

	double 	m_minx, m_miny,	///< Smallest x, y components of the box
			m_maxx, m_maxy;	///< Largest x, y components of the box
};

// INLINE METHODS
//-----------------------------------------------------------------------------
inline BBox2D::BBox2D() : m_minx(-1), m_miny(-1), m_maxx(1), m_maxy(1)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline BBox2D::BBox2D(double x, double y)
: m_minx(x), m_miny(y), m_maxx(x), m_maxy(y)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline BBox2D::BBox2D(double minx, double miny, double maxx, double maxy)
: m_minx(minx), m_miny(miny), m_maxx(maxx), m_maxy(maxy)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline BBox2D::BBox2D(const BBox2D& box)
: m_minx(box.m_minx), m_miny(box.m_miny),
m_maxx(box.m_maxx), m_maxy(box.m_maxy)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline BBox2D::~BBox2D()
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline BBox2D& BBox2D::operator = (const BBox2D& box)
{
	if (this == &box)
		return *this;

	m_minx = box.m_minx; m_miny = box.m_miny;
	m_maxx = box.m_maxx; m_maxy = box.m_maxy;
	return *this;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline bool BBox2D::operator == (const BBox2D& box)
{
	return (m_minx == box.m_minx)
	&& (m_miny == box.m_miny)
	&& (m_maxx == box.m_maxx)
	&& (m_maxy == box.m_maxy);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void BBox2D::Invalidate()
{
	m_minx = m_miny = 1;
	m_maxx = m_maxy = -1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void BBox2D::Shift(double tx, double ty)
{
	m_minx += tx; m_miny += ty;
	m_maxx += tx; m_maxy += ty;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void BBox2D::Inflate(double d)
{
	m_minx -= d; m_miny -= d;
	m_maxx += d; m_maxy += d;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline double BBox2D::Lowx() const
{
	return m_minx;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline double BBox2D::Lowy() const
{
	return m_miny;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline double BBox2D::Highx() const
{
	return m_maxx;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline double BBox2D::Highy() const
{
	return m_maxy;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline double BBox2D::Width() const
{
	return m_maxx - m_minx;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline double BBox2D::Height() const
{
	return m_maxy - m_miny;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline Vec2 BBox2D::Center() const
{
	return Vec2((m_minx + m_maxx)/2, (m_miny + m_maxy)/2);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline bool BBox2D::IsValid() const
{
	if ( m_minx > m_maxx ) return false;
	if ( m_miny > m_maxy ) return false;
	return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline bool BBox2D::IsInside(double x, double y) const
{
	if ( x < m_minx ) return false;
	if ( y < m_miny ) return false;
	if ( x > m_maxx ) return false;
	if ( y > m_maxy ) return false;
	return true;
}
//-----------------------------------------------------------------------------

#endif /* BBOX2D_H_ */
