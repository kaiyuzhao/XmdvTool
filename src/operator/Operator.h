/*
 * Operator.h
 *
 *  Created on: Jan 5, 2009
 *      Author: Zaixian Xie
 */

/*
 * This is the base class for all queries.
 * The data member "input" denotes what will enter this operator.
 * The function "doOperator()" will generate a OkcData as the output
 * and save it in the data member "output".
 * The subclass should implement the functions setInput(),
 * preOperator(), doOperator(), and postOperator() appropriately.
 * In order to do a operator, these functions should be called in the
 * following order:
 * setInput()
 * preOperator()
 * doOperator()
 * postOperator()
 * getOutput()
 *
 */

#ifndef OPERATOR_H_
#define OPERATOR_H_

#include "main/XmdvTool.h"

class Data;
class Pipeline;

class Operator {
public:
	Operator();
	virtual ~Operator();

protected:
	// The input and output of this operator
	Data* m_input;
	Data* m_output;

	// The pipeline that this operator belongs to
	// Note that an operator can be shared by multiple pipelines,
	// this variable will be set in the function
	// Transformation::setupTransformation(),
	// which will be called each time when the pipeline is assembled
	Pipeline* m_pipeline;

	// whether this operator is enabled.
	// if this operator is disabled, it will not do anything for input,
	// and just get a copy of input and set it as output
	bool m_enabled;

public:
	// return the operator type
	virtual XmdvTool::OPERATORTYPE getOperatorType();

	// Set and get the pointer to the pipeline that
	// this operator belongs to currently
	void setPipeline(Pipeline* pipeline);
	Pipeline* getPipeline();

	// The function setInput is virtual because
	// the derived class wants to do type verification
	virtual void setInput(Data* input);
	Data* getInput();

	Data* getOutput();

	// set the flag m_enabled
	void setEnabled(bool enabled);
	bool isEnabled();

	// Do some initialization work that needs to be done before operator,
	// e.g., setting operator parameters
	virtual void preOperator();
	// Do operator to generate an output and set to m_output.
	// Return true if operator is executed successfully,
	// otherwise return false.
	virtual bool doOperator();
	// Do some work that needs to be done after operator,
	// e.g., reset operator parameters
	virtual void postOperator();
};

#endif /* OPERATOR_H_ */
