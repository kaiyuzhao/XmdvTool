/*
 * VisualMap.h
 *
 *  Created on: Apr 1, 2008
 *      Author: Zaixian Xie
 */

/*
 * class VisualMap does the visual mapping in the
 * visualization pipeline.  It defines the basic behaviors of
 * visual mapping.  Developer should define a derived class
 * to handle a particular data type, e.g., OkcVisualMap
 *
 */
#ifndef VISUALMAP_H_
#define VISUALMAP_H_

#include "main/XmdvTool.h"
#include <vector>
using namespace std;

class Data;
class VisualMapResult;
class Pipeline;

class VisualMap
{
public:
	VisualMap();
	VisualMap(Data* input);
	virtual ~VisualMap();

protected:
	// the pipeline that this instance belongs to
	Pipeline* m_pipeline;
	// the main input data
	Data* m_input;
	// the assistant input data, such as the brush storage
	vector<Data*> m_assisInput;
	vector<XmdvTool::ASSISDATATYPE> m_assisInputType;
	VisualMapResult* m_visualMapResult;

public:
	void setPipeline(Pipeline* pipeline);
	Pipeline* getPipeline();

	virtual void setInput(Data* input);
	Data* getInput();

	// Set the result of all assistant transformations
	virtual void setAssisInput(int n, Data* assisInput[]);
	// Set the result of a specific assistant transformation
	virtual void setSingleAssisInput(int i, Data* assisInput);
	int getAssisInputSize();
	Data* getAssisInput(int i);

	// Set the result type of all assistant transformations
	virtual void setAssisInputType(int n, XmdvTool::ASSISDATATYPE assisInputType[]);
	// Set the result type of a specific assistant transformation
	virtual void setSingleAssisInputType(int i, XmdvTool::ASSISDATATYPE assisInputType);
	XmdvTool::ASSISDATATYPE getAssisInputType(int i);

	virtual void doVisualMap();
	VisualMapResult* getVisualMapResult();
};

#endif /*VISUALMAP_H_*/
