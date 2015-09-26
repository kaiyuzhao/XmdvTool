/*
 * SortRowsOperator.cpp
 *
 *  Created on: May 12, 2009
 *      Author: Zaixian Xie
 */

#include "operator/rowindex/SortRowsOperator.h"
#include "main/XmdvTool.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierRowIndex.h"
#include "data/multidim/OkcDataModifierManager.h"
#include <stdlib.h>
#include <typeinfo>

SortRowsOperator::SortRowsOperator() : ViewOperator() {
}

SortRowsOperator::~SortRowsOperator() {
}


void SortRowsOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(OkcData));
	Operator::setInput(input);
}

void SortRowsOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
}

bool SortRowsOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
	OkcData* input = (OkcData*)m_input;

	// Create a new instance of class OkcData as output
	// or use the old one
	OkcData* output = new OkcData(input);
	m_output = dynamic_cast<Data*>(output);

	OkcDataModifierManager* manager = output->getOkcDataModifierManager();
	OkcDataModifier*  modifier =
		manager->getOkcDataModifier(XmdvTool::MODIFIER_ROWINDEX);
	OkcDataModifierRowIndex* modifierRowIndex;
	if (modifier) {
		// the modifier does exist
		modifierRowIndex = dynamic_cast<OkcDataModifierRowIndex*>(modifier);
		modifierRowIndex->initRowIndexArr();
		sortRows(output, modifierRowIndex);
	} else {
		// the modifier does not exist
		modifierRowIndex = new OkcDataModifierRowIndex(manager->getOkcData());
		modifierRowIndex->initRowIndexArr();
		sortRows(output, modifierRowIndex);
		manager->addOkcDataModifier
		(XmdvTool::MODIFIER_ROWINDEX, modifierRowIndex);
	}

	return true;
}

void SortRowsOperator::postOperator() {
}

void SortRowsOperator::updateInput() {

}

void SortRowsOperator::setSortWay(XmdvTool::OKCDIM sortWay) {
	m_sortWay = sortWay;
}

XmdvTool::OKCDIM SortRowsOperator::getSortWay() {
	return m_sortWay;
}

void SortRowsOperator::setSortDimNo(int dimNo) {
	m_sortDimNo = dimNo;
}

int SortRowsOperator::getSortDimNo() {
	return m_sortDimNo;
}

void SortRowsOperator::setSortOrder(XmdvTool::SORTORDER order) {
	m_sortOrder = order;
}

XmdvTool::SORTORDER SortRowsOperator::getSortOrder() {
	return m_sortOrder;
}

void SortRowsOperator::sortRows(OkcData* dataset, OkcDataModifierRowIndex* rowIndex) {
/*
	switch (m_sortWay) {
	case XmdvTool::OKCDIM_ORIGINAL :
		sortByDim(dataset, rowIndex, m_sortDimNo);
		break;
	case XmdvTool::OKCDIM_COMDIST :
		//m_sortDim==OKCDIM_COMDIST means that we will sort by one derived dimension
		//m_sortDimNo will always be -1, assuming this is called by pixel display only
		sortByDim(dataset, rowIndex, m_sortDimNo);
		break;
	default:
		break;
	}
*/
	//If m_sortDim==OKCDIM_ORIGINAL, we will sort by one dimension.
	//If m_sortDim==OKCDIM_COMDIST,  we will sort by one derived dimension and m_sortDimNo will always be -1
	sortByDim(dataset, rowIndex, m_sortDimNo);
}

void SortRowsOperator::sortByDim(OkcData* dataset, OkcDataModifierRowIndex* rowIndex, int sortDimNo) {
	int data_size = dataset->getDataSize();
	SortStruct *sortArray = new SortStruct[data_size];

	int i;
	std::vector<double> data_buf;
	for ( i = 0; i < data_size; i++ )
	{
		dataset->getData(data_buf, i);
		sortArray[i].value = data_buf[sortDimNo];
		sortArray[i].number = i;
	}

	switch (m_sortOrder) {
	case XmdvTool::SORTORDER_INCREASE :
		qsort( sortArray, data_size, sizeof( SortStruct ), SortRowsOperator::increaseSort );
		break;
	case XmdvTool::SORTORDER_DECREASE :
		qsort( sortArray, data_size, sizeof( SortStruct ), SortRowsOperator::decreaseSort );
		break;
	}

	for ( i = 0; i < data_size; i++ ) {
		rowIndex->setRowIndex(i, sortArray[i].number);
	}

	delete sortArray;


}

int SortRowsOperator::increaseSort ( const void *elm1, const void *elm2)
{
	double value1 = ((SortStruct *)elm1)->value;
	double value2 = ((SortStruct *)elm2)->value;

	if ( value1 < value2 )
		return -1;
	else if ( value1 == value2 )
		return 0;
	else
		return 1;
}

int SortRowsOperator::decreaseSort ( const void *elm1, const void *elm2)
{
	double value1 = ((SortStruct *)elm1)->value;
	double value2 = ((SortStruct *)elm2)->value;

	if ( value1 < value2 )
		return 1;
	else if ( value1 == value2 )
		return 0;
	else
		return -1;
}

