/*
 * DimReductViewOperator.h
 *
 *  Created on: Sep 16, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class aims to generate a new dataset in a lower dimensional space.
 * We link this operator InterRing dialog, in which users can select
 * nodes in the dimension cluster tree. This operator projects the
 * original dataset to a lower dimensional space containing only
 * those selected dimension clusters as columns.
 */

#ifndef DIMREDUCTVIEWOPERATOR_H_
#define DIMREDUCTVIEWOPERATOR_H_

#include "operator/Operator.h"
#include "main/XmdvTool.h"

class Data;
class LinkList;

class DimReductViewOperator : public Operator {
public:
	DimReductViewOperator();
	virtual ~DimReductViewOperator();

private:
    // The pointer to the current dimension list chosen by the user
    // This list is saved in the DimRPipeline.  This class has only the reference to it.
	LinkList* m_current_list;

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

public:
	// set the pointer to the current dimension list
	void setCurrentList(LinkList* current_list);
	// get the pointer to the current dimension list
	LinkList* getCurrentList();

};

#endif /* DIMREDUCTVIEWOPERATOR_H_ */
