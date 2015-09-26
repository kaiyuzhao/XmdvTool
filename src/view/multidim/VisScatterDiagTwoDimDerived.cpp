/*
 * VisScatterDiagTwoDimDerived.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: zyguo
 */

#include "VisScatterDiagTwoDimDerived.h"

#include "view/multidim/VisScatter.h"
#include "view/ViewDisplay.h"
#include "data/multidim/OkcData.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/GLCanvas.h"
#include "visualmap/VisualAttribute.h"
#include "color/ColorManager.h"
#include "operator/brush/Brush.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/OkcDataModifierPCADerive.h"
#include "data/multidim/OkcDataModifierManager.h"

VisScatterDiagTwoDimDerived::VisScatterDiagTwoDimDerived(VisScatter* _visScatter)
: VisScatterDiag(_visScatter) {
}

VisScatterDiagTwoDimDerived::~VisScatterDiagTwoDimDerived() {
}

void VisScatterDiagTwoDimDerived::DrawScatters(){
	OkcDataModifierPCADerive* mod = getModifierPCA();

	int data_size = getDataSize();

	int cox = this->visScatter->getDerivedXDimNumber();
	int coy = this->visScatter->getDerivedYDimNumber();

	std::vector<double> data(data_size);

	std::vector<double> locationx(data_size);
	std::vector<double> locationy(data_size);

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;


	OkcData* okcdata = visScatter->getDisplayDataset();
	OkcVisualMapResult* okcVMR = visScatter->getOkcVisualMapResult();
	GLCanvas* canvas = GetCanvas();

	std::vector<int> temp_buf, buf;
	int all_dims = okcdata->getOrigDimSize();
	temp_buf.resize(all_dims);
	for(int i=0; i<all_dims; i++){
		temp_buf[i] = i;
	}
	OkcDataModifierManager* okcDataModifierManager = okcdata->getOkcDataModifierManager();
	okcDataModifierManager->mapData(temp_buf, buf);
	int dim_size = buf.size();

	if(cox>2){
		okcdata->GetDimensionData(locationx, buf[cox-3]);
	}else{
		mod->getOneDimPrincipal(locationx, cox);
	}
	if(coy>2){
		okcdata->GetDimensionData(locationy,  buf[coy-3]);
	}else{
		mod->getOneDimPrincipal(locationy, coy);
	}
	int shape = visScatter->getShapeType();
	int size = visScatter->getPointSize();

	int width, height;
	width = XmdvTool::SUBWIN_WIDTH;
	height = XmdvTool::SUBWIN_HEIGHT;
	double radiusW = (double) size / (4.0 * width);
	double radiusH = (double) size / (4.0 * height);


	for (int k = 0; k < dim_size; k++) {
		int i = buf[k];
		okcdata->GetDimensionData(data, i);
		double x1, y1;
		for (int j = 0; j < data_size; j++) {

			VisualAttribute* curAttr = okcVMR->getDiagMultiDimDataVisAttr(i, j);
			canvas->setForeground(curAttr->color);
			x1 = XMAP(k, cox, locationx[j]);
			y1 = YMAP(k, coy, locationy[j]);

			if (shape == 0) {
				glBegin(GL_POINTS);
				glPointSize((float) size);
				glVertex2f(x1, y1);
				glEnd();
			} else if (shape == 1) {
				canvas->drawCircle(x1, y1, radiusW, radiusH);
			}
		}
	}
}

OkcDataModifierPCADerive* VisScatterDiagTwoDimDerived::getModifierPCA(){
	OkcData*okcData =  this->visScatter->getDisplayDataset();
	OkcDataModifierManager* okcDataModifierManager = okcData->getOkcDataModifierManager();
	OkcDataModifierPCADerive* mod = dynamic_cast<OkcDataModifierPCADerive*>
		(okcDataModifierManager->getOkcDataModifier(XmdvTool::MODIFIER_PCADERIVE));
	return mod;
}

//Use scrDim as the screen dimension, valDim as value Dimension, and VAL as value
double VisScatterDiagTwoDimDerived::XMAP(int srnDim, int valDim, double VAL)
{
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;

	OkcDataModifierPCADerive* mod = getModifierPCA();
	if(valDim>2){
		valDim = valDim - 3;
		return scatter_X_axes[srnDim] +
		      ( VAL -dim_min[valDim] ) * ( scatter_X_axes[srnDim+1] -scatter_X_axes[srnDim] )
		         / (dim_max[valDim] - dim_min[valDim] );
	}else{
		return scatter_X_axes[srnDim] +
			( VAL - mod->principal_min[valDim] ) * ( scatter_X_axes[srnDim+1] -scatter_X_axes[srnDim] )
				      / (mod->principal_max[valDim] - mod->principal_min[valDim] );
	}


}


double VisScatterDiagTwoDimDerived::YMAP(int srnDim, int valDim, double VAL)
{
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;

	OkcDataModifierPCADerive* mod = getModifierPCA();
	if (valDim > 2) {
		valDim = valDim - 3;
		return scatter_Y_axes[srnDim] +
        ( (dim_max[valDim] - VAL) * (scatter_Y_axes[srnDim+1] - scatter_Y_axes[srnDim] )
          / (dim_max[valDim] -dim_min[valDim]) );
	} else {
		return scatter_Y_axes[srnDim] +
        ( (mod->principal_max[valDim] - VAL) * (scatter_Y_axes[srnDim+1] - scatter_Y_axes[srnDim] )
          / (mod->principal_max[valDim] -mod->principal_min[valDim]) );
	}
}
