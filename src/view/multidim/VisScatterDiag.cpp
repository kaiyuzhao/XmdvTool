/*
 * VisScatterDiag.cpp
 *
 *  Created on: Aug 1, 2009
 *      Author: zyguo
 */

#include "VisScatterDiag.h"

VisScatterDiag::VisScatterDiag() {

}

VisScatterDiag::VisScatterDiag(VisScatter* _visScatter) {
	visScatter = _visScatter;
}

VisScatterDiag::~VisScatterDiag() {
}


void VisScatterDiag::DrawScatters(){

}

GLCanvas* VisScatterDiag::GetCanvas(){
	return visScatter->m_canvas;
}

int VisScatterDiag::getDataSize(){
	return visScatter->m_data_size;
}

int VisScatterDiag::getDimSize(){
	return visScatter->m_dim_size;
}
