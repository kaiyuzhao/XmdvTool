/*
 * OkcViewDisplay.cpp
 *
 *  Created on: Jan 16, 2009
 *      Author: Zaixian Xie
 */

#include "view/multidim/OkcViewDisplay.h"

#include <cmath>
#include <typeinfo>

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Pipeline.h"
#include "interaction/MouseKeyboardStatus.h"
#include "operator/Operator.h"
#include "operator/brush/Brush.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "visualmap/VisualMapResult.h"
#include "visualmap/VisualAttribute.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/ViewWindow.h"
#include "view/ViewManager.h"
#include "view/GLDisplay.h"
#include "view/GLCanvas.h"
#include "color/ColorManager.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/OkcDataModifierHighlight.h"
#include "data/multidim/OkcDataModifier.h"
#include "data/cluster/OkcDataModifierClusterNodeMax.h"
#include "data/cluster/OkcDataModifierClusterNodeMin.h"
#include "data/cluster/OkcDataModifierClusterHighlight.h"

OkcViewDisplay::OkcViewDisplay(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm) :
	ViewDisplay (viewWindow, pm, pipelineID, vm) {
	m_input_mode = NONE;
	m_dim_size = 0;
	m_data_size = 0;
	m_hier_info = 0;
}

OkcViewDisplay::~OkcViewDisplay() {
}

OkcVisualMapResult* OkcViewDisplay::getOkcVisualMapResult() {
	VisualMapResult* vmr = getVisualMapResult();
	if (vmr) {
		// if vmr is not NULL, cast it to OkcVisualMapResult* and return
		OkcVisualMapResult* okc_vmr = dynamic_cast<OkcVisualMapResult*>(vmr);
		return okc_vmr;
	} else {
		// if vmr is NULL, return NULL
		return 0;
	}
}

OkcData* OkcViewDisplay::getDisplayDataset() {
	Data* data = getOkcVisualMapResult()->getData();
	OkcData* okcdata = dynamic_cast<OkcData*>(data);
	return okcdata;
}

void OkcViewDisplay::paint() {
    OkcVisualMapResult* vmr = getOkcVisualMapResult();

	// If the VisualMapResult is NULL, do nothing
	if (!vmr) {
		return;
	}

    // Set parameters about data size, dimension size and dimension attributes
	setParameters();

	// If the dataset does not have enough dimensions, do nothing
	if (!testDimensionality()) return;

	Data* data = vmr->getData();
	assert ( typeid(*data)==typeid(OkcData));
	// if the brush is not null, draw it
	Brush* curBrush = vmr->getBrush();
	if (curBrush) {
		drawSingleBrush(curBrush);
	}

	drawData();

	drawAxes();

}

bool OkcViewDisplay::testDimensionality() {
	return true;
}

void OkcViewDisplay::setParameters() {
	OkcVisualMapResult* vmr = getOkcVisualMapResult();
	if (vmr) {
		// Retrieve min and max values for dimensions and dimension size
		// to save time for future computation
		m_dim_size = vmr->getDimSize();
		m_data_size = vmr->getDataSize();
		m_dim_names = vmr->getDimNameArr();
		m_dim_min = vmr->getDimMinArr();
		m_dim_max = vmr->getDimMaxArr();
		m_dim_cardinality = vmr->getCardinality();
		m_hier_info = vmr->getHierInfo();
	}
}

void OkcViewDisplay::setPlacement() {
}

void OkcViewDisplay::drawData(){
	Pipeline* pl = getPipeline();
	switch (pl->getPipelineType()) {
	case XmdvTool::PLTYPE_FLATNORMAL :
	case XmdvTool::PLTYPE_DIMR :
		drawDataFlatNormal();
		break;
	case XmdvTool::PLTYPE_SBB :
		drawDataSBB();
		break;
	}
}

void OkcViewDisplay::drawDataFlatNormal() {
	int i;
	std::vector<double> buf;

	OkcVisualMapResult *vmr = getOkcVisualMapResult();
	OkcData* okcdata = getDisplayDataset();
	OkcDataModifierManager* manager = okcdata->getOkcDataModifierManager();
	OkcDataModifier* modifier  =
		manager->getOkcDataModifier(XmdvTool::MODIFIER_HIGHLIGHT);
	OkcDataModifierHighlight* modifierHighlight = 0;
	if (modifier) {
		// If the modifier with type MODIFIER_HIGHLIGHT exists,
		// cast it to type OkcDataModifierHighlight*
		modifierHighlight = dynamic_cast<OkcDataModifierHighlight*>(modifier);
	}

	if (modifierHighlight) {
		// If the modifier with type MODIFIER_HIGHLIGHT exists,
		// we draw unbrushed data firstly and then brushed data,
		// so the brush data will not be hidden by unbrushed data.
		// If this okcdata does not has the highlight modifier (brush),
		// draw all data using one loop, which is actually used to draw
		// the result from a SBBPipeline.
		for (i = 0; i < m_data_size; i++) {
			VisualAttribute* curAttr = vmr->getSingleDataVisAttr(i);
			// skip the brushed data
			if(modifierHighlight->getHighlight(manager->getOrigLine(i))) continue;
			m_canvas->setForeground(curAttr->color);
			okcdata->getData(buf, i);
			drawSingleData(buf, i);

		}
		// for brushed data only
		for (i = 0; i < m_data_size; i++) {
			VisualAttribute* curAttr = vmr->getSingleDataVisAttr(i);
			// skip the unbrushed data
			if(!(modifierHighlight->getHighlight(manager->getOrigLine(i)))) continue;
			m_canvas->setForeground(curAttr->color);
			okcdata->getData(buf, i);
			drawSingleData(buf, i);
		}
	} else {
		// Otherwise, we draw all data one by one
		for (i = 0; i < m_data_size; i++) {
			VisualAttribute* curAttr = vmr->getSingleDataVisAttr(i);
			m_canvas->setForeground(curAttr->color);
			okcdata->getData(buf, i);
			drawSingleData(buf, i);
		}
	}

	drawSpecialCustomization();
}
void OkcViewDisplay::drawDataSBB() {
	// The max and min values for each dimension
	// This is used to draw a band.
	std::vector<double> buf;
	// all_max and all_min are the upper and lower bound for each cluster
	// on all dimensions, including visible and invisible ones.
	std::vector<double> all_max, all_min;
	// max and min are the upper and lower bound for each cluster
	// on only visible dimensions.
	std::vector<double> max, min;
	OkcVisualMapResult *vmr = getOkcVisualMapResult();
	OkcData* okcdata = getDisplayDataset();
	OkcDataModifierManager* manager = okcdata->getOkcDataModifierManager();
	OkcDataModifierClusterNodeMax* modifierMax =
		dynamic_cast<OkcDataModifierClusterNodeMax*>
	(manager->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERNODEMAX));
	OkcDataModifierClusterNodeMin* modifierMin =
		dynamic_cast<OkcDataModifierClusterNodeMin*>
	(manager->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERNODEMIN));
	OkcDataModifierClusterHighlight* modifierHighlight =
		dynamic_cast<OkcDataModifierClusterHighlight*>
	(manager->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERHIGHLIGHT));

	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	int i;

	for (i = 0; i < m_data_size; i++) {
		int origLineIdx = manager->getOrigLine(i);

		bool brushed = modifierHighlight->getHighlight(origLineIdx);

		// Get the data and visual attributes first
		VisualAttribute* curAttr = vmr->getSingleDataVisAttr(i);
		okcdata->getData(buf, i);

		if (brushed && m_hier_info->fade_factor<0) {
			// for brushed data
			m_opacity=1+m_hier_info->fade_factor;
		} else if  (!brushed && m_hier_info->fade_factor>0) {
			// for unbrushed data
			m_opacity=1-m_hier_info->fade_factor;
		} else {
			m_opacity = 1.0;
		}

		//If the current configuration needs the band, draw it first
		if (m_hier_info->SHOW_BANDS) {
			// Get the cluster upper and lower bounds for all dimensions
			modifierMax->getClusterMax(all_max, manager->getOrigLine(i));
			modifierMin->getClusterMin(all_min, manager->getOrigLine(i));
			// Call the function mapData to turn on/off and reorder dimensions
			manager->mapData(all_max,  max);
			manager->mapData(all_min,  min);
			drawSingleBand(curAttr->bandColor, buf, max, min, i);
		}

		m_canvas->setForeground(curAttr->color, m_opacity);
		drawSingleData(buf, i);
	}

	glShadeModel(GL_FLAT);
	glDisable(GL_BLEND);
}

void OkcViewDisplay::drawSpecialCustomization() {

}

void OkcViewDisplay::drawAxes() {

}

void OkcViewDisplay::drawSingleData(std::vector<double> &, int ) {

}

void OkcViewDisplay::drawSingleBand(RGBt &, std::vector<double> &, std::vector<double> &, std::vector<double> &, int ) {

}

Brush* OkcViewDisplay::getBrush() {
	OkcVisualMapResult *vmr = getOkcVisualMapResult();
	Brush *brush = vmr->getBrush();
	return brush;
}

void OkcViewDisplay::drawSingleBrush(Brush*) {
}

void OkcViewDisplay::refreshBrush() {
	OkcVisualMapResult* okvVMR = getOkcVisualMapResult();
	Operator* brushQ = (Operator*)(okvVMR->getBrushOperator());
	PipelineManager* pm = getPipelineManager();
	Brush *brush = okvVMR->getBrush();

	//If the maximum and minimum values for one dimension has the exact same value,
	//they should be adjusted to a little different values,
	//otherwise, some records having the same value as this max/min value
	//cannot be highlighted.

	int i;
	for (i=0; i<brush->getDimSize(); i++) {
		brush->max[i] += XMDV_DBL_EPSILON;
		brush->min[i] -= XMDV_DBL_EPSILON;
	}

	OkcData* brushStorage = brush->toOkcDataStorage();
	std::vector<double> buf;
	brushStorage->getData(buf, 3);
	int pID = getPipelineID();
	// update the assistant input by the reverse pipeline
	pm->updateAssisInput(pID, brushStorage, XmdvTool::ASSISDATA_BRUSH);

	delete brushStorage;
	pm->assemble(pID, brushQ);
	getViewManager()->refreshDisplayByPID(pID);
}



bool OkcViewDisplay::betweenTwoPoint(double startPos, double endPos, double inputPos){
	double start, end;
	// ensure that start<end
	if (startPos<endPos) {
		start = startPos;
		end = endPos;
	} else {
		start = endPos;
		end = startPos;
	}

	// use XMDV_DBL_EPSILON to avoid judging inputPos>=startPos
	// which is not safe.
	if (inputPos>start-XMDV_DBL_EPSILON
			&& inputPos<end+XMDV_DBL_EPSILON ) {
		return true;
	} else {
		return false;
	}
}
