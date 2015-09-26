/*
 * OkcVisualMapResult.h
 *
 *  Created on: Jan 13, 2009
 *      Author: Zaixian Xie
 */

/*
 * class OkcVisualMapResult represents the visual attributes
 * for an OkcData.  It is the output of class OkcVisualMap.
 */

#ifndef OKCVISUALMAPRESULT_H_
#define OKCVISUALMAPRESULT_H_

#include "main/XmdvTool.h"
#include <vector>
#include "visualmap/multidim/DiagVisAttr.h"
#include "visualmap/VisualMapResult.h"


class VisualAttribute;
class Data;
class Brush;
class BrushOperator;
class GlyphPlaceResult;
class HierDisplayInformation;
class DiagVisAttr;
class ScatterVisAttr;

class OkcVisualMapResult: public VisualMapResult {
	friend class OkcVisualMap;
public:
	OkcVisualMapResult();
	OkcVisualMapResult(Data* input);
	virtual ~OkcVisualMapResult();

private:
	//Initialize visual attribute constants associated with brush
	// and m_brushOperator
	void initBrush();
	//Initialize the glyph placement result
	void initGlyphPlaceResult();
	//Initialize parameters for Hierarchical display
	void initHierParameters();

private:
	// If the last operator of pipeline is BrushOperator,
	// the pointer to this operator will be stored
	BrushOperator* m_brushOperator;
	// This is the brush converted from BrushStorage
	Brush* m_brush;
	std::vector<VisualAttribute*> data_VisAttr;
	VisualAttribute *inBrushVisAttr;
	VisualAttribute *outBrushVisAttr;

	//std::vector<std::vector<VisualAttribute*>> diag_VisAttr;
	DiagVisAttr* diag_VisAttr;

	// The information about glyph placement
	GlyphPlaceResult* m_glyphPlaceResult;
	// the glyph shape (it makes sense only for glyphs)
	XmdvTool::GLYPHSHAPE m_glyphShape;

	ScatterVisAttr* m_scatterVisAttr;

	// The cardinality (the number of bins for each dimension)
	// in the views. This might be different from the original dataset
	// because of dimension on/off/reordering
	std::vector<int> m_cardinality;

private:
	// Is this a visual map result of SBB pipeline
	bool m_bIsSBBResult;
	// The hierarchical display information
	HierDisplayInformation* m_hier_info;
	/*
	 * The functions to get information about data
	 */
public:
	// get the number of rows for the tabular data to be visualized
	int getDataSize();
	// get the number of dimensions for the tabular data to be visualized
	int getDimSize();
	// get the data item in a particular line
	// This is not efficient because of type check
	// If this function will be called multiple times,
	// the developer should first check the type of data
	// and then call OkcData->getData()
	// or DerivedOkcData->getData()
	void getDataItem(std::vector<double> &buf, int line);

	// get the dimension names
    std::vector<char*> getDimNameArr();
    // get the minimum values for dimensions
    std::vector<double> getDimMinArr();
    // get the maximum values for dimensions
    std::vector<double> getDimMaxArr();
    //get the cardinality for dimensions
    // std::vector<int> getDimCardinalityArr();

public:
	void setGlyphPlaceResult(GlyphPlaceResult* placeResult);
	GlyphPlaceResult* getGlyphPlaceResult();

	// set the glyph shape
	void setGlyphShape(XmdvTool::GLYPHSHAPE shape);
	// get the glyph shape
	XmdvTool::GLYPHSHAPE getGlyphShape();

	// Set the brushOperator pointer to the last operator
	// in the pipeline if it is.
	void setBrushOperator(BrushOperator* brushOperator);
	// Get the object of BrushOperator
	BrushOperator* getBrushOperator();
	// set the brush object
	void setBrush(Brush* brush);
	// get the object of Brush
	Brush * getBrush();

	std::vector<VisualAttribute*> getDataVisAttr();
	void setDataVisAttr(std::vector<VisualAttribute*> data_VisAttr);
	VisualAttribute* getInBrushVisAttr();
	void setInBrushVisAttr(VisualAttribute* inBrushVisAttr);
	VisualAttribute* getOutBrushVisAttr();
	void setOutBrushVisAttr(VisualAttribute* outBrushVisAttr);

	void setDiagMultiDimDataVisAttr(VisualAttribute* VisAttr, int dim, int pos);
	VisualAttribute* getDiagMultiDimDataVisAttr(int dim, int pos);

	void setSingleDataVisAttr(VisualAttribute* VisAttr, int pos);
	VisualAttribute* getSingleDataVisAttr(int pos);

	void initDiagVisAttr() ;
	void setDiagVisAttr(DiagVisAttr* _diag_VisAttr);

	void setSBBResult(bool isSBB);
	bool isSBBResult();

	void setHierInfo(HierDisplayInformation* hier_info);
	HierDisplayInformation* getHierInfo();


	ScatterVisAttr* getScatterVisAttr();
	void setScatterVisAttr(ScatterVisAttr* scatterVisAttr);

	void setCardinality(std::vector<int> card);
	std::vector<int> getCardinality();
};

#endif /* OKCVISUALMAPRESULT_H_ */
