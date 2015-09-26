/*
 * SBBPipeline.h
 *
 *  Created on: Jul 16, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class SBBPipeline manages the pipeline
 * to visualize a multidimensional data via
 * a hierarchical display.
 * It is a derived class of OkcPipeline.
 */

#ifndef SBBPIPELINE_H_
#define SBBPIPELINE_H_

#include <QStringList>

#include "pipeline/multidim/OkcPipeline.h"

class HierDisplayInformation;
class TreeContour;

class SBBPipeline : public OkcPipeline {
public:
	SBBPipeline();
	virtual ~SBBPipeline();

private:
	// The hierarchical display information
	HierDisplayInformation* m_hier_info;
	// The data about tre contour
	TreeContour* m_tree_contour;

protected:
	// Configure the operators and visual mapping
	// in terms of the pipeline settings.
	void setupPipeline();

	// Use the result from the assistant transformation to set the main transformation,
	// mainly focusing on setup parameters of operators. This is a virtual function.
	void setupMainTransformation();

public:
	// Return a string list to describe this pipeline
	// It should be rewritten in the subclass
	QStringList toStringList();

	void setHierInfo(HierDisplayInformation* hier_info);
	HierDisplayInformation* getHierInfo();

	TreeContour* getTreeContour();
};

#endif /* SBBPIPELINE_H_ */
