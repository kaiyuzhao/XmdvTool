/*
 * PCADeriveOperator.cpp
 *
 *  Created on: Sep 2, 2009
 *      Author: Zaixian Xie
 */

#include "operator/pcaderive/PCADeriveOperator.h"

#include <typeinfo>

#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierPCADerive.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"
#include <vector>
#include "util/algo/pca/PCA.h"

PCADeriveOperator::PCADeriveOperator() : Operator() {
}

PCADeriveOperator::~PCADeriveOperator() {
}

void PCADeriveOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(OkcData));
	Operator::setInput(input);
}

void PCADeriveOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
}

bool PCADeriveOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
	OkcData* input = (OkcData*)m_input;

	// Create a new instance of class OkcData as output
	// or use the old one
	OkcData* output;
	SAFE_DELETE(m_output);
	output = new OkcData(input);
	m_output = dynamic_cast<Data*>(output);

	OkcDataModifierManager* manager = output->getOkcDataModifierManager();
	OkcDataModifier*  modifier =
		manager->getOkcDataModifier(XmdvTool::MODIFIER_PCADERIVE);
	OkcDataModifierPCADerive* modifierPCA;
	if (modifier) {
		// the modifier does exist
		modifierPCA = dynamic_cast<OkcDataModifierPCADerive*>(modifier);
	} else {
		// the modifier does not exist
		modifierPCA = new OkcDataModifierPCADerive(manager->getOkcData());
		manager->addOkcDataModifier
			(XmdvTool::MODIFIER_PCADERIVE, modifierPCA);
	}

	// do principal component analysis
	bool success = doPCA(input, modifierPCA);
	return success;
}

void PCADeriveOperator::postOperator() {
}

bool PCADeriveOperator::doPCA(OkcData* okcdata, OkcDataModifierPCADerive* mod) {
	int n, m;
	double **data;

	n = okcdata->getDataSize();
	m = okcdata->getDimSize();

	// The function PCA::matrix will request memory and
	// make data be a 2d array
	data = PCA::matrix(n, m);
	// Read the data from the okcdata and organize them into a 2d array
	int i, j;
	for (i = 0; i < n; i++ )
	{
	    std::vector<double> datapoint;
	    okcdata->getData(datapoint, i);
		for (j = 0; j < m; j++ )
		{
			data[i][j] = datapoint[j];
		}
	}

	// Initialize the array to store the principal components
	mod->initPrinArr();
	// Call PCA algorithm to do principal component analysis
    bool success = PCA::principal_Components_Analysis(n, m, data, mod->prin_num,
    		mod->principal, mod->principal_max, mod->principal_min);

    // Call PCA::free_matrix() to release memory occupied by the 2d array data.
	PCA::free_matrix(data, n, m);

	return success;
}
