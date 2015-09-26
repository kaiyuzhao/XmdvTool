/*
 * ViewOperator.cpp
 *
 *  Created on: Feb 5, 2009
 *      Author: Zaixian Xie
 */

#include "ViewOperator.h"

ViewOperator::ViewOperator() : Operator() {
}

ViewOperator::~ViewOperator() {
}

XmdvTool::OPERATORTYPE ViewOperator::getOperatorType() {
	return XmdvTool::OPERATOR_VIEW;
}

void ViewOperator::updateInput() {

}

void ViewOperator::setOutput(Data* output) {
	m_output = output;
}

Data* ViewOperator::getInput() {
	return m_input;
}

