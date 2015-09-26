/*
 * OkcVisualMap.cpp
 *
 *  Created on: Jan 12, 2009
 *      Author: Zaixian Xie
 */

#include "OkcVisualMap.h"
#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifierHighlight.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/storage/CardStorage.h"
#include "data/cluster/OkcDataModifierClusterColor.h"
#include "data/cluster/OkcDataModifierClusterHighlight.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Pipeline.h"
#include "pipeline/multidim/SBBPipeline.h"
#include "operator/brush/Brush.h"
#include "operator/brush/BrushOperator.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "visualmap/VisualAttribute.h"
#include "visualmap/VisualMapResult.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "visualmap/multidim/GlyphPlace.h"
#include "color/ColorManager.h"
#include "main/XmdvToolMainWnd.h"
#include "main/HandleTools.h"
#include "interface/color/ColorStrategyManager.h"
#include "visualmap/multidim/DiagVisAttr.h"
#include "visualmap/multidim/ScatterVisAttr.h"


#include <typeinfo>
using namespace std;

OkcVisualMap::OkcVisualMap() {
	m_brushOperator = 0;
}

OkcVisualMap::~OkcVisualMap() {
}

void OkcVisualMap::setInput(Data* input) {
	assert ( typeid(*input) == typeid(OkcData) );
	VisualMap::setInput(input);
}

void OkcVisualMap::setBrushOperator(BrushOperator* brushQ) {
	m_brushOperator = brushQ;
}

BrushOperator* OkcVisualMap::getBrushOperator() {
	return m_brushOperator;
}

void OkcVisualMap::setGlyphPlace(GlyphPlace* glyphPlace) {
	m_glyphPlace = glyphPlace;
}

GlyphPlace* OkcVisualMap::getGlyphPlace() {
	return m_glyphPlace;
}

void OkcVisualMap::setGlyphShape(XmdvTool::GLYPHSHAPE shape) {
	m_glyphShape = shape;
}



XmdvTool::GLYPHSHAPE OkcVisualMap::getGlyphShape() {
	return m_glyphShape;
}

ScatterVisAttr* OkcVisualMap::getScatterVisAttr() {
	return this->m_scatterVisAttr;
}

void OkcVisualMap::setScatterVisAttr(ScatterVisAttr* scatterVisAttr) {
	m_scatterVisAttr = scatterVisAttr;
}

void OkcVisualMap::doVisualMap() {
	assert(m_input);
	OkcData* okcdata = dynamic_cast<OkcData*>(m_input);

	this->m_visualMapResult = new OkcVisualMapResult(m_input);

	OkcVisualMapResult* okcVMR = dynamic_cast<OkcVisualMapResult*>(m_visualMapResult);

	Pipeline* pl = getPipeline();
	XmdvTool::PLTYPE plType = pl->getPipelineType();

	switch (plType) {
	// For flat normal and dimension reduction pipelines.
	// Note that dimension reduction pipeline is mostly similar to flat normal in the rendering stage
	case XmdvTool::PLTYPE_FLATNORMAL :
	case XmdvTool::PLTYPE_DIMR :
		//	{
		// Set highlighting and brush boundary
		okcVMR->setSBBResult(FALSE);
		setBrush(okcdata, okcVMR);

		// Set cardinality for the visual map result
		setCardinality(okcVMR);

		// Set the visual attributes for scatterplot matrices
		// if there are scatterplot matrices as output
		if(pl->willOuputToVis(XmdvTool::VISTYPE_SC)){
			okcVMR->setScatterVisAttr(this->m_scatterVisAttr);
			setDiagPlotColor(okcdata, okcVMR);
		}
		// Compute the position and size for glyphs only if there are star glyphs as output
		if (pl->willOuputToVis(XmdvTool::VISTYPE_GL)) {
			placeGlyph(okcdata, okcVMR);
			okcVMR->setGlyphShape(getGlyphShape());
		}
		//	}
		break;
		// For SBB pipeline
	case XmdvTool::PLTYPE_SBB :
	{
		// Set cardinality for the visual map result
		setCardinality(okcVMR);

		// Set the visual attributes for scatterplot matrices
		// if there are scatterplot matrices as output
		if(pl->willOuputToVis(XmdvTool::VISTYPE_SC)){
			okcVMR->setScatterVisAttr(this->m_scatterVisAttr);
			setDiagPlotColor(okcdata, okcVMR);
		}

		// Compute the position and size for glyphs
		//only if there are star glyphs as output
		if (pl->willOuputToVis(XmdvTool::VISTYPE_GL)) {
			placeGlyph(okcdata, okcVMR);
			okcVMR->setGlyphShape(getGlyphShape());
		}

		SBBPipeline* sbbPL = dynamic_cast<SBBPipeline*>(pl);
		okcVMR->setSBBResult(TRUE);
		okcVMR->setHierInfo(sbbPL->getHierInfo());
		setSBBColor(okcdata, okcVMR);
	}
	break;
	}
}

void OkcVisualMap::setDiagPlotColor(OkcData* okcdata, OkcVisualMapResult* okcVMR) {
	DiagVisAttr* diag_VisAttr = new DiagVisAttr();
	okcVMR->setDiagVisAttr(diag_VisAttr);
	diag_VisAttr->setOkcData(okcdata);
	diag_VisAttr->setOkcVisualMapResult(okcVMR);
	diag_VisAttr->diagColorMapping();

}

void OkcVisualMap::setBrush(OkcData* okcdata, OkcVisualMapResult* okcVMR) {
	int i;

	// if the current brush operator is not null,
	// it should be set to visual mapping result
	if (getBrushOperator()) {
		okcVMR->setBrushOperator(getBrushOperator());
	}

	// set the brush from the assistant input
	int n = getAssisInputSize();
	for (i=0; i<n; i++) {
		if (getAssisInputType(i)==XmdvTool::ASSISDATA_BRUSH) {
			OkcData* brushStorage = dynamic_cast<OkcData*>(getAssisInput(i));
			std::vector<double> buf;
			brushStorage->getData(buf, 3);
			Brush *brush = new Brush();
			brush->copyFromOkcDataStorage(brushStorage);
			okcVMR->setBrush(brush);
		}
	}

	VisualAttribute* inBrush = okcVMR->getInBrushVisAttr();
	VisualAttribute* outBrush = okcVMR->getOutBrushVisAttr();

	OkcDataModifierManager* manager = okcdata->getOkcDataModifierManager();
	OkcDataModifier* modifier  =
		manager->getOkcDataModifier(XmdvTool::MODIFIER_HIGHLIGHT);

	ColorManager *cm = g_globals.colorManager;
	VisualAttribute* newAttr;
	float index[okcdata->getDataSize()];
	int selectedDim = getPipeline()->getPipelineManager()->getMainWnd()->getHandleTools()->
	getColorStrategyManager()->getValueDim(getPipeline()->getPipelineID());

	// Generate the index (0.0f-1.0f) for the color for each record
	for(i=0; i < okcdata->getDataSize(); i++) {
		switch(cm->getStrategy()) {
		case ColorManager::NONE:
			index[i] = 1.0f;
			break;
		case ColorManager::VALUE:
			index[i] = float(((*okcdata->data_buf)[manager->getOrigLine(i)*okcdata->getOrigDimSize() + selectedDim] -
					okcdata->dim_min[selectedDim]) / (okcdata->dim_max[selectedDim] -
							okcdata->dim_min[selectedDim]));
			break;
		case ColorManager::ORDER:
			index[i] = float(i) / float(okcdata->getDataSize());
			break;
		default:
			index[i] = 1.0f;
		}
	}

	// Set the VisualAttribute and color for each record
	if (modifier) {
		// this okcdata has the highlight modifier
		OkcDataModifierHighlight* modifierHighlight =
			dynamic_cast<OkcDataModifierHighlight*>(modifier);
			for (i=0; i<okcdata->getDataSize(); i++) {
				if (modifierHighlight->getHighlight(manager->getOrigLine(i))) {
					newAttr = new VisualAttribute(*inBrush);
					newAttr->color = cm->getColor(ColorManager::BRUSH_SELECTED1, index[i]);
					okcVMR->setSingleDataVisAttr(newAttr, i);
				} else {
					newAttr = new VisualAttribute(*outBrush);
					newAttr->color = cm->getColor(ColorManager::BRUSH_UNSELECTED, index[i]);
					okcVMR->setSingleDataVisAttr(newAttr, i);
				}
			}
	} else {
		for (i=0; i<okcdata->getDataSize(); i++) {
			newAttr = new VisualAttribute(*outBrush);
			newAttr->color = cm->getColor(ColorManager::BRUSH_UNSELECTED, index[i]);
			okcVMR->setSingleDataVisAttr(newAttr, i);
		}
	}
}

void OkcVisualMap::setCardinality(OkcVisualMapResult* okcVMR) {
	int i, n = getAssisInputSize();
	for (i=0; i<n; i++) {
		if (getAssisInputType(i)==XmdvTool::ASSISDATA_CARD) {
			CardStorage* cardStorage = new CardStorage(	dynamic_cast<OkcData*>(getAssisInput(i)) );
			okcVMR->setCardinality( cardStorage->toARR() );
			delete cardStorage;
		}
	}

}

void OkcVisualMap::setSBBColor(OkcData* okcdata, OkcVisualMapResult* okcVMR) {
	ColorManager *cm = g_globals.colorManager;
	VisualAttribute* newAttr;

	OkcDataModifierManager* mm = okcdata->getOkcDataModifierManager();
	OkcDataModifierClusterColor* modifierColor  =
		dynamic_cast<OkcDataModifierClusterColor*>
	(mm->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERCOLOR));
	std::vector<double>& cc = modifierColor->getColorArr();

	OkcDataModifierClusterHighlight* modifierCH  =
		dynamic_cast<OkcDataModifierClusterHighlight*>
	(mm->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERHIGHLIGHT));
	std::vector<bool>& ch = modifierCH->getHighlightArr();

	int i;
	for (i=0; i<okcdata->getDataSize(); i++) {
		newAttr = new VisualAttribute();
		int origLine = mm->getOrigLine(i);
		double color = cc[origLine];
		if (ch[origLine]){
			newAttr->color = cm->getColor(ColorManager::BRUSH_SELECTED1, 1.0f);
		} else {
			newAttr->color = cm->getColor(ColorManager::BRUSH_UNSELECTED, color);
		}

		// band color is independent from SBB
		newAttr->bandColor = cm->getColor(ColorManager::BRUSH_UNSELECTED, color);
		okcVMR->setSingleDataVisAttr(newAttr, i);
	}
}


void OkcVisualMap::placeGlyph(OkcData* okcdata, OkcVisualMapResult* okcVMR)
{
	m_glyphPlace->setOkcData(okcdata);
	m_glyphPlace->placeGlyph();
	okcVMR->setGlyphPlaceResult(m_glyphPlace->getPlaceResult());
}
