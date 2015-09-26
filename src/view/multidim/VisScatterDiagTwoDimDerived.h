/*
 * VisScatterDiagTwoDimDerived.h
 *
 *  Created on: Aug 21, 2009
 *      Author: zyguo
 */

#ifndef VISSCATTERDIAGTWODIMDERIVED_H_
#define VISSCATTERDIAGTWODIMDERIVED_H_

#include "view/multidim/VisScatterDiag.h"

#include "data/multidim/OkcDataModifierPCADerive.h"

class VisScatterDiagTwoDimDerived : public VisScatterDiag {
public:
	VisScatterDiagTwoDimDerived(VisScatter* _visScatter);
	virtual ~VisScatterDiagTwoDimDerived();

	void DrawScatters();

	double XMAP(int srnDim, int valDim, double VAL);
	double YMAP(int srnDim, int valDim, double VAL);
	OkcDataModifierPCADerive* getModifierPCA();
};

#endif /* VISSCATTERDIAGTWODIMDERIVED_H_ */
