/*
 * DimRPipeline.cpp
 *
 *  Created on: Sep 10, 2009
 *      Author: Zaixian Xie
 */

#include "pipeline/multidim/DimRPipeline.h"

#include <assert.h>

#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"
#include "datatype/LinkList.h"
#include "pipeline/Transformation.h"
#include "operator/dimreduct/DimTreeOperator.h"
#include "operator/dimreduct/DimReductViewOperator.h"
#include "operator/brush/BrushOperator.h"

DimRPipeline::DimRPipeline() : OkcPipeline() {
	// initialize the current dimension list
	m_current_list = new LinkList();
}

DimRPipeline::~DimRPipeline() {
	SAFE_DELETE(m_current_list);
}

QStringList DimRPipeline::toStringList() {
	QStringList result;

	result << Pipeline::toStringList();

	// Get the number of selected dimension in the interring
	if (m_current_list) {
		result << QString( "-- The number of displayed dimensions : %1" )
			.arg( m_current_list->GetLength() );
	}

	return result;
}

void DimRPipeline::setupPipeline() {
	// Call the derived class to do setup for this pipeline
	OkcPipeline::setupPipeline();

	// Do setup specific for this type of pipeline
	// Set the pointer to current list to DimTreeOperator
	DimTreeOperator* dimTreeQ = dynamic_cast<DimTreeOperator*>
		(this->getMainTransformation()->m_operatorList[PL_DIMR_OP_DIMTREE]);
	dimTreeQ->setCurrentList(this->m_current_list);

	// Set the pointer to current list to DimReductViewOperator
	DimReductViewOperator* drViewQ = dynamic_cast<DimReductViewOperator*>
		(this->getMainTransformation()->m_operatorList[PL_DIMR_OP_DRVIEW]);
	drViewQ->setCurrentList(this->m_current_list);
}

void DimRPipeline::setupMainTransformation() {
	// We set the Brush for BrushOperator in the main transformation
	// if the BrushOperator is after DimOOOOperator.
	// Because brush probably was applied to DimOOOOperator,
	// we use the dimension on/off/reordering
	// result in the assistant transformation for the brush storage

	assert ( m_assisDataType.size()==PL_DIMR_ASSIS_CNT );

	// The index for the assistant transformation of the brush storage
	int brushTranIdx = PL_FN_ASSIS_BRUSH_INDEX;
	// If the BrushOperator is after DimOOOOperator,
	// we use the BrushStorage after DimOOOOperator as the Brush of
	// the BrushOperator.
	OkcData* brushStorageBeforeDim = dynamic_cast<OkcData*>
		( m_assisTransformations[brushTranIdx]->getOperatorInput(PL_DIMR_ASSIS_BRUSH_OP_DIMOOO) );

	OkcData* brushStorageAfterDim = dynamic_cast<OkcData*>
		( m_assisTransformations[brushTranIdx]->getOperatorOutput(PL_DIMR_ASSIS_BRUSH_OP_DIMOOO) );

	BrushOperator* brushQ = dynamic_cast<BrushOperator*>
		( m_mainTransformation->getOperator(PL_DIMR_OP_BRUSH) );

	// Although this is a constant expression, we still need to keep it
	// to make the future change convenient.
	if ( PL_DIMR_OP_BRUSH > PL_DIMR_OP_DIMOOO ) {
		brushQ->setBrushStorage(brushStorageAfterDim);
	} else {
		brushQ->setBrushStorage(brushStorageBeforeDim);
	}

}

LinkList* DimRPipeline::getCurrentList() {
	return m_current_list;
}
