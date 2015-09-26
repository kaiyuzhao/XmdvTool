/*
 * ScatterVisAttr.h
 *
 *  Created on: Aug 18, 2009
 *      Author: zyguo
 */

#ifndef SCATTERVISATTR_H_
#define SCATTERVISATTR_H_

#include "main/XmdvTool.h"

class ScatterVisAttr {
public:
	ScatterVisAttr();
	virtual ~ScatterVisAttr();

	 int size;

	// shape = 0: square
	// shape = 1: circle
	int shape;

	// the display type of diagonal display
	// 0: original; 1: histogram; 2: 1-D display; 3: 2-D display
	XmdvTool::DIAGPLOT_MODE diagType;

	// for diagonal 2-D display, the selected data driven x and y dimension
	int dataDrivenXDimNumber;
	int dataDrivenYDimNumber;

	// The X and Y dimension number for derived mode
	int derivedXDimNumber;
	int derivedYDimNumber;

	// for one-dimensional display, the data are sorted by which dimension(DimSortDim)
	// 0 is for original order
	int oneDimSortDim;

	void setPointSize(int _size);
	int getPointSize();

	void setShape(int _shape);
	int getShape();

	void setDiagType(XmdvTool::DIAGPLOT_MODE _diagType);
	XmdvTool::DIAGPLOT_MODE getDiagType();

	void setDataDrivenXDimNumber(int _dataDrivenXDimNumber);
	int getDataDrivenXDimNumber();

	void setDataDrivenYDimNumber(int _dataDrivenYDimNumber);
	int getDataDrivenYDimNumber();

	void setDerivedXDimNumber(int _derivedXDimNumber);
	int getDerivedXDimNumber();

	void setDerivedYDimNumber(int _derivedYDimNumber);
	int getDerivedYDimNumber();

	void setOneDimSortDim(int sortDim);
	int getOneDimSortDim();


};

#endif /* SCATTERVISATTR_H_ */
