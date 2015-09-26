/*
 * HistogramModifier.cpp
 *
 *  Created on: Aug 14, 2009
 *      Author: zyguo
 */

#include "HistogramModifier.h"
#include "data/multidim/OkcData.h"
#include "datatype/VectorUtil.h"

HistogramModifier::HistogramModifier() {
}

HistogramModifier::HistogramModifier(OkcData* okcdata) :
	OkcDataModifier(okcdata) {

}

HistogramModifier::~HistogramModifier() {
}


void HistogramModifier::initRequiredArr(int histogramSize){

	this->histogram_size = histogramSize;
	OkcData* okcdata = getOkcData();

	int dimSize = okcdata->getDimSize();

	VectorUtil::init2DVector(frequency, dimSize, histogram_size);
	VectorUtil::init2DVector(marked_freq, dimSize, histogram_size);
	VectorUtil::init2DVector(histo_cut_points, dimSize, histogram_size + 1);
	max_frequency.assign( dimSize, 0 );

}

void HistogramModifier::copyFrom(const HistogramModifier* copy) {
	OkcDataModifier::copyFrom((OkcDataModifier*)copy);
	this->histogram_size = copy->histogram_size;
	initRequiredArr(histogram_size);

	OkcData* okcdata = getOkcData();

	int dimSize = okcdata->getDimSize();

	for (int i = 0; i < dimSize; i++) {
		for (int j = 0; j < histogram_size; j++) {
			this->frequency[i][j] = copy->frequency[i][j];
		}
	}
	for (int i = 0; i < dimSize; i++) {
		for (int j = 0; j < histogram_size; j++) {
			this->marked_freq[i][j] = copy->marked_freq[i][j];
		}
	}
	for (int i = 0; i < dimSize; i++) {
		for (int j = 0; j < histogram_size + 1; j++) {
			this->histo_cut_points[i][j]
					= copy->histo_cut_points[i][j];
		}
	}
	for (int i = 0; i < dimSize; i++) {
		this->max_frequency[i] = copy->max_frequency[i];
	}

	/*for (int i = 0; i < dimSize; i++) {
		for (int j = 0; j < histogram_size; j++) {
			printf("frequency[%d][%d]=%d\n", i, j, frequency[i][j]);
		}
	}*/

}

void HistogramModifier::operator=(const HistogramModifier &copy) {
	copyFrom(&copy);
}
