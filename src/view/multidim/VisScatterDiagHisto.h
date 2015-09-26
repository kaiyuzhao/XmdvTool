/*
 * VisScatterDiagHisto.h
 *
 *  Created on: Aug 1, 2009
 *      Author: zyguo
 */

#ifndef VISSCATTERDIAGHISTO_H_
#define VISSCATTERDIAGHISTO_H_

#include "view/multidim/VisScatterDiag.h"


class VisScatterDiagHisto : public VisScatterDiag{
public:
	VisScatterDiagHisto(VisScatter* _visScatter);
	virtual ~VisScatterDiagHisto();

	/*std::vector<double> histo_cut_points;
	std::vector<int> frequency;
	std::vector<int> marked_freq;

	std::vector<int> max_frequency;*/

	void DrawScatters();
	void DrawSingleBrush();

	/*void Calculate_Histogram_Cutpoint();

	void getDimCutpoints(std::vector<double> &cutpoints, int dimIdx);

	void CalculateHistogramData();
	void getBinIdxForAllDim(std::vector<double> &binIdx, int dataIdx);


	void GetFrequency(std::vector<int> &fre, std::vector<int> &mfre, int &min, int &max, int dim);

	void setARRZero(std::vector<int> &array);*/

};

#endif /* VISSCATTERDIAGHISTO_H_ */
