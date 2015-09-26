/**
 * The ColorRamp class stores two arrays of floats indicating
 * the colors along the ramp. The first array, m_tabcmpnt, is
 * for continuous color ramps, and the second, m_tabcmpntbins,
 * is for a binned color ramp. Both are stored in a single color
 * ramp to allow users to easily switch between the two.
 *
 * The first special type of color ramp is RAMP_SINGLE. This "ramp"
 * is merely the same color perpetuated throughout the entire ramp.
 *
 * Next there is the standard sequential, diverging, and qualitative
 * ramps. The continuous components for these 3 ramps are all handled
 * in the same way. However, the bins for the qualitative ramps
 * are calculated differently than the other two.
 * For sequential and diverging, bins are calculated by sampling the
 * continuous array in intervals of m_maxndx/m_numBins. The halfway
 * point between each interval is used for the color, then the entire
 * interval is filled with that color.
 * For qualitative ramps, it grabs the colors directly at intervals of
 * m_maxndx/(m_origBins-1). So, say you specify a ramp that's red at 0.0f,
 * green at 0.5f, and blue at 1.0f. If you have two bins, 0.0f-0.5f will
 * be pure red and 0.5f-1.0f will be pure green. If you then switch to three
 * bins, 0.0f-0.33f will be pure red and 0.33f-0.66f will be pure green still,
 * and now 0.66f-1.0f will be pure blue, rather than doing any kind of sampling.
 *
 * For a much simpler explanation, load up the program and actually look at what
 * happens when you change the number of bins in the color selector options.
 *
 * A special note for qualitative ramps: They are automatically set to BINS when
 * created and although it is possible to switch them back to continuous in the code,
 * don't do that without a really good reason. This is functionality restricted by
 * the GUI rather than in the code, just in case you need it for some reason.
 *
 * The second special type of color ramp is RAMP_LINK. A link indicates that
 * two or more colors are "linked" together, and if one changes, so does the other.
 * A linked ramp is really just a specialized qualitative ramp; as in, you have several
 * discrete boundaries between colors. Because each color in a link maps to a specific
 * bin, users cannot change the number of bins in a linked ramp. This is also only
 * restricted by the GUI.
 *
 * For examples of creating ColorRamps, see the ColorManager class
 */

#ifndef __COLORRAMP_H__
#define __COLORRAMP_H__

#include <vector>

#include "datatype/RGBt.h"

class ColorRamp
{
public:
	// Interpolation function
	// I think only LINEAR is used right now
	enum RampFunction {
		CONSTANT = 0,
		LINEAR = 1,
		POLYNOMIAL = 2,
	};

	// The type of ramp, linked, single color, sequential, diverging, qualitative
	enum RampType {
		RAMP_LINK = 0,
		RAMP_SINGLE = 1,
		RAMP_SEQ = 2,
		RAMP_DIV = 4,
		RAMP_QUAL = 8,
	};

	// The link type, LINK_NONE is the default value for non-linked RampTypes
	enum LinkType {
		LINK_NONE,
		LINK_REL2,
		LINK_CON2,
		LINK_REL5,
	};

	// Whether the color retrieved should be from the bins array or the normal array
	enum RampMode {
		CONTINUOUS,
		BINS,
	};

	// RGB, HSI, HSV, etc. feel free to add more, see ColorAssign.h
	enum ColorModel {
		MODEL_RGB,
		MODEL_HSI,
	};

	// Stores a color at a location in the ramp, used for interpolation
	struct RampPoint
	{
		struct Point {
			float x, y;
		}			point;
		RampFunction mode;
		struct Tangent {
			float x0, y0, x1, y1;
		}			tangent;
		float		polynom[4];
	};

	typedef std::vector<RampPoint*>	PointList;
	typedef PointList::iterator		PointListIter;

public:
	// Single-Color Constructor
	// \param color The ARGB or RGB value to assign to the ramp
	ColorRamp(unsigned int color = 0x000000);

	// Constructor - initialize with a predefined set of ramp points
	// \param N number of ramp points
	// \param list array of ramp points
	// \param type The type of ramp
	// \param model RGB, HSI, etc.
	// \param linktype The type of link, only change if type == RAMP_LINK
	ColorRamp(unsigned int N, float list[][5], int id, RampType type, ColorModel model = MODEL_RGB, LinkType linktype = LINK_NONE);

	// Copy Constructor
	// \param ColorRamp& copy this
	ColorRamp(const ColorRamp&);

	// Destructor
	~ColorRamp();

	// Overloaded equals operator
	// \param ColorRamp& The ramp to copy from
	void operator = (const ColorRamp&);

	// Initialize the ColorRamp using a list of points defining the ramp
	// \param N number of points
	// \param list array of points
	void initialize(unsigned int N, float list[][5]);

	// Evaluate the value of the ramp at a particular position and for
	// the specified component of the color
	// \param index position of point
	// \param cmpnt component of the color
	// \return float value at the position on the ramp
	float evaluate(int index, int cmpnt);

	PointListIter begin(int cmpnt);
	PointListIter end(int cmpnt);

	// Insert a point on the ramp
	// \param x X position
	// \param y Y position
	// \param ndx Index value
	// \param cmpnt Component of the color
	// \param point A pointer to the new ramp point
	// \return int
	int insertPoint(float x, float y,
			int ndx, int cmpnt, PointListIter &point);

	// Remove a point from the ramp
	// \param iter the pointer to the point to be removed
	// \param cmpnt from this component of the color
	void removePoint(PointListIter iter, int cmpnt);

	// Move a point on the ramp to any position
	// \param curPoint
	// \param curCmpnt
	// \param x
	// \param y
	// \return
	int movePoint(PointListIter curPoint, int curCmpnt,
			float x, float y);

	// Move the tangent of a particular point on the ramp
	// \param curPoint
	// \param curCmpnt
	// \param sens
	// \param x
	// \param y
	// \return int
	int moveTangent(PointListIter curPoint, int curCmpnt, int sens,
			float x, float y);

	// Update the lookup table
	// \param cmpnt Component of color
	// \param curr Point to be considered
	void update(int cmpnt, PointListIter curr);

	// Inverts the matrix
	void invertmat(float from[4][4], float to[4][4]);

	// Gets m_mode
	RampMode	getRampMode();
	// Sets m_mode to mode
	void		setRampMode(RampMode mode);
	// Changes CONTINUOUS to BINS and vice/versa
	void		swapRampMode();

	// Sets m_numBins to bins
	void		setNumBins(int bins);
	// Gets m_numBins
	int			getNumBins();

	// Gets m_type
	RampType	getRampType();
	// Gets m_origBins
	int			getOrigBins();

	// Gets m_model
	ColorModel	getColorModel();

	// Gets m_id
	int			getID();
	// Gets m_dimlut
	int			getsize();
	// Gets m_linkType
	LinkType	getLinkType();

private:	// members

	int		m_dimlut;		// size of the lookup table
	int		m_maxndx;		// max addressable index values in the table
	float	m_lutratio;		// maxndx / 255
	float	m_tanlen;		// length of the tangent points from the point
	float	*m_tabcmpnt;	// the lookup table
	float	*m_tabcmpntbins;// the lookup table for bins

	int		m_numBins;		// the current number of bins
	int		m_origBins;		// the original number of bins (for qualitative)

	int		m_id;			// the corresponding ColorManager::PredefinedColorRamps enum

	ColorModel	m_model;	// RGB, HSI, ...
	RampMode m_mode;		// CONTINUOUS or BINS
	RampType m_type;		// sequential, diverging, etc.
	LinkType m_linkType;	// if type == RAMP_LINK, the link type

	PointList m_userPoints[4]; // the four component valued points

private:

	// Retrieve the coefficients to make polynomial function
	// \param mode function to be used
	// \param a0,b0,t1
	// \param a1,b1,t1
	// \param coeff
	void getPolynomial(RampFunction mode,
			float a0, float b0, float t0,
			float a1, float b1, float t1,
			float coeff[4]);

	// Solve a polynomial function given by its coefficient values by
	// substituting the given x value
	// \param x value
	// \param *coeff coefficients in the polynomial
	// \return float evaluated value
	float solvePolynomial(float x, float *coeff);

	// Updates the values in m_tabcmpntbins when the ramp is created or
	// m_numBins is updated
	void updateBins();

};

inline ColorRamp::RampMode ColorRamp::getRampMode() {
	return m_mode;
}

inline void ColorRamp::setRampMode(RampMode mode) {
	m_mode = mode;
}

inline void ColorRamp::swapRampMode() {
	if(m_mode == BINS) m_mode = CONTINUOUS;
	else m_mode = BINS;
}

inline void ColorRamp::setNumBins(int bins) {
	m_numBins = bins;
	updateBins();
}

inline int ColorRamp::getNumBins() {
	return m_numBins;
}

inline ColorRamp::RampType ColorRamp::getRampType() {
	return m_type;
}

inline int ColorRamp::getOrigBins() {
	return m_origBins;
}

inline ColorRamp::ColorModel ColorRamp::getColorModel() {
	return m_model;
}

inline int ColorRamp::getID() {
	return m_id;
}

inline int ColorRamp::getsize() {
	return m_dimlut;
}

inline ColorRamp::LinkType ColorRamp::getLinkType() {
	return m_linkType;
}

#endif //__ColorRamp_H__
