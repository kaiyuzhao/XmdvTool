/*
 * VisualAttribute.h
 *
 *  Created on: Sep 24, 2008
 *      Author: zyguo
 */

#ifndef VISUALATTRIBUTE_H_
#define VISUALATTRIBUTE_H_

#include "datatype/RGBt.h"
#include <vector>


class VisualAttribute {
public:
	VisualAttribute();
	VisualAttribute(bool brushFlag);
	virtual ~VisualAttribute();

	VisualAttribute(const VisualAttribute&);

public:
	// the color for data item
	RGBt color;
	// the color for band
	RGBt bandColor;
	bool inBrush;
	//int parcoordLineWidth;
	//int scaterPlotSize;
	//std::vector<int> glyphLineWidth;

	//std::vector<RGBt> quaColor;
	//std::vector<int> quaLineWidth;
	//std::vector<int> quaBrightness;

};


#endif /* VISUALATTRIBUTE_H_ */
