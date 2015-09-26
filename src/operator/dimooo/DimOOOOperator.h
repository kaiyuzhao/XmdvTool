/*
 * DimOOOOperator.h
 *
 *  Created on: Feb 10, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class DimOOOOperator can add an instance of
 * OkcDataModifierDimOOO to the input to
 * do dimension On/Off/reOrdering.
 */

#ifndef DIMOOOOPERATOR_H_
#define DIMOOOOPERATOR_H_

#include <vector>
#include "operator/ViewOperator.h"

class DimOOOMap;

class DimOOOOperator: public ViewOperator {
public:
	DimOOOOperator();
	virtual ~DimOOOOperator();

private:
	DimOOOMap* m_dimOOOMap;

public:
	// initialize two ARR
	void initMap(int dims);

	DimOOOMap* getDimOOOMap();

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

	// update m_input using ARR map_in_to_out and on
	void updateInput();
};

#endif /* DIMOOOOPERATOR_H_ */
