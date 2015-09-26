/*
 * Subwindow.h
 *
 *  Created on: May 13, 2009
 *
 *  Used by VisPixel
 */

#ifndef SUBWINDOW_H_
#define SUBWINDOW_H_

#include "datatype/Vec2.h"
#include "datatype/BBox2D.h"
#include <vector>
#include "view/auxiliary/Image.h"

struct PixelInfo
{
	// Normalized values
	std::vector<double> norm_values;
	std::vector<int> indices;
	std::vector<double> randomNums;
	//double dist_min, dist_max;
	//double brush_level;
	std::vector<int> sizes;

	int data_size;
};

class Brush;
class HierDisplayInformation;

class Subwindow {
public:

	enum DisplayType {
		PDT_SPIRAL,
		PDT_HORIZONTAL,
		PDT_VERTICAL,
	};

	///
	/// Constructor
	///
	Subwindow(int dim);

	///
	/// Copy constructor
	///
	/// \param from the value to copy to this object
	///
	Subwindow(const Subwindow& from);

	///
	/// Destructor
	///
	virtual ~Subwindow();

	///
	/// Assignment operator
	///
	/// \param from the value to assign to this object
	/// \return reference to this object
	Subwindow& operator = (const Subwindow& from);

	void Paint(Image *img, const PixelInfo& flat_pixel_info,
			int dim_size,
			double startSample = 0, double endSample = 1.0,
			DisplayType type = PDT_SPIRAL, bool hilite = false);

	void PaintHier(Image *img, const PixelInfo& flat_pixel_info,
			int dim_size, HierDisplayInformation *hier,
			DisplayType type = PDT_SPIRAL, bool hilite = false);

	void sspiral(double startSample, double endSample, Image *rgb, int dim,
			const PixelInfo &pixel_info, int dim_size,
			DisplayType type, HierDisplayInformation *hier = 0);

	void SetWindow(float x, float y, float width, float height);

	/// ACCESS
	BBox2D& GetBoundingBox();
	float GetX() const;
	void  SetX(float val);
	float GetY() const;
	void  SetY(float val);
	float GetWidth() const;
	float GetHeight() const;
	Vec2 GetCenter() const;

	bool Contains(float x, float y);
	bool Contains(float x, float y, int &n, Subwindow::DisplayType displayType, int &rx, int &ry);

	int			m_dimension;
	BBox2D		m_box;
	int			m_size;
	long		m_lowColor;
	long		m_highColor;

};

// INLINE METHODS
//-----------------------------------------------------------------------------
inline BBox2D& Subwindow::GetBoundingBox()
{
	return m_box;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline Vec2 Subwindow::GetCenter() const
{
	return m_box.Center();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline float Subwindow::GetWidth() const
{
	return (float) m_box.Width();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline float Subwindow::GetHeight() const
{
	return (float) m_box.Height();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline float Subwindow::GetX() const
{
	return (float) m_box.Lowx();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void Subwindow::SetX(float x)
{
	m_box.Shift(x, 0);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline float Subwindow::GetY() const
{
	return (float) m_box.Lowy();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void Subwindow::SetY(float y)
{
	m_box.Shift(0, y);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline bool Subwindow::Contains(float x, float y)
{
	return m_box.IsInside(x, y);
}
//-----------------------------------------------------------------------------

inline void Subwindow::SetWindow(float x, float y, float width, float height)
{
	m_box = BBox2D(x, y, x+width, y+height);
}

#endif /* SUBWINDOW_H_ */
