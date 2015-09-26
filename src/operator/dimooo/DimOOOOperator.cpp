/*
 * DimOOOOperator.cpp
 *
 *  Created on: Feb 10, 2009
 *      Author: Zaixian Xie
 */

#include "operator/dimooo/DimOOOOperator.h"

#include <typeinfo>

#include "operator/ViewOperator.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierDimOOO.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/DimOOOMap.h"
#include "data/storage/CardStorage.h"
#include <vector>

DimOOOOperator::DimOOOOperator() : ViewOperator() {
	m_dimOOOMap = new DimOOOMap();
}

DimOOOOperator::~DimOOOOperator() {
	SAFE_DELETE(m_dimOOOMap);
}

void DimOOOOperator::initMap(int dims) {
	m_dimOOOMap->setDimSize(dims);
	// initialize the map to turn on all dimensions
	// and keep the original dimension order
	m_dimOOOMap->initMap();
}

DimOOOMap* DimOOOOperator::getDimOOOMap() {
	return m_dimOOOMap;
}

void DimOOOOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert( typeid(*input)==typeid(OkcData)
			|| typeid(*input)==typeid(CardStorage) );
	Operator::setInput(input);
}

void DimOOOOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert( typeid(*m_input)==typeid(OkcData)
			|| typeid(*m_input)==typeid(CardStorage) );
	OkcData* input = (OkcData*)m_input;

	// If the current array map_in_to_out or on has the
	// different size than the input, initialize them
	int dimSize = input->getDimSize();
	if ( dimSize!= getDimOOOMap()->getDimSize() ) {
		getDimOOOMap()->setDimSize(dimSize);
		getDimOOOMap()->initMap();
	}
}

bool DimOOOOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert( typeid(*m_input)==typeid(OkcData)
			|| typeid(*m_input)==typeid(CardStorage) );
	OkcData* input = (OkcData*)m_input;

	// Create a new instance of class OkcData as output
	// or use the old one
	OkcData* output = new OkcData(input);
	m_output = dynamic_cast<Data*>(output);

	OkcDataModifierManager* manager = output->getOkcDataModifierManager();
	OkcDataModifier*  modifier =
		manager->getOkcDataModifier(XmdvTool::MODIFIER_DIMOOO);
	OkcDataModifierDimOOO* modifierDimOOO;
	if (modifier) {
		// the modifier does exist
		modifierDimOOO = dynamic_cast<OkcDataModifierDimOOO*>(modifier);
	} else {
		// the modifier does not exist
		modifierDimOOO = new OkcDataModifierDimOOO(manager->getOkcData());
		manager->addOkcDataModifier
			(XmdvTool::MODIFIER_DIMOOO, modifierDimOOO);
	}

	// copy the map information
	*(modifierDimOOO->getDimOOOMap()) = *(this->getDimOOOMap());
	return true;
}

void DimOOOOperator::postOperator() {
}

void DimOOOOperator::updateInput() {
	OkcData* input = dynamic_cast<OkcData*>(m_input);
	OkcData* output = dynamic_cast<OkcData*>(m_output);
	int i, j;

	OkcDataModifierManager* modiManager =
		input->getOkcDataModifierManager();
	OkcDataModifier* modi =
		modiManager->getOkcDataModifier(XmdvTool::MODIFIER_DIMOOO);
	// input should not have DimOOO modifier
	assert(modi==0);

	int input_data_size = input->getDataSize();
	int input_dim_size = input->getDimSize();

	// posInView denote the positions in the view
	// for each dim of input
	std::vector<int> posInView;
	getDimOOOMap()->getPosInView(posInView);

	std::vector<double> inputData, outputData;
	for (i=0; i<input_data_size; i++) {
		//get the input data
		input->getData(inputData, i);
		output->getData(outputData, i);
		for (j=0; j<input_dim_size; j++) {
			if (posInView[j]>=0) {
				// change the input data based on the output data
				inputData[j] = outputData[posInView[j]];
			}
		}
		input->setData(inputData, i);
	}

}

