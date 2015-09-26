/*
 * VisScatterDiagTwoDesDim.cpp
 *
 *  Created on: Aug 8, 2009
 *      Author: zyguo
 */

#include "VisScatterDiagTwoDesDim.h"

#include "view/multidim/VisScatter.h"
#include "view/ViewDisplay.h"
#include "data/multidim/OkcData.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/GLCanvas.h"
#include "visualmap/VisualAttribute.h"
#include "color/ColorManager.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "operator/brush/Brush.h"

VisScatterDiagTwoDesDim::VisScatterDiagTwoDesDim(VisScatter* _visScatter)
: VisScatterDiag(_visScatter) {
}

VisScatterDiagTwoDesDim::~VisScatterDiagTwoDesDim() {
}

void VisScatterDiagTwoDesDim::DrawScatters(){
	int data_size = getDataSize();

	std::vector<double> data(data_size);
	std::vector<double> locationx(data_size);
	std::vector<double> locationy(data_size);

	int cox = this->visScatter->getDataDrivenXDimNumber();
	int coy = this->visScatter->getDataDrivenYDimNumber();

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;

	OkcData* okcdata = visScatter->getDisplayDataset();

	OkcVisualMapResult* okcVMR = visScatter->getOkcVisualMapResult();
	GLCanvas* canvas = GetCanvas();

	okcdata->GetDimensionData(locationx, cox);
	okcdata->GetDimensionData(locationy, coy);

	int shape = visScatter->getShapeType();
	int size = visScatter->getPointSize();


	int width, height;
	width = XmdvTool::SUBWIN_WIDTH;
	height = XmdvTool::SUBWIN_HEIGHT;
	double radiusW = (double) size / (4.0 * width);
	double radiusH = (double) size / (4.0 * height);

	std::vector<int> temp_buf, buf;
	int all_dims = okcdata->getOrigDimSize();
	temp_buf.resize(all_dims);
	for(int i=0; i<all_dims; i++){
		temp_buf[i] = i;
	}
	OkcDataModifierManager* okcDataModifierManager = okcdata->getOkcDataModifierManager();
	okcDataModifierManager->mapData(temp_buf, buf);
	int dim_size = buf.size();

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

//Use scrDim as the screen dimension, valDim as vaue Dimension, and VAL as value
double VisScatterDiagTwoDesDim::XMAP(int srnDim, int valDim, double VAL)
{
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;



	OkcData* okcdata = visScatter->getDisplayDataset();

	std::vector<double> ori_dim_min, ori_dim_max;
	okcdata->getOrigDimMinArr(ori_dim_min);
	okcdata->getOrigDimMaxArr(ori_dim_max);

	return scatter_X_axes[srnDim] +
	( VAL -ori_dim_min[valDim] ) * ( scatter_X_axes[srnDim+1] -scatter_X_axes[srnDim] )
	/ (ori_dim_max[valDim] - ori_dim_min[valDim] );
}


double VisScatterDiagTwoDesDim::YMAP(int srnDim, int valDim, double VAL)
{
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	OkcData* okcdata = visScatter->getDisplayDataset();
	std::vector<double> ori_dim_min, ori_dim_max;
	okcdata->getOrigDimMinArr(ori_dim_min);
	okcdata->getOrigDimMaxArr(ori_dim_max);

	return scatter_Y_axes[srnDim] +
	( (ori_dim_max[valDim] - VAL) * (scatter_Y_axes[srnDim+1] - scatter_Y_axes[srnDim] )
			/ (ori_dim_max[valDim] -ori_dim_min[valDim]) );
}


//reverse of xmap. map screen back to data
double VisScatterDiagTwoDesDim::XINVMAP(int srnDim, int valDim, double VAL)
{
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;

	OkcData* okcdata = visScatter->getDisplayDataset();
	OkcDataModifierManager* okcDataModifierManager = okcdata->getOkcDataModifierManager();

	std::vector<double> ori_dim_min, ori_dim_max;

	okcDataModifierManager->invMapData(dim_min, ori_dim_min);
	okcDataModifierManager->invMapData(dim_max, ori_dim_max);

	return (VAL-scatter_X_axes[srnDim]) * (ori_dim_max[valDim] - ori_dim_min[valDim])
	/ (scatter_X_axes[srnDim+1] -scatter_X_axes[srnDim] )
	+ ori_dim_min[valDim];
}


double VisScatterDiagTwoDesDim::YINVMAP(int srnDim, int valDim, double VAL)
{
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;

	OkcData* okcdata = visScatter->getDisplayDataset();
	OkcDataModifierManager* okcDataModifierManager = okcdata->getOkcDataModifierManager();

	std::vector<double> ori_dim_min, ori_dim_max;

	okcDataModifierManager->invMapData(dim_min, ori_dim_min);
	okcDataModifierManager->invMapData(dim_max, ori_dim_max);

	return dim_max[valDim] -
	( VAL - scatter_Y_axes[srnDim] ) * ( ori_dim_max[valDim] - ori_dim_min[valDim] )
	/ ( scatter_Y_axes[srnDim+1] - scatter_Y_axes[srnDim] );
}
