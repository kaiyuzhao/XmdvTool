/*
 * DimTreeOperator.h
 *
 *  Created on: Sep 8, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class DimTreeOperator generates a dimension cluster tree
 * based on an OkcData. Its input is a OkcData and output is
 * InterRingDimClusterTree
 */

#ifndef DIMTREEOPERATOR_H_
#define DIMTREEOPERATOR_H_

#include "operator/Operator.h"
#include "main/XmdvTool.h"

class Data;
class LinkList;
class InterRingDimClusterTree;

class DimTreeOperator : public Operator {
public:
	DimTreeOperator();
	virtual ~DimTreeOperator();

private:
	// The dc file name to contain the dimension cluster tree information.
	// If this file does not exist, this operator will generate dimension cluster tree
	// and store it in this file; otherwise, the output will be directly from this file.
	char m_dcFileName[FILENAME_MAXLEN];

    // The pointer to the current dimension list chosen by the user
    // This list is saved in the DimRPipeline.  This class has only the reference to it.
	LinkList* m_current_list;

public:
	void setDcFileName(char *dcFileName);
	char* getDcFileName();

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

private:
	// Create an initial dimension list containing all original dimension.
	void initCurrentList(InterRingDimClusterTree* dtree);
	// The function to compare the okc id of two nodes.
	// When we sort the nodes in the current dimension list, we need this function.
	static int compareDim( const void *elm1, const void *elm2);
};

#endif /* DIMTREEOPERATOR_H_ */
