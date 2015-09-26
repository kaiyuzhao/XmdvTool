/*
 * VisScatterDiagTwoDim.h
 *
 *  Created on: Aug 8, 2009
 *      Author: zyguo
 */

#ifndef VISSCATTERDIAGTWODIM_H_
#define VISSCATTERDIAGTWODIM_H_

#include "view/multidim/VisScatterDiag.h"

class VisScatterDiagTwoDim : public VisScatterDiag {
public:
	VisScatterDiagTwoDim(VisScatter* _visScatter);
	virtual ~VisScatterDiagTwoDim();
	void DrawScatters();

protected:
	int resize_x_from, resize_x_to;
	int resize_y_from, resize_y_to;
	int columns, rows;

public:
	void CalculateRowsColumns();
	double XOrderMAP(int DIM, int order);
	double XOrderMAP2(int DIM, int order);
	int XOrderINVMAP(int DIM, double VAL);
	double YOrderMAP(int DIM, int order);
	double YOrderMAP2(int DIM, int order);
	//reverse of xmap. map screen back to data
	int YOrderINVMAP(int DIM, double VAL);
};

#endif /* VISSCATTERDIAGTWODIM_H_ */
