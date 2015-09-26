/*
 * SortRowsOnAllDimOperator.h
 *
 *  Created on: Aug 21, 2009
 *      Author: zyguo
 */

#ifndef SORTROWSONALLDIMOPERATOR_H_
#define SORTROWSONALLDIMOPERATOR_H_

#include "operator/Operator.h"
#include "main/XmdvTool.h"
#include <vector>
#include "operator/rowindex/SortRowsOperator.h"
#include "data/multidim/diagonal/RowIndexOnAllDimModifier.h"

class OkcData;
class Operator;
//class HistogramModifier;

class SortRowsOnAllDimOperator : public Operator{
public:
	SortRowsOnAllDimOperator();
	virtual ~SortRowsOnAllDimOperator();

private:
	std::vector< std::vector <int> >  sortOrderOnAllDim;

public:
	// The function setInput first do type verification
	// and then call the base class
	void setInput(Data* input);
	// Initialize histogram operator
	void preOperator();
	// calculate the frequency in each histogram bin both for brushed and unbrushed data
	bool doOperator();
	void initRequiredArr();
	void setAllDimSortModifierArray(RowIndexOnAllDimModifier* rowIndexOnAllDimModifier);

	void SortAllDim();
};

#endif /* SORTROWSONALLDIMOPERATOR_H_ */
