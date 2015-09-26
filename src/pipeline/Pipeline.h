/*
 * Pipeline.h
 *
 *  Created on: Jan 8, 2009
 *      Author: Zaixian Xie
 */

/*
 * class Pipeline manages a visualization pipeline,
 * which is composed of a main transformation and
 * some assistant transformations.
 * The main transformation processes the main input
 * denoted by m_origDataset.
 * Other transformations process assistant data, such as brush storage.
 */

#ifndef PIPELINE_H_
#define PIPELINE_H_

#include "main/XmdvTool.h"
#include <vector>
using namespace std;

#include <QString>
#include <QStringList>

class PipelineManager;
class OkcData;
class VisualMap;
class VisualMapResult;
class Operator;
class Data;
class Transformation;
class PipelineSettings;

class Pipeline {
	// To declare PipelineManager as the friend class of Pipeline
	// to allow PipelineManager to access the private data member
	// such as m_origDataset, m_operatorList and m_visualMap
	friend class PipelineManager;

public:
	Pipeline();
	virtual ~Pipeline();

protected:
	PipelineManager* m_pipelineManager;
	int m_pipelineID;

	XmdvTool::PLTYPE m_pipelineType;

	PipelineSettings* m_plSettings;

	//The original dataset
	Data *m_origDataset;
	// The main transformation
	Transformation *m_mainTransformation;

	// The assistant transformations
	vector<XmdvTool::ASSISDATATYPE> m_assisDataType;
	vector<Data*> m_assisInputData;
	vector<Transformation*> m_assisTransformations;

	VisualMap *m_visualMap;

protected:
	// tune up the operators and visual mapping
	// in terms of pipeline type (m_pipelineType)
	// pipeline settings (class PipelineSettings)
	// This function should be overloaded by different pipeline types
	virtual void setupPipeline();

	// Use the result from the assistant transformation to set the main transformation,
	// mainly focusing on setup parameters of operators. This function should be
	// overloaded by different pipeline types
	virtual void setupMainTransformation();

public:
	// Return a string representation of pipeline type
	static QString getTypeString(XmdvTool::PLTYPE plType);

	// Return a string list to describe this pipeline
	// It should be rewritten in the subclass
	virtual QStringList toStringList();

	void setOrigDataset(Data* data);
	Data* getOrigDataset();
	VisualMap* getVisualMap();

	void setPipelineManager(PipelineManager* pm);
	PipelineManager* getPipelineManager();

	void setPipelineID(int ID);
	int getPipelineID();

	void setPipelineType(XmdvTool::PLTYPE type);
	XmdvTool::PLTYPE getPipelineType();

	void setPipelineSettings(PipelineSettings* plSettings);
	PipelineSettings* getPipelineSettings();

	// Return the pointer to the main transformation
	Transformation* getMainTransformation();

	// Return the transformation corresponding to a specified assistant data type
	Transformation* getAssisTransformation(XmdvTool::ASSISDATATYPE dataType);

	// To assemble the visualization pipeline
	// starting from the operator specified by startOperatorNo.
	// Please see below figure:
	// Q0, Q1, Q2, ..., Qn
	// R0, R1, R2, ..., Rn
	// If we start from Qi, the assembling will use R(i-1)
	// as the input
	// If No<0 means that only the visualMap will be done,
	// and the last element in the list m_operatorResultList
	// will be used as the input
	VisualMapResult* assemble(int startOperatorNo);

	// The same function as assemble(int startOperatorNo)
	// but starting from the operator specified by startOperator.
	// If startOperator==NULL, only the visualMap will be done.
	VisualMapResult* assemble(Operator* startOperator);

	// Reversely traverse the assistant transformation
	// to update the input.
	// output: the input of reverse transformation
	// type: the assistant data type
	void updateAssisInput(Data* output, XmdvTool::ASSISDATATYPE type);

	// Determine whether this pipeline will be output to a specific visualization type
	// It is used for optimization purpose.  For example, if the pipeline is not output
	// to glyphs, the instance of GlyphPlace can be deactivated in visual mapping
	bool willOuputToVis(XmdvTool::VISTYPE visType);

	// Check whether this pipeline has the operator op.
	bool hasOperator(Operator* op);
};

#endif /* PIPELINE_H_ */
