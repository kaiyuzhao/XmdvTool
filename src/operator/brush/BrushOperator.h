/*
 * BrushOperator.h
 *
 *  Created on: Jan 14, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class BrushOperator processes an OkcData
 * and highlight a subset of this data
 * in terms of brush.  Its input is an OkcData
 *  and it outputs a DerivedOkcData
 */

#ifndef BRUSHOPERATOR_H_
#define BRUSHOPERATOR_H_

#include "operator/Operator.h"

class Brush;

class BrushOperator: public Operator {
public:
	BrushOperator();
	virtual ~BrushOperator();

private:
	// The pointer to point to a brush storage
	OkcData* m_brushStorage;
	// The number of records highlighted.  This variable is not usable until
	// the function doOperator is called.
	int m_highlightedCount;

public:
	// Set the brush
	void setBrushStorage(OkcData* brushStorage);
	// Get the brush
	OkcData* getBrushStorage();

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

	// Return the number of highlighted recrods after executing this operator
	int getighlightedCount();
};

#endif /* BRUSHOPERATOR_H_ */
