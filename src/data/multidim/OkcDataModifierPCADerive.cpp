/*
 * OkcDataModifierPCADerive.cpp
 *
 *  Created on: Sep 2, 2009
 *      Author: xiezx
 */

#include "data/multidim/OkcDataModifierPCADerive.h"
#include "data/multidim/OkcDataModifier.h"
#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"

OkcDataModifierPCADerive::OkcDataModifierPCADerive() {
	resetPrinArr();
}

OkcDataModifierPCADerive::OkcDataModifierPCADerive(OkcData* okcdata) :
	OkcDataModifier(okcdata) {
	resetPrinArr();
}

OkcDataModifierPCADerive::~OkcDataModifierPCADerive() {
	// release the memory held by principal components
	int i;
	if (prin_num!=0) {
		for (i=0; i<prin_num; i++) {
			SAFE_DELETE_ARR(principal[i]);
		}
		SAFE_DELETE_ARR(principal_max);
		SAFE_DELETE_ARR(principal_min);
	}
}

XmdvTool::MODIFIERTYPE OkcDataModifierPCADerive::getModifierType() {
	return XmdvTool::MODIFIER_PCADERIVE;
}

void OkcDataModifierPCADerive::resetPrinArr() {
	prin_num = 0;
	principal = 0;
	principal_max = 0;
	principal_min = 0;
}

void OkcDataModifierPCADerive::initPrinArr() {
	int i, dim_size, data_size;
	OkcData* okcdata = getOkcData();
	dim_size = okcdata->getDimSize();
	data_size = okcdata->getDataSize();
	if (  dim_size < 3 )
		prin_num = dim_size;
	else
		prin_num = 3;
	arr_len = data_size;

	principal = new double *[prin_num];
	for ( i = 0; i < prin_num; i++ )
		principal[i] = new double[data_size];
	principal_max = new double[prin_num];
	principal_min = new double[prin_num];
}


void OkcDataModifierPCADerive::copyFrom(const OkcDataModifierPCADerive* copy) {
	int i,j;
	OkcDataModifier::copyFrom((OkcDataModifier*)copy);

	// copy the prin_num and arr_len
	prin_num = copy->prin_num;
	arr_len = copy->arr_len;

	// copy the principal
	principal = new double *[prin_num];
	for ( i = 0; i < prin_num; i++ ) {
		principal[i] = new double[arr_len];
		for (j=0; j<arr_len; j++) {
			principal[i][j] = copy->principal[i][j];
		}
	}

	// copy the principal_max and principal_min
	principal_max = new double[prin_num];
	principal_min = new double[prin_num];

	for (i=0; i<prin_num; i++) {
		principal_max[i] = copy->principal_max[i];
		principal_min[i] = copy->principal_min[i];
	}
}

void OkcDataModifierPCADerive::operator=(const OkcDataModifierPCADerive &copy) {
	copyFrom(&copy);
}

void OkcDataModifierPCADerive::getOneDimPrincipal(std::vector<double> &data, int dimIdx) {
	int i;
	data.resize(arr_len);

	for (i = 0; i < arr_len; i++)
		data[i] = principal[dimIdx][i];

}
