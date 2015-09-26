/*
 * Transformation.cpp
 *
 *  Created on: Feb 4, 2009
 *      Author: Zaixian Xie
 */

#include "pipeline/Transformation.h"

#include <assert.h>
#include <vector>
#include <typeinfo>
#include <algorithm>
using namespace std;

#include "main/XmdvTool.h"
#include "pipeline/Pipeline.h"
#include "operator/Operator.h"
#include "operator/ViewOperator.h"
#include "operator/brush/BrushOperator.h"
#include "data/Data.h"

Transformation::Transformation() {
	m_operatorList.clear();
	m_operatorResultList.clear();
}

Transformation::~Transformation() {
	int i;
	if (!m_operatorResultList.empty()) {
		for (i=0; i<(int)m_operatorResultList.size(); i++) {
			SAFE_DELETE(m_operatorResultList[i]);
		}
	}
}

void Transformation::setupTransformation(Pipeline* pipeline) {
	int i, n=m_operatorList.size();
	for (i=0; i<n; i++) {
		m_operatorList[i]->setPipeline(pipeline);
	}
}

void Transformation::setInput(Data* input) {
	m_input = input;
}

Data* Transformation::getInput() {
	return m_input;
}

void Transformation::setOperators(int n, Operator* operatorArr[]) {
	m_operatorList.clear();
	m_operatorList.resize(n);
	m_enabled.clear();
	m_enabled.resize(n);;
	int i;
	for (i=0; i<n; i++) {
		m_operatorList[i] = operatorArr[i];
		m_enabled[i] = true;
	}
}

Operator* Transformation::getOperator(int i) {
	assert(i>=0);
	assert(i<(int)m_operatorList.size());
	return m_operatorList[i];
}


void Transformation::enableOperator(int i, bool enabled) {
	m_enabled[i] = enabled;
}

bool Transformation::isEnabledOperator(int i) {
	return m_enabled[i];
}

Data* Transformation::doOperator(int startOperatorNo) {
	//first find the input
	Data* input = 0;
	// The operator starting from which we will apply
	vector<Operator*>::iterator itStartOperator;



	if (startOperatorNo<0) {
		assert (m_operatorResultList.size()==m_operatorList.size());
		return m_operatorResultList.back();
	}

	if (m_operatorList.empty()) {
		return m_input;
	}
	assert(startOperatorNo<=(int)m_operatorList.size());

	int recentEnabledOperatorNo = startOperatorNo - 1;

	if (startOperatorNo==0) {
		input = m_input;
	} else {
		while(recentEnabledOperatorNo>=0 &&  m_enabled[recentEnabledOperatorNo]==false){
			recentEnabledOperatorNo--;
		}
		if(recentEnabledOperatorNo<0){
			input = m_input;
		}else if(m_enabled[recentEnabledOperatorNo]==true){
			input = m_operatorResultList[recentEnabledOperatorNo];
		}

	}

	m_operatorResultList.resize(m_operatorList.size(), 0);
	Data* result = 0;
	// Do operator one by one
	int i;
	for (i=startOperatorNo; i<(int)m_operatorList.size(); i++) {
		if (!m_enabled[i]) continue;
		m_operatorList[i]->setInput(input);
		m_operatorList[i]->preOperator();
		if ( !m_operatorList[i]->doOperator() ) {
			// If the operator failed, return a null pointer
			return 0;
		}
		m_operatorList[i]->postOperator();
		input = m_operatorList[i]->getOutput();
		m_operatorResultList[i] = m_operatorList[i]->getOutput();
		result = m_operatorList[i]->getOutput();
	}



	return result;//m_operatorResultList.back();
}

Data* Transformation::getOperatorInput(int operatorNo) {
	// We look backward until an enabled operator.
	// Please see comments in header file
	while (operatorNo-1>=0 && !m_enabled[operatorNo-1]) {
		operatorNo--;
	}
	if (operatorNo-1>=0) {
		// If we did not arrive at the first operator
		return m_operatorResultList[operatorNo-1];
	} else {
		// If we get the first operator, return the input of this transformation
		return m_input;
	}
}

Data* Transformation::getOperatorOutput(int operatorNo) {
	if ( m_enabled[operatorNo]) {
		return m_operatorResultList[operatorNo];
	} else {
		return 0;
	}
}

void Transformation::updateAssisInput(Data* output) {
	int i, n = m_operatorList.size();
	// all of queries must be view operator
	// (they do not change or create data)
	for (i=n-1; i>=0; i--) {
		assert(m_operatorList[i]->getOperatorType()==XmdvTool::OPERATOR_VIEW);
	}
	Data* inteResult = output;
	for (i=n-1; i>=0; i--) {
		// updateInput will use m_output to update m_input
		ViewOperator* viewOp = dynamic_cast<ViewOperator*>(m_operatorList[i]);
		// Set the output of this operator
		viewOp->setOutput(inteResult);

		if (i>0) {
			// If this is not the first operator, retrieve the input of this operator
			// from the intermediate result list of this transformation.  Note that
			// m_input of this operator is not valid because operators can be shared by
			// multiple transformations.
			viewOp->setInput( m_operatorResultList[i-1] );
		} else {
			// If this is the first operator, its input is just the input of this transformation.
			viewOp->setInput( m_input );
		}
		// Update the input and then m_input will be the updated data
		viewOp->updateInput();
		// Retrieve the updated input, which will be set to the previous operator
		inteResult = viewOp->getInput();
	}

}

BrushOperator* Transformation::findLastBrushOperator() {
	int i, n = m_operatorList.size();
	for (i=n-1; i>=0; i--) {
		// search the whole operator list from the end to the beginning
		if (typeid(*(m_operatorList[i]))==typeid(BrushOperator)) {
			return (BrushOperator*)m_operatorList[i];
		}
	}
	// if no brush operator is gotten, return NULL
	return 0;
}

bool Transformation::hasOperator(Operator* op) {
	vector<Operator*>::iterator it;
	it = find ( m_operatorList.begin(), m_operatorList.end(), op);
	if ( it == m_operatorList.end() ) {
		return false;
	} else {
		return true;
	}
}
