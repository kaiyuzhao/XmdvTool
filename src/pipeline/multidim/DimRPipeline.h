/*
 * DimRPipeline.h
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class DimRPipeline manages the pipeline
 * to visualize a multidimensional data via
 * dimension reduction.
 * It is a derived class of OkcPipeline.
 */

#ifndef DIMRPIPELINE_H_
#define DIMRPIPELINE_H_

#include <QStringList>

#include "pipeline/multidim/OkcPipeline.h"

class LinkList;

class DimRPipeline : public OkcPipeline {
public:
	DimRPipeline();
	virtual ~DimRPipeline();

private:
    // The pointer to the current dimension list chosen by the user
	LinkList* m_current_list;

protected:
	// Assign the pointer to the current dimension list to each operator
	void setupPipeline();
	// Use the result from the assistant transformation to set the main transformation,
	// mainly focusing on setup parameters of operators. This is a virtual function.
	void setupMainTransformation();

public:
	// Return a string list to describe this pipeline
	// It should be rewritten in the subclass
	QStringList toStringList();

	// Get a pointer to the current dimension list
	LinkList* getCurrentList();
};

#endif /* DIMRPIPELINE_H_ */
