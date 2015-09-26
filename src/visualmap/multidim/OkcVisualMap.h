/*
 * OkcVisualMap.h
 *
 *  Created on: Jan 12, 2009
 *      Author: Zaixian xie
 */

/*
 * This class does the visual map for OkcData
 */

#ifndef OKCVISUALMAP_H_
#define OKCVISUALMAP_H_

#include "visualmap/VisualMap.h"

class BrushOperator;
class OkcVisualMapResult;
class OkcData;
class GlyphPlace;
class ScatterVisAttr;


class OkcVisualMap: public VisualMap {
public:
	OkcVisualMap();
	virtual ~OkcVisualMap();

private:
	// The brush in m_brushOperator will be stored in the
	// visual mapping result, so that it can be drawn
	// by the view display.  Normally, if the last operator
	// in the operator list is brush operator, it will be set to here.
	// However, the future developers can use it to
	// set any brush to draw.
	BrushOperator* m_brushOperator;

	// The object to calculate glyph position and size
	GlyphPlace* m_glyphPlace;

	XmdvTool::GLYPHSHAPE m_glyphShape;

	ScatterVisAttr* m_scatterVisAttr;

public:
	// Verify the class type of input
	void setInput(Data* input);

	// set the current brush operator
	void setBrushOperator(BrushOperator* brushQ);
	// get the current brush operator
	BrushOperator* getBrushOperator();

	// set the current GlyphPlace
	void setGlyphPlace(GlyphPlace* glyphPlace);
	// get the current GlyphPlace
	GlyphPlace* getGlyphPlace();

	// set the glyph shape
	void setGlyphShape(XmdvTool::GLYPHSHAPE shape);
	// get the glyph shape
	XmdvTool::GLYPHSHAPE getGlyphShape();

	// Implement the virtual function to do visual mapping
	void doVisualMap();

	ScatterVisAttr* getScatterVisAttr();
	void setScatterVisAttr(ScatterVisAttr* scatterVisAttr);

private:
	// set the visual attributes in terms of highlighting attributes;
	// set the brush boundary
	void setBrush(OkcData* okcdata, OkcVisualMapResult* okcVMR);
	// Set the cardinality numbers in the VisualMapResult
	void setCardinality(OkcVisualMapResult* okcVMR);

	// Calculate the glyph position if the view is glyphs
	void placeGlyph(OkcData* okcdata, OkcVisualMapResult* okcVMR);
	// Set the color for each node in the SBB pipeline
	void setSBBColor(OkcData* okcdata, OkcVisualMapResult* okcVMR);

	// set different colors for one point based on different dimension
	// for scatterplot diagonal plot two dimension disp
	void setDiagPlotColor(OkcData* okcdata, OkcVisualMapResult* okcVMR);


};

#endif /* OKCVISUALMAP_H_ */
