/*
 * HistogramOperator.h
 *
 *  Created on: Aug 14, 2009
 *      Author: zyguo
 */

#ifndef HISTOGRAMOPERATOR_H_
#define HISTOGRAMOPERATOR_H_

#include "operator/Operator.h"
#include "main/XmdvTool.h"
#include <vector>

class OkcData;
class HistogramOperator;
class Operator;
class HistogramModifier;

class HistogramOperator : public Operator {
public:
	HistogramOperator();
	virtual ~HistogramOperator();

	void setHistogramSize(int _histogram_size);

private:
	int histogram_size;

	// the frequency for each histogram bin for all data points
	std::vector< std::vector <int> >  frequency;
	// the frequency for each histogram bin for brushed data points
	std::vector< std::vector <int> > marked_freq;
	// the cut points for each histogram bin
	std::vector< std::vector <double> > histo_cut_points;
	// the maximum frequency for each dimension
	std::vector<int> max_frequency;


public:
	// The function setInput first do type verification
	// and then call the base class
	void setInput(Data* input);
	// Initialize histogram operator
	void preOperator();
	// calculate the frequency in each histogram bin both for brushed and unbrushed data
	bool doOperator();

	void initRequiredArr();

	void Calculate_Histogram_Data();
	void Calculate_Histogram_Cutpoint();
	void getDimCutpoints(std::vector<double> &cutpoints, int dimIdx);
	void getBinIdxForAllDim(std::vector<double> &binIdx, int dataIdx);
	void Calculate_Frequency();
	void setARRZero(std::vector< std::vector<int> > &array);
	void setARRZero(std::vector<int> &array);
	void setHistogramModifierArray(HistogramModifier* histogramModifier);
	void clearARRARR(std::vector< std::vector<int> > &array);
	void clearARRARR(std::vector< std::vector<double> > &array);
	//std::vector< ARR <int> >  copyARR_ARR_INT(std::vector< ARR <int> > array);
};

#endif /* HISTOGRAMOPERATOR_H_ */
