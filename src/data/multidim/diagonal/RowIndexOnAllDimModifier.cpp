/*
 * RowIndexOnAllDimModifier.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: zyguo
 */

#include "RowIndexOnAllDimModifier.h"
#include "data/multidim/OkcData.h"

#include "datatype/VectorUtil.h"

RowIndexOnAllDimModifier::RowIndexOnAllDimModifier() {
}

RowIndexOnAllDimModifier::RowIndexOnAllDimModifier(OkcData* okcdata) :
	OkcDataModifier(okcdata) {
}

RowIndexOnAllDimModifier::~RowIndexOnAllDimModifier() {
}

void RowIndexOnAllDimModifier::initRequiredArr(){

	OkcData* okcdata = getOkcData();

	int dataSize = okcdata->getDataSize();
	int dimSize = okcdata->getDimSize();

	VectorUtil::init2DVector(sortOrderOnAllDim, dimSize + 1, dataSize);

}

void RowIndexOnAllDimModifier::copyFrom(const RowIndexOnAllDimModifier* copy) {
	OkcDataModifier::copyFrom((OkcDataModifier*)copy);

	initRequiredArr();

	OkcData* okcdata = getOkcData();

	int dataSize = okcdata->getDataSize();
	int dimSize = okcdata->getDimSize();

	for (int i = 0; i < dimSize + 1; i++) {
		for (int j = 0; j < dataSize; j++) {
			this->sortOrderOnAllDim[i][j] = copy->sortOrderOnAllDim[i][j];
		}
	}
}

void RowIndexOnAllDimModifier::operator=(const RowIndexOnAllDimModifier &copy) {
	copyFrom(&copy);
}
