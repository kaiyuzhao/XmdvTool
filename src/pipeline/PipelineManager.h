/*
 * This class manages all pipelines.
 * It maintain a vector of class pipelines, namely m_pipelines.
 * The function assemble(int i=-1) can assemble all
 * pipelines or one specified by parameter i.
 * All of assembled results are stored in a vector
 */

#ifndef PIPELINEMANAGER_H_
#define PIPELINEMANAGER_H_

#include <string>
#include <vector>
using namespace std;

#include "main/XmdvTool.h"

class VisualMapResult;
class XmdvToolMainWnd;
class Pipeline;
class Operator;
class Data;
class ViewManager;
class Transformation;
class ScatterVisAttr;

class PipelineManager {
	friend class ViewManager;

public:
	PipelineManager(XmdvToolMainWnd* wnd);
	virtual ~PipelineManager();

private:
	XmdvToolMainWnd *mainWnd;
	// All the original datasets
	// This vector is only used to release memory.
	vector<Data*> m_origDatasets;
	// All pipelines
	vector<Pipeline*> m_pipelines;
	// All the pipeline outputs,
	// the same size as m_pipelines
	// 1-1 mapping between m_pipelines and m_visualMapResults
	vector<VisualMapResult*> m_visualMapResults;
	// all of queries in all pipelines
	// This vector is only used to release memory.
	// Since pipelines or transformations might share queries,
	// it is impossible to release memory in their destructor function
	vector<Operator*> m_allOperators;
	// all assistant input data
	// This vector is only used to release memory.
	vector<Data*> m_allAssisInputData;

public:
	void setMainWnd(XmdvToolMainWnd* wnd);
	XmdvToolMainWnd *getMainWnd();

	ViewManager* getViewManager();

	// return a pipeline specified by pipeline ID
	Pipeline* getPipeline(int pID);
	// Return the pipeline ID based on the input of a pointer to a pipeline
	int getPipelineID(Pipeline* pl);
	// return the number of all pipelines, including the closed ones
	int getPipelinesCount();
	// Delete the pipeline specified by the pID
	void deletePipeline(int pID);
	// Return the vector containing all piplines associated with a data
	vector<int> getPipelinesByData(Data* origData);

	// return the ID for the pipeline corresponding to the active window
	int getActivePipelineID ();

	// return the pipeline corresponding to the active window
	Pipeline* getActivePipeline();

	// open an okc file and display it in a new window
	// if needRefresh=true, the GLDisplay will be enforced to
	// update.  If users use the open file dialog to open the file,
	// needRefresh should be set to false to save time cost,
	// because the GLDisplay will be updated automatically
	// because of the open file dialog
	void openFile(QString fileName, bool needRefresh, XmdvTool::VISTYPE visType);

	// Create a pipelines for hierarchical display using structure-based brush.
	// Parameters:
	// 		origData: the pointer to the OkcData corresponding to the input dataset
	// Return:
	//		the pipeline ID corresponding to the new pipeline.
	int createSBBPipeline(OkcData* origData);

	// Create a pipelines for hierarchical display using structure-based brush.
	// Parameters:
	// 		origData: the pointer to the OkcData corresponding to the input dataset
	// Return:
	//		the pipeline ID corresponding to the new pipeline.
	int createDimRPipeline(OkcData* okcdata);

	// Assemble all pipelines in the vector m_pipelines
	// The results are saved in the vector m_assembledResults
	void assembleAll();

	// Assemble the pipeline  m_pipelines[i] starting
	// from a specified operator;
	// If startOperatorNo=-1, only visualMap will be done.
	// The results are saved in vector m_assembledResults
	// This function assume that the vector m_assembledResults
	// has been initialized to the same size as
	// the vector m_assembledResults
	void assemble(int i, int startOperatorNo);

	// Assemble the pipeline  m_pipelines[i] starting
	// from a specified operator;
	// if operator=NULL, only visualMap will be done
	void assemble(int i, Operator* oper);

	// Reversely traverse the assistant transformation
	// to update the input.
	// pID: pipeline ID
	// output: the input of reverse transformation
	// type: the assistant data type
	void updateAssisInput(int pID, Data* output, XmdvTool::ASSISDATATYPE type);

	// Get the assembled result m_assembledResults[i].
	// The results are saved in the vector m_assembledResults
	VisualMapResult* getVisualMapResult(int i);
	// Get the assembled result for a specific pipeline
	VisualMapResult* getVisualMapResult(Pipeline* pl);

	// add a data to m_origDatasets
	void addOrigDatasets(Data* data);

	// Return the number of all datasets (including opened and closed)
	int getOrigDatasetsCount();
	// Delete the input dataset
	void deleteOrigDataset(Data* origData);

	// Return the pointer to the data with index i.
	// If the return value is 0, it means this data has been closed
	Data* getOrigDataset(int index);

	// add a operator to m_allOperators
	void addOperator(Operator* oper);
	// This function will try to delete operators used by the pipeline pID.
	// An operator of pID will be deleted only if this operator is not used
	// by any other pipelines.
	void tryDeleteOperators(int pID);
	// This function will try to delete operators used by the Transformation t.
	// An operator of this transformation will be deleted only if this operator
	// is not used by any other pipelines.
	void tryDeleteOperators(int pID, Transformation* t);
	// Check whether the operator is used by any pipelines other than the pipeline pID
	bool operatorUsedByOtherPipelines(Operator* op, int pID);

	// add a data to m_allAssisInputData
	void addAssisInputData(Data* data);

	// for memory clean up;
private:
	ScatterVisAttr* scatterVisAttr;
};

#endif /*PIPELINEMANAGER_H_*/
