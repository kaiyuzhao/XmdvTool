/*
 * DimControl.h
 *
 *  Created on: Nov 25, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class DimControl contains some static functions shared by
 * classes DimManualControl and DimAutoReorderControl
 */
#ifndef DIMCONTROL_H_
#define DIMCONTROL_H_

#include <vector>

class OkcData;
class Pipeline;
class DimOOOMap;
class DimOOOOperator;
class PipelineManager;
class XmdvToolMainWnd;
class ViewWindow;

class DimControl {
public:
	DimControl();
	virtual ~DimControl();

public:
	// Return an OkcData, which is the input of dimension on/off/reorder operator
	static OkcData* getInputOkcData(Pipeline* pl);
	// Return the pointer to the DimOOOMap associated with a specific pipeline
	static DimOOOMap* getDimOOOMap(Pipeline* pl);
	// Return the pointer to the dimension on/off/reorder operator
	// associated with a specific pipeline
	static DimOOOOperator* getDimOOOOperator(Pipeline* pl);
	// Reassemble the pipeline from the dimension on/off/reorder operator
	static void updateDisplay(XmdvToolMainWnd* mainWnd, PipelineManager* pm, int pID, ViewWindow *win);
	// Get the dimension names and assign it to dim_names
	static void getDimNames(Pipeline* pl, std::vector<char*>& dim_names);
};

#endif /* DIMCONTROL_H_ */
