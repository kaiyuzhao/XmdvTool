/*
 * SBBOperator.h
 *
 *  Created on: Jul 9, 2009
 *      Ported from XmdvTool 7.0 by Zaixian Xie
 */

#ifndef SBBOPERATOR_H_
#define SBBOPERATOR_H_

#include "operator/Operator.h"

class Data;
class HierDisplayInformation;

class SBBOperator : public Operator {
public:
	SBBOperator();
	virtual ~SBBOperator();

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

private:
	// the instance pointer to the hierarchical display information
	HierDisplayInformation* m_hierInfo;
	// represent which parameters in m_hier_info has been modified (dirty)
	// This variable will be set while initialization and users change
	// parameters on structure-based brush dialog
	int m_changeFlag;

public:
	void setHierDisplayInformation(HierDisplayInformation* hierInfo);
	HierDisplayInformation* getHierDisplayInformation();

	void clearChangeFlag();
	void markChangeFlag(int flag);
	int getChangeFlag();
};

#endif /* SBBOPERATOR_H_ */
