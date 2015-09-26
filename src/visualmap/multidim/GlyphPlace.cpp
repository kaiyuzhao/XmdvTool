/*
 * GlyphPlace.cpp
 *
 *  Created on: Apr 22, 2009
 *      Author: Zaixian xie
 */

#include "visualmap/multidim/GlyphPlace.h"
#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierPCADerive.h"
#include "visualmap/multidim/GlyphPlaceResult.h"

GlyphPlace::GlyphPlace() {
	m_placeResult = 0;
}

GlyphPlace::~GlyphPlace() {
	SAFE_DELETE(m_placeResult);
}

void GlyphPlace::setOkcData(OkcData* okcdata) {
	m_okcData = okcdata;
}

OkcData* GlyphPlace::getOkcData(){
	return m_okcData;
}


void GlyphPlace::setPlaceResult(GlyphPlaceResult* placeResult) {
	m_placeResult = placeResult;
}

GlyphPlaceResult* GlyphPlace::getPlaceResult() {
	return m_placeResult;
}

void GlyphPlace::setGlyphPlaceMode(XmdvTool::GLYPHPLACE_MODE mode) {
	m_placeMode = mode;
}

XmdvTool::GLYPHPLACE_MODE GlyphPlace::getGlyphPlaceMode() {
	return m_placeMode;
}

void GlyphPlace::setDimX(XmdvTool::OKCDIM dimX) {
	m_dimX = dimX;
}

void GlyphPlace::setDimXNo(int dimXNo) {
	m_dimXNo = dimXNo;
}

void GlyphPlace::setDimY(XmdvTool::OKCDIM dimY) {
	m_dimY = dimY;
}

void GlyphPlace::setDimYNo(int dimYNo) {
	m_dimYNo = dimYNo;
}

void GlyphPlace::placeGlyph() {
	SAFE_DELETE(m_placeResult);
	m_placeResult = new GlyphPlaceResult();

	OkcData* input = getOkcData();

	int numRows = input->getDataSize();
	// Set the glyph size
	m_placeResult->blocksize = 1.0/(1.0 + sqrt((double)numRows));
	m_placeResult->blockcnt = int( .5 + sqrt((double)numRows) );
	m_placeResult->halfblocksize = .5 * m_placeResult->blocksize;
    // Initialize the array basex and basey
	m_placeResult->basex.resize(numRows);
	m_placeResult->basey.resize(numRows);

    switch (m_placeMode) {
	case XmdvTool::GLYPHPLACE_ORIGINAL :
	case XmdvTool::GLYPHPLACE_ORDERED :
		// Since the data has been sorted by SortOperator
		// when the place mode is GLYPHPLACE_ORDERED,
		// so we still call placement_Original_Glyph();
		placement_Original_Glyph();
		break;
	case XmdvTool::GLYPHPLACE_DATA_DRIVEN :
		placement_DataDriven_Glyph();
		break;
	case XmdvTool::GLYPHPLACE_DERIVED :
		Placement_Derived_Glyph();
		break;
	}

}

void GlyphPlace::placement_Original_Glyph()
{
	GlyphPlaceResult* result = getPlaceResult();
	OkcData* input = getOkcData();

	int i, numRows = input->getDataSize();;
	for ( i = 0; i < numRows; i++ )
	{
		result->basex[i] = result->blocksize * (i%result->blockcnt) + result->halfblocksize;
		result->basey[i] = result->blocksize * (i/result->blockcnt) + result->halfblocksize;
	}

}


void GlyphPlace::placement_DataDriven_Glyph()
{
	GlyphPlaceResult* result = getPlaceResult();
	OkcData* input = getOkcData();

	// Get the dimension max and min values
	std::vector<double> dim_min, dim_max;
	input->getDimMinArr(dim_min);
	input->getDimMaxArr(dim_max);
	// Prepare parameters for computing positions
	double xmin = dim_min[m_dimXNo];
	double ymin = dim_min[m_dimYNo];
	double xpara = 1.0/(dim_max[m_dimXNo] - xmin);
	double ypara = 1.0/(dim_max[m_dimYNo] - ymin);

	int i, numRows = input->getDataSize();
	std::vector<double> buf;
	for (i = 0; i < numRows; i++ )
	{
		input->getData(buf, i);
		result->basex[i] = (buf[m_dimXNo] - xmin) * xpara;
		result->basey[i] = 1-(buf[m_dimYNo] - ymin) * ypara;
	}
}

void GlyphPlace::Placement_Derived_Glyph() {
	GlyphPlaceResult* result = getPlaceResult();
	OkcData* input = getOkcData();
	// Get the modifier containing pca derived pricipal components
	OkcDataModifierPCADerive* mod = dynamic_cast<OkcDataModifierPCADerive*>
	(input->getOkcDataModifierManager()->getOkcDataModifier(XmdvTool::MODIFIER_PCADERIVE));
	// Get the dimension max and min values
	std::vector<double> dim_min, dim_max;
	input->getDimMinArr(dim_min);
	input->getDimMaxArr(dim_max);

	double xmin=0.0, ymin=0.0, xpara=0.0, ypara=0.0;
	// Calculate the xmin and xpara to prepare the computation
	// of horizontal position of glyphs.
	if ( m_dimX==XmdvTool::OKCDIM_ORIGINAL) {
		// Get data from data buffer
		xmin = dim_min[m_dimXNo];
		xpara = 1.0/(dim_max[m_dimXNo] - xmin);
	} else if (m_dimX==XmdvTool::OKCDIM_PCADERIVE) {
		// Get data from principal components to compute the glyph position
		xmin = mod->principal_min[m_dimXNo];
		xpara = 1.0/(mod->principal_max[m_dimXNo] - xmin);
	}
	// Calculate the ymin and ypara to prepare the computation
	// of vertical position of glyphs.
	if ( m_dimY==XmdvTool::OKCDIM_ORIGINAL) {
		// Get data from data buffer
		ymin = dim_min[m_dimYNo];
		ypara = 1.0/(dim_max[m_dimYNo] - ymin);
	} else if (m_dimY==XmdvTool::OKCDIM_PCADERIVE) {
		// Get data from principal components to compute the glyph position
		ymin = mod->principal_min[m_dimYNo];
		ypara = 1.0/(mod->principal_max[m_dimYNo] - ymin);
	}

	int i, numRows = input->getDataSize();
	std::vector<double> buf;
	for ( i = 0; i < numRows; i++ )
	{
		input->getData(buf, i);
		// calculate the horizontal position of glyphs
		if ( m_dimX==XmdvTool::OKCDIM_ORIGINAL) {
			result->basex[i] = (buf[m_dimXNo] - xmin) * xpara;
		} else if (m_dimX==XmdvTool::OKCDIM_PCADERIVE) {
			result->basex[i] = (mod->principal[m_dimXNo][i] - xmin) * xpara;
		}
		// calculate the vertical position of glyphs
		if ( m_dimY==XmdvTool::OKCDIM_ORIGINAL) {
			result->basey[i] = 1.0 - (buf[m_dimYNo] - ymin) * ypara;
		} else if (m_dimX==XmdvTool::OKCDIM_PCADERIVE) {
			result->basey[i] = 1.0 - (mod->principal[m_dimYNo][i] - ymin) * ypara;
		}
	}
}
