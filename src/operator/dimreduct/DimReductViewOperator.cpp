/*
 * DimReductViewOperator.cpp
 *
 *  Created on: Sep 16, 2009
 *      Ported by Zaixian Xie
 */

#include "operator/dimreduct/DimReductViewOperator.h"

#include <typeinfo>
#include <assert.h>
#include "datatype/LinkList.h"
#include "data/dimreduct/InterRingDimClusterTree.h"
#include "data/dimreduct/InterRingDimCluster.h"
#include "data/multidim/OkcData.h"
#include "pipeline/multidim/OkcPipeline.h"

DimReductViewOperator::DimReductViewOperator() : Operator() {
}

DimReductViewOperator::~DimReductViewOperator() {
}

void DimReductViewOperator::setCurrentList(LinkList* current_list){
	m_current_list = current_list;
}

LinkList* DimReductViewOperator::getCurrentList() {
	return m_current_list;
}

void DimReductViewOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(InterRingDimClusterTree));
	Operator::setInput(input);
}

void DimReductViewOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(InterRingDimClusterTree));
}

bool DimReductViewOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(InterRingDimClusterTree));
	InterRingDimClusterTree* input = (InterRingDimClusterTree*)m_input;

	OkcData* output = new OkcData();
	input->generateDimReductResult(output, getCurrentList());

	SAFE_DELETE(m_output);
	m_output = dynamic_cast<Data*>(output);

	// Because we get a new dataset having the different structure
	// from the original data, we need to refresh the brush storage
	OkcPipeline* okc_pl = dynamic_cast<OkcPipeline*> (getPipeline());
	okc_pl->refreshBrushStorage(output);

	return true;
}

void DimReductViewOperator::postOperator() {
}
