/*
 * ClusterOperator.h
 *
 *  Created on: Jun 4, 2009
 *      Author: Zaixian xie
 */

/*
 * Class ClusterOperator can do clustering on an input OkcData
 * and set the clustering result as the output.  If a .cg file exists,
 * this operator can read cluster directly from .cg file instead of
 * do clustering on OkcData
 */

#ifndef CLUSTEROPERATOR_H_
#define CLUSTEROPERATOR_H_

#include "operator/Operator.h"
#include "main/XmdvTool.h"

class HierDisplayInformation;
class TreeContour;

class ClusterOperator : public Operator {
public:
	ClusterOperator();
	virtual ~ClusterOperator();

private:
	char m_cgFileName[FILENAME_MAXLEN];
	// the instance pointer to the hierarchical display information
	HierDisplayInformation* m_hierInfo;
	TreeContour* m_tree_contour;

public:
	void setCgFileName(char *cgFileName);
	char* getCgFileName();

	void setHierDisplayInformation(HierDisplayInformation* hierInfo);
	HierDisplayInformation* getHierDisplayInformation();

	void setTreeContour(TreeContour* tree_contour);
	TreeContour* getTreeContour();

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

#endif /* CLUSTEROPERATOR_H_ */
