/*
 * ViewOperator.h
 *
 *  Created on: Feb 5, 2009
 *      Author: Zaixian Xie
 */

/*
 * This class is the base of all queries whose types are
 * OPERATOR_VIEW, e.g. Dim3OOperator(Dim On/Off/Ordering)
 */

#ifndef VIEWOPERATOR_H_
#define VIEWOPERATOR_H_

#include "Operator.h"
#include "main/XmdvTool.h"

class ViewOperator: public Operator {
public:
	ViewOperator();
	virtual ~ViewOperator();

public:
	// return the operator type
	XmdvTool::OPERATORTYPE getOperatorType();

	// update m_input based on m_output
	// the subclass should define its behavior.
	virtual void updateInput();

	// setOutput() and getInput() make sense only for ViewOperator
	// when the function updateInput is called.
	virtual void setOutput(Data* output);
	virtual Data* getInput();
};

#endif /* VIEWOPERATOR_H_ */
