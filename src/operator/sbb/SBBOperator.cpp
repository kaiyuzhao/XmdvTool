/*
 * SBBOperator.cpp
 *
 *  Created on: Jul 9, 2009
 *      Ported from XmdvTool 7.0 by Zaixian Xie
 */

#include "operator/sbb/SBBOperator.h"

#include <typeinfo>
#include <assert.h>

#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"
#include "data/cluster/ClusterTree.h"
#include "operator/sbb/HierDisplayInformation.h"

SBBOperator::SBBOperator() : Operator() {
}

SBBOperator::~SBBOperator() {
}

void SBBOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(ClusterTree));
	Operator::setInput(input);
}

void SBBOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(ClusterTree));
}

bool SBBOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(ClusterTree));
	ClusterTree* input = dynamic_cast<ClusterTree*>(m_input);

	// If the non-brushed cluster radius has been changed,
	// re-mark the contour of non-brushed nodes
	if (m_changeFlag&DIRTY_CLUSTER_RADIUS) {
		ClusterTree::hierMarkContourNodes(input,
				m_hierInfo->root_radius * m_hierInfo->cluster_radius, IS_CONTOUR0);
	}

	// If the brushed cluster radius has been changed,
	// re-mark the contour of brushed nodes
	if (m_changeFlag&DIRTY_BRUSHED_RADIUS) {
		ClusterTree::hierMarkContourNodes( input,
				m_hierInfo->root_radius * m_hierInfo->brushed_radius, IS_CONTOUR1 );
	}

	// If the handle position has been changed,re-mark the node color
	if ( m_changeFlag&DIRTY_HANDLE_POSITION) {
	   	ClusterTree::SetupBrushParameters(input, m_hierInfo);
	}

	OkcData* output = new OkcData();
	input->generateSBBResult(output);

	SAFE_DELETE(m_output);
	m_output = (Data*)output;

	return true;
}

void SBBOperator::postOperator() {
}

void SBBOperator::setHierDisplayInformation(HierDisplayInformation* hierInfo) {
	m_hierInfo = hierInfo;
}

HierDisplayInformation* SBBOperator::getHierDisplayInformation() {
	return m_hierInfo;
}

void SBBOperator::clearChangeFlag(){
	m_changeFlag = 0;
}

void SBBOperator::markChangeFlag(int flag) {
	m_changeFlag |= flag;
}

int SBBOperator::getChangeFlag() {
	return m_changeFlag;
}
