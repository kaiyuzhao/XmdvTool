/*
 * BrushOperator.cpp
 *
 *  Created on: Jan 14, 2009
 *      Author: Zaixian Xie
 */

#include "main/XmdvTool.h"
#include "operator/brush/BrushOperator.h"
#include "operator/brush/Brush.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/OkcDataModifierHighlight.h"
#include <typeinfo>
using namespace std;

BrushOperator::BrushOperator() : Operator() {
	m_highlightedCount = 0;
}

BrushOperator::~BrushOperator() {
}

void BrushOperator::setBrushStorage(OkcData* brushStorage){
	this->m_brushStorage = brushStorage;
}

// Get the brush
OkcData* BrushOperator::getBrushStorage() {
	return m_brushStorage;
}

void BrushOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(OkcData));
	Operator::setInput(input);
}

void BrushOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
}

bool BrushOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
	OkcData* input = (OkcData*)m_input;

	// Create a new instance of class OkcData as output
	// or use the old one
	OkcData* output;
	SAFE_DELETE(m_output);
	output = new OkcData(input);
	m_output = dynamic_cast<Data*>(output);

	OkcDataModifierManager* outputManager = output->getOkcDataModifierManager();
	OkcDataModifier*  modifier =
		outputManager->getOkcDataModifier(XmdvTool::MODIFIER_HIGHLIGHT);
	OkcDataModifierHighlight* modifierHighlight;
	if (modifier) {
		// the modifier does exist
		modifierHighlight = dynamic_cast<OkcDataModifierHighlight*>(modifier);
	} else {
		// the modifier does not exist
		modifierHighlight = new OkcDataModifierHighlight(outputManager->getOkcData());
		outputManager->addOkcDataModifier
			(XmdvTool::MODIFIER_HIGHLIGHT, modifierHighlight);
	}
	modifierHighlight->initHighlightArr();
	int origDataSize = input->getOrigDataSize();
	int dimSize = input->getDimSize();
	int i,j;
	std::vector <double> curData(dimSize);
	Brush brush;
	brush.copyFromOkcDataStorage(getBrushStorage());

	// The brush operator stores all brush flag (OkcDataModifierHighlight::m_highlighted)
	// in the original order of the datapoints.  This means that it will ignore all sorting order
	// or sampling represented by OkcDataModifierRowIndex.  Thus, we need to set the original order
	// for each datapoint when setting the highlight array.
	OkcDataModifierManager* inputManager = input->getOkcDataModifierManager();
	m_highlightedCount = 0;
	for (i=0; i<origDataSize; i++){
		bool highlight = true;
		input->getData(curData,i);
		for (j=0; j<dimSize; j++){
			if (curData[j]>brush.max[j]||curData[j]<brush.min[j]) {
				highlight = false;
				break;
			}
		}
		if (highlight) {
			m_highlightedCount++;
		}
		modifierHighlight->setHighlight(inputManager->getOrigLine(i), highlight);
	}

	return true;
}

void BrushOperator::postOperator() {
}

int BrushOperator::getighlightedCount() {
	return m_highlightedCount;
}

