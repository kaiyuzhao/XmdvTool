/*
 * PCADeriveOperator.h
 *
 *  Created on: Sep 2, 2009
 *      Author: xiezx
 */

#ifndef PCADERIVEOPERATOR_H_
#define PCADERIVEOPERATOR_H_

#include "operator/Operator.h"

class Data;
class OkcData;
class OkcDataModifierPCADerive;

class PCADeriveOperator : public Operator {
public:
	PCADeriveOperator();
	virtual ~PCADeriveOperator();

private:
	// Do principal component analysis.
	// This function will be called by doOperator();
	bool doPCA(OkcData* okcdata, OkcDataModifierPCADerive* mod);

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
};


#endif /* PCADERIVEOPERATOR_H_ */
