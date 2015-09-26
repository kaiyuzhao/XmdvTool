/*
 * HistogramModifier.h
 *
 *  Created on: Aug 14, 2009
 *      Author: zyguo
 */

#ifndef HISTOGRAMMODIFIER_H_
#define HISTOGRAMMODIFIER_H_

#include "data/multidim/OkcDataModifier.h"
#include "main/XmdvTool.h"
#include <vector>

class HistogramModifier : public OkcDataModifier{
	friend class HistogramOperator;
	friend class OkcDataModifierManager;
public:
	HistogramModifier();
	HistogramModifier(OkcData* okcdata);
	virtual ~HistogramModifier();

	void initRequiredArr(int histogramSize);

	void copyFrom(const HistogramModifier* copy);
	// overloading the operator "="
	void operator=(const HistogramModifier &copy);

public:

	int histogram_size;

	// the frequency for each histogram bin for all data points
	std::vector< std::vector <int> >  frequency;
	// the frequency for each histogram bin for brushed data points
	std::vector< std::vector <int> > marked_freq;
	// the cut points for each histogram bin
	std::vector< std::vector <double> > histo_cut_points;
	// the maximum frequency for each dimension
	std::vector<int> max_frequency;
};

#endif /* HISTOGRAMMODIFIER_H_ */
