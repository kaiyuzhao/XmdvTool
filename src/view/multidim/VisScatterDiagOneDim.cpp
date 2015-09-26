/*
 * VisScatterDiagOneDim.cpp
 *
 *  Created on: Aug 3, 2009
 *      Author: zyguo
 */

#include "VisScatterDiagOneDim.h"
#include "view/multidim/VisScatter.h"
#include "view/ViewDisplay.h"
#include "data/multidim/OkcData.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/GLCanvas.h"
#include "visualmap/VisualAttribute.h"
#include "color/ColorManager.h"
#include "operator/brush/Brush.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/diagonal/RowIndexOnAllDimModifier.h"


VisScatterDiagOneDim::VisScatterDiagOneDim(VisScatter* _visScatter)
: VisScatterDiag(_visScatter) {
}

VisScatterDiagOneDim::~VisScatterDiagOneDim() {
}

void VisScatterDiagOneDim::DrawScatters(){
	int data_size = visScatter->m_data_size;
	OkcData* okcdata = visScatter->getDisplayDataset();

	OkcVisualMapResult* okcVMR = visScatter->getOkcVisualMapResult();
	GLCanvas* canvas = GetCanvas();

	DrawSingleBrush();

	OkcDataModifierManager* okcDataModifierManager =
		okcdata->getOkcDataModifierManager();

	OkcDataModifier* modifier = okcDataModifierManager->getOkcDataModifier(
			XmdvTool::MODIFIER_ALLDIMROWINDEX);

	RowIndexOnAllDimModifier* rowIndexOnAllDimModifier = 0;

	if (modifier) {
		// the modifier does exist
		rowIndexOnAllDimModifier = dynamic_cast<RowIndexOnAllDimModifier*> (modifier);
	}

	assert(rowIndexOnAllDimModifier!=0);
	int sortDim = visScatter->getOneDimSortDim();

	std::vector<int> temp_buf, buf;
	int all_dims = okcdata->getOrigDimSize();
	temp_buf.resize(all_dims);
	for(int i=0; i<all_dims; i++){
		temp_buf[i] = i;
	}
	okcDataModifierManager->mapData(temp_buf, buf);
	int dim_size = buf.size();

	std::vector<double> dataDim;
	for (int k = 0; k < dim_size; k++) {

		int i = buf[k];
		okcdata->GetDimensionData(dataDim, i);
		int count = 0;

		double x2, y2;
		double lastx2 = 0.5, lasty2 = 0.5;
		for (int j = 0; j < data_size; j++) {
			VisualAttribute* curAttr = okcVMR->getDiagMultiDimDataVisAttr(k, rowIndexOnAllDimModifier->sortOrderOnAllDim[sortDim][j]);
			canvas->setForeground(curAttr->color);
			y2 = (double) visScatter->YMAP(k, dataDim[rowIndexOnAllDimModifier->sortOrderOnAllDim[sortDim][j]]);
			x2 = OrderMAP(k, j);

			if (count > 0)
					canvas->drawLine(lastx2, lasty2, x2, y2);
			lastx2 = x2;
			lasty2 = y2;
			count++;
		}
	}
}

void VisScatterDiagOneDim::DrawSingleBrush(){
	OkcVisualMapResult* okcVMR = visScatter->getOkcVisualMapResult();
	Brush* brush = okcVMR->getBrush();

	GLCanvas* canvas = GetCanvas();
	canvas->setForeground(g_globals.colorManager->getColor(
			ColorManager::BRUSH_BG1));
	canvas->beginDraw();
	int dim_size = visScatter->m_dim_size;

	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;

	for (int j = 0; j < dim_size; j++) {
		Vec2 p, q;

		p.Y = brush->max[j];
		p.Y = CLAMP(p.Y, dim_min[j], dim_max[j]);

		q.Y = brush->min[j];
		q.Y = CLAMP(q.Y, dim_min[j], dim_max[j]);

		p.Y = visScatter->YMAP(j, p.Y);
		q.Y = visScatter->YMAP(j, q.Y);

		p.X = dim_min[j];
		q.X = dim_max[j];

		p.X = visScatter->XMAP(j, p.X);
		q.X = visScatter->XMAP(j, q.X);

		canvas->fillRectangle(p.X, p.Y, q.X - p.X, q.Y - p.Y);
	}

	canvas->endDraw();

}

//Map order data to screen.
double VisScatterDiagOneDim::OrderMAP(int DIM, int order)
{
	int data_size = visScatter->m_data_size;
    return 	visScatter->scatter_X_axes[DIM] +
				   (order ) *
				   ( visScatter->scatter_X_axes[DIM+1] - visScatter->scatter_X_axes[DIM] )
					 / (double)(data_size);

}

//reverse of xmap. map screen back to data
int VisScatterDiagOneDim::OrderINVMAP(int DIM, double VAL)
{
	int data_size = visScatter->m_data_size;
    return (int)((VAL-visScatter->scatter_X_axes[DIM]) / (visScatter->scatter_X_axes[DIM+1] - visScatter->scatter_X_axes[DIM] )
				*  data_size);
}
