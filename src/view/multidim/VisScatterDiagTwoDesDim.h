/*
 * VisScatterDiagTwoDesDim.h
 *
 *  Created on: Aug 8, 2009
 *      Author: zyguo
 */

#ifndef VISSCATTERDIAGTWODESDIM_H_
#define VISSCATTERDIAGTWODESDIM_H_

#include "view/multidim/VisScatterDiag.h"

class VisScatterDiagTwoDesDim : public VisScatterDiag {
public:
	VisScatterDiagTwoDesDim(VisScatter* _visScatter);
	virtual ~VisScatterDiagTwoDesDim();

	void DrawScatters();

	double XMAP(int srnDim, int valDim, double VAL);
	double YMAP(int srnDim, int valDim, double VAL);
	double XINVMAP(int srnDim, int valDim, double VAL);
	//reverse of xmap. map screen back to data
	double YINVMAP(int srnDim, int valDim, double VAL);

};

#endif /* VISSCATTERDIAGTWODESDIM_H_ */
