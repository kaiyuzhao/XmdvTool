/*
 * OkcPipeline.cpp
 *
 *  Created on: May 21, 2009
 *      Author: Zaixian Xie
 */

#include "pipeline/multidim/OkcPipeline.h"

#include <QStringList>
#include <assert.h>

#include "main/XmdvTool.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/PipelineSettings.h"
#include "pipeline/Transformation.h"
#include "pipeline/multidim/OkcPipelineSettings.h"
#include "operator/rowindex/SortRowsOperator.h"
#include "operator/brush/BrushOperator.h"
#include "operator/brush/Brush.h"
#include "operator/rowindex/SortRowsOperator.h"
#include "operator/pcaderive/PCADeriveOperator.h"
#include "visualmap/VisualMap.h"
#include "visualmap/VisualMapResult.h"
#include "visualmap/multidim/OkcVisualMap.h"
#include "visualmap/multidim/GlyphPlace.h"
#include "view/ViewManager.h"

OkcPipeline::OkcPipeline()
: Pipeline() {
	m_needRunPCA = true;
	// Ensure the initial length of the weights is 0.
	m_weight.clear();
}

OkcPipeline::~OkcPipeline() {
}

QStringList OkcPipeline::toStringList() {
	QStringList result;

	result << Pipeline::toStringList();

	// If no visual mapping result, this pipeline has not been assembled
	if ( !getPipelineManager()->getVisualMapResult( this ) ) {
		return result;
	}

	Transformation* transf = this->getMainTransformation();

	// Generate the sorting information
	//
	// Get the operator to sort records
	SortRowsOperator* sortRowsQ = dynamic_cast<SortRowsOperator*>
	(transf->getOperator(PL_FN_OP_SORT));
	if ( transf->isEnabledOperator(PL_FN_OP_SORT) ) {
		XmdvTool::SORTORDER sortOrder = sortRowsQ->getSortOrder();
		QString sortDesc;
		if ( sortOrder==XmdvTool::SORTORDER_INCREASE) {
			sortDesc = "increasing";
		} else {
			sortDesc = "decreasing";
		}
		Data* inputData = sortRowsQ->getInput();
		OkcData* inputOkcData = dynamic_cast<OkcData*>(inputData);
		int dimNo = sortRowsQ->getSortDimNo();
		QString dimName;
		if ( dimNo>=0 && dimNo<(int)inputOkcData->names.size() ) {
			dimName = inputOkcData->names[dimNo];
		}
		result << QString("-- Sorted : Yes ( %1, by dimension '%2' )")
		.arg(sortDesc).arg( dimName );
	} else {
		result << "-- Sorted: No";
	}

	// Get the number of highlighted records
	BrushOperator* brushQ =  dynamic_cast<BrushOperator*>
		(transf->getOperator(PL_FN_OP_BRUSH));
	result << QString( "-- The number of highlighted records : %1")
		.arg( brushQ->getighlightedCount() );;

	return result;
}

void OkcPipeline::setupPipeline() {
	OkcVisualMap* okc_vm = getOkcVisualMap();
	GlyphPlace* glyphPlace = okc_vm->getGlyphPlace();

	okc_vm->setGlyphShape(XmdvTool::GLYPHSHAPE_STAR);

	OkcPipelineSettings* pls = getOkcPipelineSettings();

	glyphPlace->setGlyphPlaceMode(pls->m_glyphPlaceMode);

	// call the Pipeline::setupPipeline() to do some common work
	Pipeline::setupPipeline();
}

void OkcPipeline::setupMainTransformation() {
	// We set the Brush for BrushOperator in the main transformation
	// if the BrushOperator is after DimOOOOperator.
	// Because brush probably was applied to DimOOOOperator,
	// we use the dimension on/off/reordering
	// result in the assistant transformation for the brush storage
	assert ( m_assisDataType.size()==PL_FN_ASSIS_CNT );

	// The index for the assistant transformation of the brush storage
	int brushTranIdx = PL_FN_ASSIS_BRUSH_INDEX;
	// If the BrushOperator or the ComDistOperator is after DimOOOOperator,
	// we use the BrushStorage after DimOOOOperator as the Brush of
	// the BrushOperator or the ComDistOperator.
	OkcData* brushStorageBeforeDim = dynamic_cast<OkcData*>
		( m_assisTransformations[brushTranIdx]->getOperatorInput(PL_FN_ASSIS_BRUSH_OP_DIMOOO) );
	OkcData* brushStorageAfterDim = dynamic_cast<OkcData*>
		( m_assisTransformations[brushTranIdx]->getOperatorOutput(PL_FN_ASSIS_BRUSH_OP_DIMOOO) );
	BrushOperator* brushQ = dynamic_cast<BrushOperator*>
		( m_mainTransformation->getOperator(PL_FN_OP_BRUSH) );
	// Although this is a constant expression, we still need to keep it
	// to make the future change convenient.
	if ( PL_FN_OP_BRUSH > PL_FN_OP_DIMOOO ) {
		brushQ->setBrushStorage(brushStorageAfterDim);
	} else {
		brushQ->setBrushStorage(brushStorageBeforeDim);
	}

}

OkcVisualMap* OkcPipeline::getOkcVisualMap() {
	VisualMap* vm = this->getVisualMap();
	return dynamic_cast<OkcVisualMap*>(vm);
}

OkcPipelineSettings* OkcPipeline::getOkcPipelineSettings() {
	PipelineSettings* pls = getPipelineSettings();
	return dynamic_cast<OkcPipelineSettings*>(pls);
}

bool OkcPipeline::needRunPCA() {
	return m_needRunPCA;
}

void OkcPipeline::setNeedRunPCA(bool need) {
	m_needRunPCA = need;
}

void OkcPipeline::refreshBrushStorage(OkcData* input) {

	// Currently, this function will be called
	// only in dimension reduction pipeline
	assert( getPipelineType() == XmdvTool::PLTYPE_DIMR );

    // prepare the brush storage
    Brush brush;
    brush.Init(input);
    OkcData* brushStorage = brush.toOkcDataStorage();
    // add to operator list to allow PipelineManager destructor
    // to release memory occupied by this data
    getPipelineManager()->addAssisInputData((Data*)brushStorage);

    // Set the brush storage to the input of assistant transformation
	m_assisInputData[PL_DIMR_ASSIS_BRUSH_INDEX] = (Data*)brushStorage;

	// Reassemble the assistant tranformation for brush storage
	m_assisTransformations[PL_DIMR_ASSIS_BRUSH_INDEX]->
		setInput(m_assisInputData[PL_DIMR_ASSIS_BRUSH_INDEX]);
	Data* assisResult =
		m_assisTransformations[PL_DIMR_ASSIS_BRUSH_INDEX]->doOperator(0);
	XmdvTool::ASSISDATATYPE assisResultType = m_assisDataType[PL_DIMR_ASSIS_BRUSH_INDEX];

	// Now the assistant input will go to visual map
	m_visualMap->setSingleAssisInput(PL_DIMR_ASSIS_BRUSH_INDEX, assisResult);
	m_visualMap->setSingleAssisInputType(PL_DIMR_ASSIS_BRUSH_INDEX, assisResultType);

}

SortRowsOperator* OkcPipeline::getSortRowsOperator() {
	SortRowsOperator* sortOp = 0;
	Transformation* mainT = getMainTransformation();
	switch (getPipelineType()) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		sortOp = dynamic_cast<SortRowsOperator*>(mainT->getOperator(PL_FN_OP_SORT));
		break;
	case XmdvTool::PLTYPE_SBB :
		sortOp = dynamic_cast<SortRowsOperator*>(mainT->getOperator(PL_SBB_OP_SORT));
		break;
	default:
		break;
	}
	return sortOp;
}

PCADeriveOperator* OkcPipeline::getPCADeriveOperator() {
	PCADeriveOperator* pcaOp = 0;
	Transformation* mainT = getMainTransformation();
	switch ( getPipelineType() ) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		pcaOp = dynamic_cast<PCADeriveOperator*>(mainT->getOperator(PL_FN_OP_PCADERIVE));
		break;
	case XmdvTool::PLTYPE_SBB :
		pcaOp = dynamic_cast<PCADeriveOperator*>(mainT->getOperator(PL_SBB_OP_PCADERIVE));
		break;
	default:
		break;
	}
	return pcaOp;
}

