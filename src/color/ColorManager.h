/*
 * ColorManager.h
 *
 *  Created on: Jun 18, 2009
 *  Author: Jason Stasik
 *
 *  The ColorManager is a singleton class that exists in g_globals
 *  and controls all of the color in Xmdv.
 *
 *  All colors used in the system should be stored in this class,
 *  even if you don't want the user to be able to change it.
 *
 *  Getting a stored color is as simple as calling the getColor() function.
 *
 *  The following sections are instructions for adding new colors and types.
 *
 *	I:
 *  In order to ADD A NEW RAMP to the system, follow these steps:
 *	1.	Add a new enumeration value to PredefinedColorRamps, and make sure
 *		that all the SEQ are together, all the DIV are together, etc.
 *	2.	Update the appropriate value in the ColorManager constructor
 *		that indicates the total number of ramps for that type
 *	3.	Give the ramp a name in the createNames() method in ColorManager
 *	4.	Add a case statement to the getPredefinedColorRamp method.
 *		You can use the template above the method declaration.
 *	In the future it might be wise to have the ramps read from a file and
 *	stored in the class rather than creating a new one every time the method
 *	is called. This would allow users to specify their own ramps.
 *
 *	II:
 *	In order to ADD A NEW COLOR to the system, follow these steps:
 *	1.	Add a new enumaration value to Colors. Try to keep them in pairs,
 *		as this will be the order in which they are displayed to the user.
 *		If it is a color that the user cannot change, add them specifically
 *		to the uneditable colors section.
 *	1a.	If the color is uneditable, update the variable m_uneditableColors
 *		in the ColorManager constructor.
 *	2.	Add the color in the initColor() method. See examples below for defining colors.
 *	2a.	Add the color in each section of the theme() method with the color
 *		you want it to have in each theme, unless the color is constant all the time
 *		and is not editable by the user.
 *	3.	Give the color a name in createNames() in ColorManager
 *
 *	III:
 *	In order to ADD A NEW RAMP TYPE:
 *	1.	Add the name of the ramp to PredefinedTypes, this corresponds with
 *		the prefixes in PredefinedColorRamps
 *	2.	Add a new setNumColorRamps call in the ColorManager constructor
 *	3.	Update the enum RampType in ColorRamp.h
 *	4.	Add a new ARR of QLabel* to ColorSelector.h for the new ramp type
 *	5.	Add a new QGroupBox in ColorSelector.cpp (~line 50)
 *	6.	In the ColorSelector constructor, copy the entire if statement where it
 *		says: if(m_color->getAllowedType() & ColorRamp::RAMP_SEQ)
 *		Paste this below (preferably in the appropriate order, for
 *		organizational reason). Replace the variables with the new variables
 *		you just created (ie ColorManager::SEQUENTIAL goes to
 *		ColorManager::YOURRAMPTYPE).
 *	7.	In ColorSelector::drawColorBars(), copy the sequential code,
 *		paste it and update the variables like in Step 6.
 *
 *	IV:
 *	In order to ADD A NEW LINK TYPE:
 *	1.	Add the name of the link to PredefinedTypes
 *	2.	Add a new setNumColorRamps call in the ColorManager constructor
 *	3.	Add the link to the enum LinkType in ColorRamp.h
 *	4.	Add an additional if statement in the section in ColorSelector.cpp:
 *		if(m_color->getAllowedType() == ColorRamp::RAMP_LINK)
 *		like this:
 *		if(m_color->getLinkType() == ColorRamp::YOUR_LINK) colorType = ColorManager::YOUR_LINK;
 *	5.	Do the same thing in the drawColorBars() method in ColorSelector.cpp
 *
 *	V:
 *	In order to ADD A NEW THEME:
 *	1.	Add the name of the theme to the enum Themes
 *	2.	Add the colors to the switch/case in the themes() method
 *	3.	Give the theme a name in createNames()
 *
 *	VI:
 *	In order to ADD A NEW COLOR STRATEGY
 *	1.	Add the strategy to the enum Strategies
 *	2.	Add additional interface components to the ColorStrategy class
 *	3.	Add an extra case statement in OkcVisualMap::setBrush
 *
 *	Examples for creating colors:
 *
 *	I: Linked colors
 *	ColorRamp *colorramp = getPredefinedColorRamp(LINK_REL5_RD);
 * 	m_colorMaps[TESTA] = new ColorMap(colorramp, 0);
 *	m_colorMaps[TESTB] = new ColorMap(colorramp, 1);
 *	m_colorMaps[TESTC] = new ColorMap(colorramp, 2);
 *	m_colorMaps[TESTD] = new ColorMap(colorramp, 3);
 *	m_colorMaps[TESTE] = new ColorMap(colorramp, 4);
 *
 *	II: Single Colors
 *	m_colorMaps[TEST] = new ColorMap(new ColorRamp(0xFFFFFF));
 *
 *	III: Predefined Ramps
 *	m_colorMaps[TEST] = new ColorMap(getPredefinedColorRamp(SEQ_NOYELLOW));
 *
 *	IV: Multiple Allowed Types
 *	m_colorMaps[TEST] = new ColorMap(getPredefinedColorRamp(SEQ_NOYELLOW), 0, ColorRamp::RAMP_SINGLE | ColorRamp::RAMP_SEQ | ColorRamp::RAMP_DIV | ColorRamp::RAMP_QUAL);
 */

#ifndef COLORMANAGER_H_
#define COLORMANAGER_H_

#include <vector>

class ColorRamp;
class ColorMap;

class ColorManager {
public:
	// A list of all the color ramps
	// Many taken from ColorBrewer.org
	enum PredefinedColorRamps {
		SEQ_ORANGES,
		SEQ_GRAYSCALE,
		SEQ_YLORRD,
		SEQ_YLGNBU,
		SEQ_PURD,
		SEQ_NOYELLOW,
		SEQ_HSIFULL,
		DIV_RDBU,
		DIV_PUOR,
		DIV_RDYLGN,
		DIV_SPECTRAL,
		DIV_RDGY,
		DIV_PUGN,
		QUAL_SET1,
		QUAL_DARK,
		QUAL_PASTEL,
		QUAL_SET2,
		QUAL_ACCENTS,
		LINK_REL2_RD,
		LINK_REL2_GN,
		LINK_REL2_BU,
		LINK_CON2_BLWH,
		LINK_CON2_RDGN,
		LINK_REL5_RD,
		LINK_REL5_GN,
		TOTAL_RAMPS,
	};

	// A list of all the colors used in the system
	enum Colors {
		// Editable colors
		BG, TEXT,
		BRUSH_BG1, BRUSH_SELECTED1,
		BRUSH_UNSELECTED, GRID1,
		GRID2, GRID3,
		// Uneditable colors
		PIXEL_HIGHLIGHT, MARKER_UNSELECTED,
		MARKER_SELECTED,
		TOTAL_COLORS,
	};

	// A list of all the types of color ramps
	// Make sure SEQUENTIAL is always first
	enum PredefinedTypes {
		SEQUENTIAL,
		DIVERGING,
		QUALITATIVE,
		LINK_REL2,
		LINK_CON2,
		LINK_REL5,
		TOTAL_TYPES,
	};

	// Color themes
	enum Themes {
		DEFAULT,
		NOCTURNAL,
		TOTAL_THEMES,
	};

	// Color Strategies
	enum Strategies {
		NONE,
		ORDER,
		VALUE,
		//STRUCTURE,
	};

	// Constructor
	ColorManager();
	// Destructor
	virtual ~ColorManager();

	// Deletes the ColorMaps stores in m_colorMaps
	void deleteColors();

	// Retrieves a new instance of a predefined color ramp
	// \param mapName See enum PredefinedColorRamps
	// \return A new ColorRamp object
	ColorRamp*			getPredefinedColorRamp(int mapName);
	// Retrives the name of a color ramp
	// \param mapName See enum PredefinedColorRamps
	// \return The name of the ramp
	char*				getPredefinedColorRampName(int mapName);

	// Retrieves the color value of a ColorMap at a given index
	// \param colorName See enum Colors
	// \param index the normalized color value to get, from 0.0f-1.0f
	// \return The complete ARGB value of the color at that index
	unsigned long		getColor(int colorName, float index = 0.0f);
	// Retrieves an instance of color, mainly used for updating colors in ColorCustom
	// \param colorName See enum Colors
	// \return A pointer to the ColorMap stored for that name, not a copy
	ColorMap*			getColorMap(int colorName);
	// Retrieves the name of a color
	// \param colorName See enum Colors
	// \return The name of the color
	char*				getColorName(int colorName);

	// Sets a color theme
	// \param theme The enum Themes to set
	void				theme(int theme);
	// Gets a theme name
	// \param theme The enum Themes name to get
	// \return The theme's name
	char*				getThemeName(int theme);

	// Retrieves the total number of ramps for a given ramp type
	// \param index See enum PredefinedTypes
	// \return The number of ramps of type index
	int					getPredefinedTotal(int index);
	// Retrieves the location of the first ramp of a given ramp type
	// \param index See enum PredefinedTypes
	// \return The starting location of the ramps of type index
	int					getPredefinedStart(int index);
	// Retrieves the location of the last ramp of a given ramp type
	// \param index See enum PredefinedTypes
	// \return The ending location of the ramps of type index
	int					getPredefinedEnd(int index);

	// Retrieves the current color strategy
	// \return The corresponding enum Strategies
	int					getStrategy();
	// Sets m_strat to a new color strategy
	// \param strat The corresponding enum Strategies
	void				setStrategy(int strat);

	// The total number of non-editable colors
	int					m_uneditableColors;

private:
	// The names for each predefined color ramp
	std::vector<char*>			m_predefinedColorRampNames;

	// The list of all colors used in the system
	std::vector<ColorMap*>		m_colorMaps;
	// The names for each color used in the system
	std::vector<char*>			m_colorNames;

	// The number of ramps for each PredefinedTypes
	std::vector<int>			m_predefinedNumColorMaps;

	// The names of each theme
	std::vector<char*>			m_themeNames;

	// The current color strategy
	int					m_strat;

	// Give colors names
	void				createNames();
	// Updates m_predefinedNumColorMaps
	void				setNumColorRamps(PredefinedTypes type, int num);
	// Initialize colors on startup
	void				initColors();
};

#endif /* COLORMANAGER_H_ */
