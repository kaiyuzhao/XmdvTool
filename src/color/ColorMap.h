/**
 * The ColorMap class is an interface for ColorRamp. Additionally,
 * it's stored ColorRamp can change, so you can change the underlying
 * color of the ColorMap and not have to change your code to switch to
 * a new map every time you change colors.
 *
 * The ColorMap also stores an integer indicating a link number. This
 * number is used for when you want colors linked together; that is,
 * when you change one color, the others change too. This link number
 * indicates the location of the color in the link. For instance, if you
 * have 5 colors linked together, you will have link positions 0-4, 0 will
 * retrieve the 1st color in the link, and 4 will retrieve the 5th color.
 *
 * Lastly, the ColorMap stores an integer for allowed type. This means what
 * ColorRamps you are able to switch to. In ColorRamp.h there is a binary
 * exponential enumeration allowing you to binary OR values together to
 * generate any combination of allowed ramp types (except links, which have
 * to be only link or no link at all).
 *
 * For examples on creating ColorMaps, see ColorManager
 */

#ifndef COLORMAP_H
#define COLORMAP_H

#include "datatype/RGBt.h"
#include "color/ColorRamp.h"

#include <QPixmap>

// Max length
const int CMAPSIZE	= 256;

class Image;

class ColorMap
{
public:
	// Constructor
	// \param ramp The ColorRamp to associate with
	// \param link If this ColorMap is part of a link (otherwise leave it 0), what
	//        position in the link is it (0 is first color, 1 is second color, etc)
	// \param allowedType What type of ColorRamps this ColorMap is allowed to have.
	//        You should binary OR values together from ColorRamp::RampType, unless
	//        it is RAMP_LINK, in which case leave it 0. Also, leaving it as 0 will
	//        set the allowedType equal to whatever type the ramp is.
	ColorMap(ColorRamp *ramp, int link = 0, int allowedType = 0);

	// Copy Constructor (creates a new ColorMap)
	// \param from The ColorMap to copy from
	ColorMap(const ColorMap &from);

	// Destructor
	virtual		~ColorMap();

	// Overloaded equals operator (replaces old data, does not copy)
	// \param from the ColorMap to copy from
	void		operator =(const ColorMap & from);

	// Overloaded [] operator, retrieves color from the ramp
	// \param index the index to retrives the color at, from 0 to CMAPSIZE
	// \return The complete ARGB value of the color at that index
	unsigned long	operator[](int index);

	// Retrieves the color from the ramp at a given index
	// \param index The location in the ramp to get the color, from 0.0f to 1.0f
	// \return The complete ARGB value of the color at that index
	unsigned long	getColor(float index = 0.0f);


	//
	// Draws the color map as a rectangle
	// NOTE: x1/y1 is the top left position, x2/y2 is the top right position
	//
	// \param x1 x position of the initial point
	// \param y1 y position of the initial point
	// \param x2 x position of the final point
	// \param y2 y position of the final point
	// \param width width of the drawn color map
	//
	void		drawColorBar(float x1 = 0.0f, float y1 = 0.0f,
			float x2 = 1.0f, float y2 = 0.0f,
			float width = 5.0f);
	QPixmap		drawColorBar2(int w, int h);

	// Initialize the texture for drawing
	void		initTex();

	// Get the ramp that's being currently used for the map
	// \return the ramp
	ColorRamp	*getColorRamp();

	// Set the ramp(curve) to be used for the map
	// \param ramp color ramp to be used for the map
	void		setColorRamp(ColorRamp *ramp);

	// Sets the number of bins in the ColorRamp
	// \param bins The number of bins, min. 2
	void		setNumBins(int bins);
	// Sets the RampMode in the ColorRamp
	// \param mode The mode to change to
	void		setRampMode(ColorRamp::RampMode mode);
	// Switches CONTINUOUS->BINS and vice/versa
	void		swapRampMode();
	// Retrieves m_allowedType
	int			getAllowedType();
	// Retrieves the ID from the ramp
	int			getRampID();
	// Retrieves m_len
	int			getsize();
	// Retrieves the LinkType from the ramp
	ColorRamp::LinkType getLinkType();

private:	// members
	ColorRamp	*m_colorramp;	// The color ramp

	Image		*m_checkImg;	// Checkered texture for drawing
	int			m_allowedType;	// The allowed ramp types
	int			m_len;			// The length of the map (generally CMAPSIZE)
	int			m_linkNum;		// The location of this map when it is part of a link

private:
	// Gets the color for a specified bin number
	// Could make this public if necessary
	// \param index The bin number to get the color of
	// \return The complete ARGB value of the color at that index
	unsigned long	getBinColor(int index);
	// Gets the color for a ColorBar, which means that it ignores the restrictions from links
	// \param index The location in the ramp to get the color, from 0.0f to 1.0f
	// \return The complete ARGB value of the color at that index
	unsigned long	getColorForColorBar(float index);
};

//-------------------------------------------------------------------------
inline ColorRamp * ColorMap::getColorRamp()
{
	return m_colorramp;
}

inline void ColorMap::setNumBins(int bins) {
	m_colorramp->setNumBins(bins);
}

inline void ColorMap::setRampMode(ColorRamp::RampMode mode) {
	m_colorramp->setRampMode(mode);
}

inline void ColorMap::swapRampMode() {
	m_colorramp->swapRampMode();
}

inline int ColorMap::getAllowedType() {
	return m_allowedType;
}

inline int ColorMap::getRampID() {
	return m_colorramp->getID();
}

inline int ColorMap::getsize() {
	return m_len;
}

inline ColorRamp::LinkType ColorMap::getLinkType() {
	return m_colorramp->getLinkType();
}

#endif
