/*
 * VisScatterDiag.h
 *
 *  Created on: Aug 1, 2009
 *      Author: zyguo
 */

#ifndef VISSCATTERDIAG_H_
#define VISSCATTERDIAG_H_


#include "view/multidim/VisScatter.h"

class VisScatter;

class VisScatterDiag {
public:
	VisScatterDiag();
	VisScatterDiag(VisScatter* _visScatter);
	virtual ~VisScatterDiag();

	VisScatter* visScatter;

	virtual void DrawScatters();
	GLCanvas* GetCanvas();

	int getDataSize();
	int getDimSize();




};

#endif /* VISSCATTERDIAG_H_ */
