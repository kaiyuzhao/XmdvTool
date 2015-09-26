/*
 * Operator.cpp
 *
 *  Created on: Jan 5, 2009
 *      Author: Zaixian Xie
 */

#include "Operator.h"
#include "data/Data.h"

Operator::Operator() {
	m_input = 0;
	m_output = 0;
}

Operator::~Operator() {
}

XmdvTool::OPERATORTYPE Operator::getOperatorType() {
	//by default, the type is OPERATOR_NORMAL
	return XmdvTool::OPERATOR_NORMAL;
}

void Operator::setInput(Data* input){
	m_input = input;
}

Data* Operator::getInput() {
	return m_input;
}

void Operator::setPipeline(Pipeline* pipeline) {
	m_pipeline = pipeline;
}

Pipeline* Operator::getPipeline() {
	return m_pipeline;
}

Data* Operator::getOutput() {
	return m_output;
}

void Operator::setEnabled(bool enabled) {
	m_enabled = enabled;
}

bool Operator::isEnabled() {
	return m_enabled;
}

void Operator::preOperator() {
}

bool Operator::doOperator() {
	if (m_input) {
		m_output = m_input;
	} else {
		m_output = new Data();
	}
	return true;
}

void Operator::postOperator() {

}
