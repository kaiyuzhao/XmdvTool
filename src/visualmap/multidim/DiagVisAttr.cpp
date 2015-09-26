/*
 * DiagVisAttr.cpp
 *
 *  Created on: Aug 14, 2009
 *      Author: zyguo
 */

#include "visualmap/multidim/DiagVisAttr.h"

#include <QMessageBox>

#include "data/multidim/OkcData.h"

#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "color/ColorManager.h"
#include "data/multidim/OkcDataModifierHighlight.h"

class OkcData;

DiagVisAttr::DiagVisAttr() {
}

DiagVisAttr::~DiagVisAttr() {
}

void DiagVisAttr::setOkcData(OkcData* okcdata) {
	m_okcData = okcdata;
}

OkcData* DiagVisAttr::getOkcData(){
	return m_okcData;
}

void DiagVisAttr::setOkcVisualMapResult(OkcVisualMapResult* _okcVMR) {
	okcVMR = _okcVMR;
}

OkcVisualMapResult* DiagVisAttr::getOkcVisualMapResult(){
	return this->okcVMR;
}



void DiagVisAttr::diagColorMapping(){
	OkcData* input = getOkcData();
	int dataSize = input->getDataSize();
	int dimSize = input->getOrigDimSize();



	initVisAttrArray();

	VisualAttribute* inBrush = okcVMR->getInBrushVisAttr();
	VisualAttribute* outBrush = okcVMR->getOutBrushVisAttr();

	OkcDataModifierManager* manager = input->getOkcDataModifierManager();
	OkcDataModifier* modifier = manager->getOkcDataModifier(
			XmdvTool::MODIFIER_HIGHLIGHT);

	ColorManager *cm = g_globals.colorManager;
	VisualAttribute* newAttr;

	float index[dataSize];

	for (int dim = 0; dim < dimSize; dim++) {
		for (int i = 0; i < dataSize; i++) {
			index[i] = float(((*input->data_buf)[manager->getOrigLine(i)
			                                     * input->getOrigDimSize() + dim]
			                                     - input->dim_min[dim]) / (input->dim_max[dim]
			                                                                              - input->dim_min[dim]));
		}
		if (modifier) {
			// this okcdata has the highlight modifier
			OkcDataModifierHighlight* modifierHighlight =
				dynamic_cast<OkcDataModifierHighlight*> (modifier);
				for (int i = 0; i <dataSize; i++) {
					if (modifierHighlight->getHighlight(manager->getOrigLine(i))) {
						newAttr = new VisualAttribute(*inBrush);
						newAttr->color = cm->getColor(
								ColorManager::BRUSH_SELECTED1, index[i]);
						okcVMR->setDiagMultiDimDataVisAttr(newAttr, dim, i);
					} else {
						newAttr = new VisualAttribute(*outBrush);
						newAttr->color = cm->getColor(
								ColorManager::BRUSH_UNSELECTED, index[i]);
						okcVMR->setDiagMultiDimDataVisAttr(newAttr, dim, i);
					}
				}
		} else {
			for (int i = 0; i < dataSize; i++) {
				newAttr = new VisualAttribute(*outBrush);
				newAttr->color = cm->getColor(ColorManager::BRUSH_UNSELECTED,
						index[i]);
				okcVMR->setDiagMultiDimDataVisAttr(newAttr, dim, i);
			}
		}
	}

}

void DiagVisAttr::initVisAttrArray(){
	OkcData* input = getOkcData();
	int dataSize = input->getDataSize();
	int dimSize = input->getOrigDimSize();

	multi_dim_data_VisAttr.resize(dimSize);
	for (int i = 0; i < dimSize; i++) {
		multi_dim_data_VisAttr[i].resize(dataSize);
	}
}
