/*
 * VisualAttribute.cpp
 *
 *  Created on: Sep 23, 2008
 *      Author: zyguo
 */

#include "VisualAttribute.h"

VisualAttribute::VisualAttribute() {
}

VisualAttribute::VisualAttribute(bool brushFlag) {
	inBrush = brushFlag;
}

VisualAttribute::~VisualAttribute() {
}

VisualAttribute::VisualAttribute(const VisualAttribute& va) {
	color = va.color;
	bandColor = va.bandColor;
	inBrush = va.inBrush;
}
