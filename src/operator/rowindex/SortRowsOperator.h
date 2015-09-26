/*
 * SortRowsOperator.h
 *
 *  Created on: May 12, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class SortRowsOperator can sort rows in terms of
 * an existing or derived dimension.
 * It does not change the data order in the original buffer.
 * Instead, it will add an instance of
 * OkcDataModifierRowIndex to the input.
 */

#ifndef SORTROWSOPERATOR_H_
#define SORTROWSOPERATOR_H_

#include "operator/ViewOperator.h"
#include "main/XmdvTool.h"
#include "operator/diagonal/SortRowsOnAllDimOperator.h"

class OkcData;
class OkcDataModifierRowIndex;

class SortRowsOperator: public ViewOperator {
	friend class SortRowsOnAllDimOperator;
public:
	SortRowsOperator();
	virtual ~SortRowsOperator();

private:
	// in which way can we sort the rows
	XmdvTool::OKCDIM m_sortWay;
	// If m_sortWay=OKCDIM_ORIGINAL, we will use m_sortDimNo to denote
	// which dimension we will use, otherwise, m_sortDimNo does not make sense.
	int m_sortDimNo;
	// the sorting order
	XmdvTool::SORTORDER m_sortOrder;

public:
	// the functions to set the sorting configurations

	void setSortWay(XmdvTool::OKCDIM sortWay);
	XmdvTool::OKCDIM getSortWay();

	void setSortDimNo(int dimNo);
	int getSortDimNo();

	void setSortOrder(XmdvTool::SORTORDER order);
	XmdvTool::SORTORDER getSortOrder();

private:
	// sort the rows in terms of the current configuration
	void sortRows(OkcData* dataset, OkcDataModifierRowIndex* rowIndex);
	// sort the rows in terms of an original dimension
	void sortByDim(OkcData* dataset, OkcDataModifierRowIndex* rowIndex, int sortDimNo);

private:
	// the structure for sorting
	struct SortStruct{
		int number;
		double value;
	};

	// the function to be used by qsort. It can compare the values in two structures
	static int increaseSort( const void *elm1, const void *elm2);
	static int decreaseSort( const void *elm1, const void *elm2);

public:

	// The function setInput first do type verification
	// and then call the base class
	void setInput(Data* input);
	// Initialize m_brush
	void preOperator();
	// The virtual function to do the main work,
	// to highlight the subset in terms of the brush.
	bool doOperator();
	// Do some work after operator
	void postOperator();

	// update m_input
	void updateInput();
};

#endif /* SORTROWSOPERATOR_H_ */
