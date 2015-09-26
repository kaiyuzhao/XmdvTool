/*
 * SortRowsOnAllDimOperator.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: zyguo
 */

#include "SortRowsOnAllDimOperator.h"

#include "main/XmdvTool.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierHighlight.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/diagonal/RowIndexOnAllDimModifier.h"
#include "operator/Operator.h"
#include <stdlib.h>
#include <typeinfo>
#include "datatype/VectorUtil.h"

SortRowsOnAllDimOperator::SortRowsOnAllDimOperator() {
}

SortRowsOnAllDimOperator::~SortRowsOnAllDimOperator() {
}

void SortRowsOnAllDimOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(OkcData));
	Operator::setInput(input);
}

void SortRowsOnAllDimOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
}

bool SortRowsOnAllDimOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input) == typeid(OkcData));
	OkcData* input = (OkcData*) m_input;

	// Create a new instance of class OkcData as output
	// or use the old one
	OkcData* output = new OkcData(input);
	m_output = dynamic_cast<Data*> (output);

	OkcDataModifierManager* manager = output->getOkcDataModifierManager();
	OkcDataModifier* modifier = manager->getOkcDataModifier(
			XmdvTool::MODIFIER_ALLDIMROWINDEX);

	RowIndexOnAllDimModifier* rowIndexOnAllDimModifier;

	if (modifier) {
		// the modifier does exist
		rowIndexOnAllDimModifier = dynamic_cast<RowIndexOnAllDimModifier*> (modifier);

		// calculate histogram information here
		this->initRequiredArr();
		rowIndexOnAllDimModifier->initRequiredArr();
		SortAllDim();
		setAllDimSortModifierArray(rowIndexOnAllDimModifier);

		//histogramModifier->initRowIndexArr();
		//sortRows(output, modifierRowIndex);
	} else {
		// the modifier does not exist
		rowIndexOnAllDimModifier = new RowIndexOnAllDimModifier(manager->getOkcData());
		this->initRequiredArr();
		rowIndexOnAllDimModifier->initRequiredArr();
		SortAllDim();
		setAllDimSortModifierArray(rowIndexOnAllDimModifier);

		//Calculate_Histogram_Data();
		//setHistogramModifierArray(histogramModifier);
		//modifierRowIndex->initRowIndexArr();
		//sortRows(output, modifierRowIndex);
		manager->addOkcDataModifier(XmdvTool::MODIFIER_ALLDIMROWINDEX,
				rowIndexOnAllDimModifier);

	}

	return true;
}

void SortRowsOnAllDimOperator::SortAllDim(){
	OkcData* okcdata = (OkcData*) m_input;
	int data_size = okcdata->getDataSize();
	int dimSize = okcdata->getDimSize();
	SortRowsOperator::SortStruct *sortArray = new SortRowsOperator::SortStruct[data_size];

	int i;
	std::vector<double> data_buf;
	for (i = 0; i < data_size; i++) {
		this->sortOrderOnAllDim[0][i] = i;
	}
	for(int dim=0; dim<dimSize; dim++){
		okcdata->GetDimensionData(data_buf, dim);
		for (i = 0; i < data_size; i++) {
			sortArray[i].value = data_buf[i];
			sortArray[i].number = i;
		}
		qsort(sortArray, data_size, sizeof(SortRowsOperator::SortStruct),	SortRowsOperator::increaseSort);
		for (i = 0; i < data_size; i++) {
			this->sortOrderOnAllDim[dim+1][i] = sortArray[i].number;
		}
	}
	/*for (i = 0; i < data_size; i++) {
		this->sortOrderOnAllDim[dimSize][i] = i;
	}*/
	delete sortArray;

}

void SortRowsOnAllDimOperator::initRequiredArr(){

	OkcData* okcdata = (OkcData*) m_input;

	int dataSize = okcdata->getDataSize();
	int dimSize = okcdata->getDimSize();

	/*if(this->sortOrderOnAllDim.size()>0){
		for (int i = 0; i < dimSize + 1; i++) {
			sortOrderOnAllDim[i].resize(0);
		}
	}

	this->sortOrderOnAllDim.resize(0);

	this->sortOrderOnAllDim.resize(dimSize + 1);

	for (int i = 0; i < dimSize + 1; i++) {
		sortOrderOnAllDim[i].resize(dataSize);
	}*/

	VectorUtil::init2DVector(sortOrderOnAllDim, dimSize + 1, dataSize);
}

void SortRowsOnAllDimOperator::setAllDimSortModifierArray(RowIndexOnAllDimModifier* rowIndexOnAllDimModifier){
	OkcData* okcdata = (OkcData*) m_input;

	int dataSize = okcdata->getDataSize();
	int dimSize = okcdata->getDimSize();


	for (int i = 0; i < dimSize + 1; i++) {
		for (int j = 0; j < dataSize; j++) {
			rowIndexOnAllDimModifier->sortOrderOnAllDim[i][j] = this->sortOrderOnAllDim[i][j];
		}
	}

}
