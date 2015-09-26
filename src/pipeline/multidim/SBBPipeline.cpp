/*
 * SBBPipeline.cpp
 *
 *  Created on: Jul 16, 2009
 *      Author: Zaixian Xie
 */

#include "pipeline/multidim/SBBPipeline.h"

#include <assert.h>

#include "main/XmdvTool.h"
#include "pipeline/Transformation.h"
#include "data/cluster/TreeContour.h"
#include "data/multidim/OkcData.h"
#include "operator/Operator.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "operator/sbb/ClusterOperator.h"
#include "operator/sbb/SBBOperator.h"

SBBPipeline::SBBPipeline()
	: OkcPipeline() {
	m_hier_info = 0;
	m_tree_contour = new TreeContour();
}

SBBPipeline::~SBBPipeline() {
	SAFE_DELETE(m_hier_info);
}

QStringList SBBPipeline::toStringList() {
	QStringList result;

	result << Pipeline::toStringList();

	// Get the output of the SBB operator, which is the displayed dataset
	// in the final visualizations
	Transformation* transf = this->getMainTransformation();
	SBBOperator* SBBQ = dynamic_cast<SBBOperator*>( transf->getOperator(PL_SBB_OP_SBB) );
	OkcData* outputOkcData = dynamic_cast<OkcData*>
		( SBBQ->getOutput() );
	result << QString("-- The number of records at the selected level : %1 ")
		.arg(outputOkcData->data_size);

	return result;
}

void SBBPipeline::setHierInfo(HierDisplayInformation* hier_info) {
	m_hier_info = hier_info;
}

HierDisplayInformation* SBBPipeline::getHierInfo() {
	return m_hier_info;
}

TreeContour* SBBPipeline::getTreeContour() {
	return m_tree_contour;
}

void SBBPipeline::setupPipeline() {
	// Call the derived class to do setup for this pipeline
	OkcPipeline::setupPipeline();

	// Do setup specific for this type of pipeline
	HierDisplayInformation* hierInfo = this->getHierInfo();
	TreeContour* tree_contour = getTreeContour();

	ClusterOperator* clusterQ = dynamic_cast<ClusterOperator*>
		(this->getMainTransformation()->m_operatorList[PL_SBB_OP_CLUSTER]);
	clusterQ->setHierDisplayInformation(hierInfo);
	clusterQ->setTreeContour(tree_contour);

	SBBOperator* SBBQ = dynamic_cast<SBBOperator*>
		(this->getMainTransformation()->m_operatorList[PL_SBB_OP_SBB]);
	SBBQ->setHierDisplayInformation(hierInfo);

}


void SBBPipeline::setupMainTransformation() {
	assert ( m_assisDataType.size()==PL_SBB_ASSIS_CNT );
}
