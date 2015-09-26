/*
 * VisScatterDiagOneDim.h
 *
 *  Created on: Aug 3, 2009
 *      Author: zyguo
 */

#ifndef VISSCATTERDIAGONEDIM_H_
#define VISSCATTERDIAGONEDIM_H_

#include "view/multidim/VisScatterDiag.h"

class VisScatterDiagOneDim : public VisScatterDiag {
public:
	VisScatterDiagOneDim();
	VisScatterDiagOneDim(VisScatter* _visScatter);
	virtual ~VisScatterDiagOneDim();

	void DrawScatters();

	void DrawSingleBrush();

	double OrderMAP(int DIM, int order);
	int OrderINVMAP(int DIM, double VAL);


};

#endif /* VISSCATTERDIAGONEDIM_H_ */
