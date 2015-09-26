/*
 * Transformation.h
 *
 *  Created on: Feb 4, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class Transformation manages a operator list
 * and can give the operator result based on the input
 */

#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_

#include <vector>
using namespace std;

class Data;
class Operator;
class BrushOperator;
class Pipeline;

class Transformation {
	// To declare Pipeline the friend class of Transformation
	// to allow Pipeline to access the private data member
	// such as m_operatorList, m_operatorList and m_operatorResultList
	friend class Pipeline;
	friend class SBBPipeline;
	friend class DimRPipeline;
	friend class PipelineManager;

public:
	Transformation();
	virtual ~Transformation();

protected:
	Data* m_input;

private:
	//The list of queries
	vector<Operator*> m_operatorList;
	//Whether the operator is enabled in the transformation
	vector<bool> m_enabled;
	// The list of operator results
	// Assume that m_operatorList is composed of Q0, Q1,... , Qn,
	// m_operatorResultList includes the operator results from queries
	// Q0, Q0+Q1, ..., and (Q0+Q1+...+Qn),
	// namely R0, R1, ..., Rn
	vector<Data*> m_operatorResultList;

public:
	void setInput(Data* input);
	Data* getInput();

	// An initialization function to set the pointer to the pipeine
	// for each operator.
	void setupTransformation(Pipeline* pipeline);

	// create m_operatorList from a operator array
	// enable all of operators by default
	// n is the length of array
	void setOperators(int n, Operator* operatorArr[]);

	// Return an operator specified by the order no
	Operator* getOperator(int i);

	// enable the ith operator when enabled=true;
	// disable the ith operator when enabled=false;
	void enableOperator(int i, bool enabled);
	// Return a boolean value to tell whether an operator is enabled
	bool isEnabledOperator(int i);

	// do queries in the operator list one by one
	// starting from the operator specified by startOperatorNo.
	// create and return the operator result
	// Please see below figure:
	// Q0, Q1, Q2, ..., Qn
	// R0, R1, R2, ..., Rn
	// If we start from Qi, this function will use R(i-1)
	// as the input
	// Please note that 0<=startOperatorNo<m_operatorList.size()
	Data* doOperator(int startOperatorNo);


	// Get the pointer to the input of an operator specified by operator no.
	// We retrieve it from intermediate operator result list (m_operatorResultList)
	// instead of the data member m_input of this operator, because one operator can
	// be shared by multiple transformations.  The data member m_input probably
	// contains the input of this operator in other operators. In addition, we should
	// get the m_operatorResultList[operatorNo-1] probably is not valid because the
	// operator (operatorNo-1) is disabled.  Thus we look backward until an enabled operator.
	Data* getOperatorInput(int operatorNo);

	// Get the pointer to the output of an operator specified by operator no.
	// We retrieve it from intermediate operator result list (m_operatorResultList)
	// instead of the data member m_output of this operator, because one operator can
	// be shared by multiple transformations.  The data member m_output probably
	// contains the output of this operator in other operators. If this operator is
	// disabled, we return 0;
	Data* getOperatorOutput(int operatorNo);

	// Reversely traverse the assistant transformation
	// to update the input.  Note that the operator
	// m_operatorList[i] will update m_input.  So this transformation
	// must be executed at least once.
	// arguments description
	// output: the input of reverse transformation
	void updateAssisInput(Data* output);

	// find the last instance int the operator list of class BrushOperator
	BrushOperator* findLastBrushOperator();

	// Check whether this transformation has the operator op.
	bool hasOperator(Operator* op);

};

#endif /* TRANSFORMATION_H_ */
