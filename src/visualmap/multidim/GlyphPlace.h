/*
 * GlyphPlace.h
 *
 *  Created on: Apr 22, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class GlyphPlace calculates the position and size for glyphs,
 * and saves the computation result to class GlyphPlaceResult
 */
#ifndef GLYPHPLACE_H_
#define GLYPHPLACE_H_
#include "main/XmdvTool.h"

class OkcData;
class GlyphPlaceResult;

class GlyphPlace {
public:
	GlyphPlace();
	GlyphPlace(OkcData* okcData);
	virtual ~GlyphPlace();

private:
	OkcData* m_okcData;
	GlyphPlaceResult* m_placeResult;
	XmdvTool::GLYPHPLACE_MODE m_placeMode;

	// The dimension used to decide glyph position on X
	XmdvTool::OKCDIM m_dimX;
	// If m_dimX=OKCDIM_ORIGINAL, we will use m_dimXNo to denote
	// which dimension we will use, otherwise, m_dimXNo does not make sense.
	int m_dimXNo;

	// The dimension used to decide glyph position on Y
	XmdvTool::OKCDIM m_dimY;
	// If m_dimY=OKCDIM_ORIGINAL, we will use m_dimYNo to denote
	// which dimension we will use, otherwise, m_dimYNo does not make sense.
	int m_dimYNo;

private:
	// place the glyphs in the form of a matrix close to a square one
	void placement_Original_Glyph();
	// Use two dimensions to decide the position of glyphs
	void placement_DataDriven_Glyph();
	// Use dimensions or pricipal components to decide the position of glyphs
	void Placement_Derived_Glyph();

public:
	void setOkcData(OkcData* okcdata);
	OkcData* getOkcData();
	void setPlaceResult(GlyphPlaceResult* placeResult);
	GlyphPlaceResult* getPlaceResult();

	void setGlyphPlaceMode(XmdvTool::GLYPHPLACE_MODE mode);
	XmdvTool::GLYPHPLACE_MODE getGlyphPlaceMode();

	// set the dimension used to decide glyph position
	void setDimX(XmdvTool::OKCDIM dimX);
	void setDimXNo(int dimXNo);
	void setDimY(XmdvTool::OKCDIM dimY);
	void setDimYNo(int dimYNo);

	// Compute the glyph position and size
	void placeGlyph();

};

#endif /* GLYPHPLACE_H_ */
