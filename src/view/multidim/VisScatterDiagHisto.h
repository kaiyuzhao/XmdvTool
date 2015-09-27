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

	void DrawScatters();
	void DrawSingleBrush();

};

#endif /* VISSCATTERDIAGHISTO_H_ */
