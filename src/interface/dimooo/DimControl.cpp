/*
 * DimControl.cpp
 *
 *  Created on: Nov 25, 2009
 *      Author: Zaixian Xie
 */

#include "interface/dimooo/DimControl.h"

#include <typeinfo>

#include "main/XmdvToolMainWnd.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Transformation.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/DimOOOMap.h"
#include "operator/dimooo/DimOOOOperator.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/multidim/VisDimstack.h"
#include "view/multidim/VisPixel.h"

DimControl::DimControl() {
}

DimControl::~DimControl() {
}

OkcData* DimControl::getInputOkcData(Pipeline* pl) {
	OkcData* inputOkc = 0;
	switch (pl->getPipelineType()) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		// The normal pipeline type for flat display
		inputOkc = dynamic_cast<OkcData*>
		( pl->getMainTransformation()->getOperatorInput(PL_FN_OP_DIMOOO) );
		break;
	case XmdvTool::PLTYPE_SBB :
		inputOkc = dynamic_cast<OkcData*>
		( pl->getMainTransformation()->getOperatorInput(PL_SBB_OP_DIMOOO) );
		break;
		// The pipeline type to support dimension reduction (InterRing)
	case XmdvTool::PLTYPE_DIMR :
		// TODO: This should be enabled after we added dimension on/off/reorder
		// operator in the dimension reduction pipeline
		inputOkc = 0;
		break;
	}

	return inputOkc;
}

DimOOOMap* DimControl::getDimOOOMap(Pipeline* pl) {
	return getDimOOOOperator(pl)->getDimOOOMap();
}

DimOOOOperator* DimControl::getDimOOOOperator(Pipeline* pl) {
	DimOOOOperator* op = 0;

	switch (pl->getPipelineType()) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		// The normal pipeline type for flat display
		op = dynamic_cast<DimOOOOperator*>
			( pl->getMainTransformation()->getOperator(PL_FN_OP_DIMOOO) );
		break;
	case XmdvTool::PLTYPE_SBB :
		// The pipeline for structure-based brush
		op = dynamic_cast<DimOOOOperator*>
			( pl->getMainTransformation()->getOperator(PL_SBB_OP_DIMOOO) );
		break;
	// The pipeline type to support dimension reduction (InterRing)
	case XmdvTool::PLTYPE_DIMR :
		// TODO: This should be enabled after we added dimension on/off/reorder
		// operator in the dimension reduction pipeline
		break;
	}

	return op;
}

void DimControl::updateDisplay(XmdvToolMainWnd* mainWnd, PipelineManager* pm, int pID, ViewWindow *) {
	Pipeline* pl = pm->getPipeline(pID);
	switch (pl->getPipelineType()) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		// The normal pipeline type for flat display
		pm->assemble(pID, PL_FN_OP_DIMOOO);
		break;
	case XmdvTool::PLTYPE_SBB :
		// The pipeline for structure-based brush
		pm->assemble(pID, PL_SBB_OP_DIMOOO);
		break;
	// The pipeline type to support dimension reduction (InterRing)
	case XmdvTool::PLTYPE_DIMR :
		// TODO: This should be enabled after we added dimension on/off/reorder
		// operator in the dimension reduction pipeline
		break;
	}

	ViewManager* vm = mainWnd->getViewManager();
	vm->refreshDisplayByPID(pID);
}

void DimControl::getDimNames(Pipeline* pl, std::vector<char*>& dim_names) {
	// Get the input of the dimension on/off/reorder operator
	OkcData* inputOkc = getInputOkcData(pl);
	// Get all dimension names
	inputOkc->getDimNameArr(dim_names);
}
