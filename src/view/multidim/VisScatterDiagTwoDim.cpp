/*
 * VisScatterDiagTwoDim.cpp
 *
 *  Created on: Aug 8, 2009
 *      Author: zyguo
 */

#include "VisScatterDiagTwoDim.h"

#include "view/multidim/VisScatter.h"
#include "view/ViewDisplay.h"
#include "data/multidim/OkcData.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/GLCanvas.h"
#include "visualmap/VisualAttribute.h"
#include "color/ColorManager.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "operator/brush/Brush.h"

VisScatterDiagTwoDim::VisScatterDiagTwoDim(VisScatter* _visScatter)
: VisScatterDiag(_visScatter) {
}

VisScatterDiagTwoDim::~VisScatterDiagTwoDim() {
}

void VisScatterDiagTwoDim::DrawScatters(){
	int data_size =  getDataSize();

	int shape = visScatter->getShapeType();
	int size = visScatter->getPointSize();


	std::vector<double> data(data_size);

	int width, height;
	width = XmdvTool::SUBWIN_WIDTH;
	height = XmdvTool::SUBWIN_HEIGHT;
	double radiusW = (double)size/(4.0*width);
	double radiusH = (double)size/(4.0*height);

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;
	CalculateRowsColumns();

	OkcData* okcdata = visScatter->getDisplayDataset();
	OkcDataModifierManager* okcDataModifierManager = okcdata->getOkcDataModifierManager();
	OkcVisualMapResult* okcVMR = visScatter->getOkcVisualMapResult();
	GLCanvas* canvas = GetCanvas();

	std::vector<int> temp_buf, buf;
	int all_dims = okcdata->getOrigDimSize();
	temp_buf.resize(all_dims);
	for(int i=0; i<all_dims; i++){
		temp_buf[i] = i;
	}

	okcDataModifierManager->mapData(temp_buf, buf);
	int dim_size = buf.size();

	for (int k = 0; k < dim_size; k++) {
		int i = buf[k];
		okcdata->GetDimensionData(data, i);

		double x1, y1;
		for (int j = 0; j < data_size; j++) {
			VisualAttribute* curAttr = okcVMR->getDiagMultiDimDataVisAttr(i, j);

			canvas->setForeground(curAttr->color);

			x1 = XOrderMAP(k, (j % columns));
			y1 = YOrderMAP(k, (j / columns));

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

void VisScatterDiagTwoDim::CalculateRowsColumns()
{
	int data_size =  getDataSize();

	int d2_width = int(sqrt((double)data_size) + 0.5);

	columns = d2_width;
	rows = data_size/columns+1;
}


double VisScatterDiagTwoDim::XOrderMAP(int DIM, int order)
{
	CalculateRowsColumns();
	if (columns < order) order = columns;
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;


	return 	scatter_X_axes[DIM] +
	(order + 0.5) *
	( scatter_X_axes[DIM+1] - scatter_X_axes[DIM] )
	/ (double)(columns);

}

double VisScatterDiagTwoDim::XOrderMAP2(int DIM, int order)
{
	CalculateRowsColumns();
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	return 	scatter_X_axes[DIM] +
	(order) *
	( scatter_X_axes[DIM+1] - scatter_X_axes[DIM] )
	/ (double)(columns);

}

//reverse of xmap. map screen back to data
int VisScatterDiagTwoDim::XOrderINVMAP(int DIM, double VAL)
{
	CalculateRowsColumns();
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	int order1 = (int)((VAL-scatter_X_axes[DIM]) / (scatter_X_axes[DIM+1] - scatter_X_axes[DIM] )
			*  columns -0.5);
	if (order1< 0) order1=0;
	return order1;
}

double VisScatterDiagTwoDim::YOrderMAP(int DIM, int order)
{
	CalculateRowsColumns();
	if (rows < order) rows = columns;
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	return	scatter_Y_axes[DIM] +
	(order +0.5) *
	( scatter_Y_axes[DIM+1] - scatter_Y_axes[DIM] )
	/ (double)(rows);
}

double VisScatterDiagTwoDim::YOrderMAP2(int DIM, int order)
{
	CalculateRowsColumns();
	if (rows < order) rows = columns;
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	return scatter_Y_axes[DIM] +
	(order) *
	( scatter_Y_axes[DIM+1] - scatter_Y_axes[DIM] )
	/ (double)(rows);
}

//reverse of xmap. map screen back to data
int VisScatterDiagTwoDim::YOrderINVMAP(int DIM, double VAL)
{
	CalculateRowsColumns();
	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	int order1 = (int)((VAL-scatter_Y_axes[DIM]) / (scatter_Y_axes[DIM+1] - scatter_Y_axes[DIM] )
			*  rows - 0.5);

	if (order1< 0) order1=0;
	return order1;
}
