/*
 * VisScatterDiagHisto.cpp
 *
 *  Created on: Aug 1, 2009
 *      Author: zyguo
 */

#include "VisScatterDiagHisto.h"
#include "view/multidim/VisScatter.h"
#include "view/ViewDisplay.h"
#include "data/multidim/OkcData.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/GLCanvas.h"
#include "visualmap/VisualAttribute.h"
#include "color/ColorManager.h"
#include "operator/brush/Brush.h"
#include "view/multidim/VisScatterDiag.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/diagonal/HistogramModifier.h"

class visScatter;

VisScatterDiagHisto::VisScatterDiagHisto(VisScatter* _visScatter)
: VisScatterDiag(_visScatter) {
}

VisScatterDiagHisto::~VisScatterDiagHisto() {
}

void VisScatterDiagHisto::DrawScatters(){

	OkcData*okcData =  this->visScatter->getDisplayDataset();
	OkcDataModifierManager* okcDataModifierManager = okcData->getOkcDataModifierManager();

	OkcDataModifier* modifier = okcDataModifierManager->getOkcDataModifier(
				XmdvTool::MODIFIER_HISTOGRAM);

	HistogramModifier* histogramModifier = 0;

	if (modifier) {
			// the modifier does exist
		histogramModifier = dynamic_cast<HistogramModifier*> (modifier);
	}

	assert ( histogramModifier!=0 );

	int histo_size = histogramModifier->histogram_size;

	DrawSingleBrush();

	visScatter->m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::BRUSH_SELECTED1));

	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	std::vector<int> temp_buf, buf;
	int all_dims = okcData->getOrigDimSize();
	temp_buf.resize(all_dims);
	for(int i=0; i<all_dims; i++){
		temp_buf[i] = i;
	}

	okcDataModifierManager->mapData(temp_buf, buf);
	int dim_size = buf.size();

	for(int k=0; k<dim_size; k++){
		int i = buf[k];
		double width = (scatter_X_axes[k+1] - scatter_X_axes[k]) / (histo_size);
		double y1 = scatter_Y_axes[k+1];
		double x1 = scatter_X_axes[k];
		double x2 = x1 + width;
		for(int j=0; j < histo_size; j++){
			double y2 = scatter_Y_axes[k + 1] - ((float) histogramModifier->frequency[i][j] / ((float) histogramModifier->max_frequency[i] + 5))
					* (scatter_Y_axes[k + 1] - scatter_Y_axes[k]);
			double height = ((float) histogramModifier->marked_freq[i][j]
					/ ((float) histogramModifier->max_frequency[i] + 5)) * (scatter_Y_axes[k + 1]
					- scatter_Y_axes[k]);
			double y3 = y1 - height;
			visScatter->m_canvas->drawRectangle2(x1, x2, y1, y2);
			visScatter->m_canvas->fillRectangle(x1, y3, width, height);

			x1 += width;
			x2 += width;
		}
	}
}

void VisScatterDiagHisto::DrawSingleBrush(){
	OkcVisualMapResult* okcVMR = visScatter->getOkcVisualMapResult();
	Brush* brush = okcVMR->getBrush();

	GLCanvas* canvas = GetCanvas();
	canvas->setForeground(g_globals.colorManager->getColor(ColorManager::BRUSH_BG1));
	canvas->beginDraw();
	int dim_size = visScatter->m_dim_size;

	std::vector<double> scatter_X_axes = visScatter->scatter_X_axes;
	std::vector<double> scatter_Y_axes = visScatter->scatter_Y_axes;

	std::vector<double> dim_min = visScatter->m_dim_min;
	std::vector<double> dim_max = visScatter->m_dim_max;


	for (int j = 0; j < dim_size; j++) {
		Vec2 p, q;

		p.X = brush->min[j];
		p.X = CLAMP(p.X, dim_min[j], dim_max[j]);

		q.X = brush->max[j];
		q.X = CLAMP(q.X, dim_min[j], dim_max[j]);

		p.Y = dim_min[j];
		q.Y = dim_max[j];

		p.X = visScatter->XMAP(j, p.X);
		p.Y = visScatter->YMAP(j, p.Y);
		q.X = visScatter->XMAP(j, q.X);
		q.Y = visScatter->YMAP(j, q.Y);

		canvas->fillRectangle(p.X, p.Y, q.X - p.X, q.Y - p.Y);
	}

	canvas->endDraw();

}
