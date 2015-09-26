/*
 * OkcPipeline.h
 *
 *  Created on: May 21, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class OkcPipeline manages the pipeline
 * to visualize a multidimensional data.
 * It is a derived class of Pipeline.
 */

#ifndef OKCPIPELINE_H_
#define OKCPIPELINE_H_

#include "pipeline/Pipeline.h"
#include "datatype/XmdvType.h"

class OkcVisualMap;
class OkcPipelineSettings;
class SortRowsOperator;
class PCADeriveOperator;

class OkcPipeline : public Pipeline {
public:
	OkcPipeline();
	virtual ~OkcPipeline();

private:
	// This is the weight used in the ComDistOperator
	std::vector<int> m_weight;

protected:
	// tune up the operators and visual mapping
	// in terms of pipeline type (m_pipelineType)
	// pipeline settings (class PipelineSettings).
	// This is a virtual function.
	void setupPipeline();

	// Use the result from the assistant transformation to set the main transformation,
	// mainly focusing on setup parameters of operators. This is a virtual function.
	void setupMainTransformation();

	// Since the PCA algorithm only need to run once,
    // We set this boolean variable to true when this dialog shows first time,
    // If the PCA has been run once, m_needRunPCA will be false, and then
    // we do not need to run PCA again.
    bool m_needRunPCA;


public:
	// Return a string list to describe this pipeline
	// It should be rewritten in the subclass
	QStringList toStringList();

	OkcVisualMap* getOkcVisualMap();
	OkcPipelineSettings* getOkcPipelineSettings();

	// return m_needRunPCA
	bool needRunPCA();
	// set m_needRunPCA
	void setNeedRunPCA(bool need);

	// Refresh the brush storage.  This function
	// will set the input for the assistant transformation
	// for brush.
	// When to call: after the operator DimReductViewOperator
	// input: The input to the BrushOperator
	void refreshBrushStorage(OkcData* input);

	// Return the sort operator in this pipeline
	SortRowsOperator* getSortRowsOperator();

	// Return the pca operator in this pipeline
	PCADeriveOperator* getPCADeriveOperator();
};

#endif /* OKCPIPELINE_H_ */
