/*
 * VisGlyph.cpp
 *
 *  Created on: Apr 22, 2009
 *      Author: Zaixian Xie
 */

#include "VisGlyph.h"
#include "main/XmdvTool.h"
#include "pipeline/PipelineManager.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "visualmap/multidim/GlyphPlaceResult.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/GLCanvas.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "view/GLDisplay.h"
#include <cmath>


VisGlyph::VisGlyph(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm)
: OkcViewDisplay(viewWindow, pm, pipelineID, vm) {
}

VisGlyph::~VisGlyph() {
}

void VisGlyph::setParameters() {
	// call OkcViewDisplay::setParameters() to initialize
	// parameters about dataset
	OkcViewDisplay::setParameters();

	OkcVisualMapResult* okcVMR = getOkcVisualMapResult();
	GlyphPlaceResult* placeResult = okcVMR->getGlyphPlaceResult();
	m_blocksize = placeResult->blocksize;
	m_basex = &(placeResult->basex);
	m_basey = &(placeResult->basey);

	// Although these two variables are used only in drawSingleData(),
	// we initialize them within setParameters() to reduce time cost.
	m_norm.resize(m_dim_size);
	m_poly.resize(m_dim_size+1);

    // Call setPlacement after initializing m_dim_size,
    // because setPlacement needs this value
    setPlacement();
}

void VisGlyph::setPlacement()
{
	int j;

    double alpha = 2.0*XmdvTool::PI/m_dim_size;
    star_angles.resize(m_dim_size);
    star_angles_cos_sized.resize(m_dim_size);
    star_angles_sin_sized.resize(m_dim_size);

	for (j=0; j<m_dim_size; j++)
    {
        star_angles[j] = j*alpha;
        star_angles_cos_sized[j] = (m_blocksize/2.0) * cos(star_angles[j]);
        star_angles_sin_sized[j] = (m_blocksize/2.0) * sin(star_angles[j]);
    }
}


void VisGlyph::drawSingleData(std::vector<double> &data, int datano) {

	// Compute normalized length of each ray for sample
	//
	int j;
	double last_x, last_y;     // ZFC
	double basex,basey;
	basex = (*m_basex)[datano];
	basey = (*m_basey)[datano];

	for (j = 0; j < m_dim_size; j++)
		m_norm[j] = (data[j] - m_dim_min[j]) / (m_dim_max[j] - m_dim_min[j]);

	// Compute endpoints for each ray based on evenly spaced angles
	// from center
	//
	double alpha	= 2.0 * XmdvTool::PI / m_dim_size;
	double center	= ((m_dim_size - 1) * alpha / 70) / 2;  // ZFC for centering the Profile
	bool skip = 0;

	//Draw the Profile Shape
	OkcVisualMapResult* okcVMR = getOkcVisualMapResult();
	switch (okcVMR->getGlyphShape()) {
	case XmdvTool::GLYPHSHAPE_PROFILE :
		last_x = basex;
		last_y = basey;

		for (j = 0; j < m_dim_size; j++)
		{
			m_poly[j].X = basex + (j * alpha / 70) ;
			m_poly[j].Y = basey - (m_norm[j] * m_blocksize);
			m_canvas->drawLine((float)(last_x - center), (float)(last_y + center),
					(float)(m_poly[j].X - center), (float)(m_poly[j].Y + center) );
			last_x = m_poly[j].X;
			last_y = m_poly[j].Y;
		}

		// Draw bounding polyline
		m_canvas->drawLine((float)(last_x - center), (float)(basey + center),
				(float)(last_x - center), (float)(last_y + center) );
		m_canvas->drawLine((float)(last_x - center), (float)(basey + center),
				(float)(basex - center), (float)(basey + center) );
		break;
	case XmdvTool::GLYPHSHAPE_STAR :
		for (j = 0; j < m_dim_size; j++)
		{

			m_poly[j].X = basex + m_norm[j] * star_angles_cos_sized[j];
			m_poly[j].Y = basey - m_norm[j] * star_angles_sin_sized[j];

			m_canvas->drawLine((float)basex, (float)basey, (float)m_poly[j].X, (float)m_poly[j].Y);
			skip = 0;
		}
		m_poly[m_dim_size].X = m_poly[0].X;
		m_poly[m_dim_size].Y = m_poly[0].Y;

		m_canvas->drawLines(m_poly);
		break;
	}
}

void VisGlyph::drawSingleBand(RGBt & color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int datano) {



	double temp1, temp2, temp3;
	int i;

	std::vector<Vec2> polymean(m_dim_size + 1);
	std::vector<Vec2> polymax(m_dim_size + 1);
	std::vector<Vec2> polymin(m_dim_size + 1);

	double basex,basey;
	basex = (*m_basex)[datano];
	basey = (*m_basey)[datano];

	double alpha = 2.0*M_PI/m_dim_size;
	double *isin = new double[m_dim_size];
	double *icos = new double[m_dim_size];

	double temp;
	for (i = 0; i < m_dim_size; i++) {
		temp = (m_blocksize / 2.0) / (m_dim_max[i]
				- m_dim_min[i]);
		icos[i] = temp * cos(i * alpha);
		isin[i] = temp * sin(i * alpha);
	}

	for ( i = 0; i < m_dim_size; i++ )
	{
        temp1 = (data[i] - m_dim_min[i]);
        temp2 = (max[i] - m_dim_min[i]);
        temp3 = (min[i] - m_dim_min[i]);
		if ( fabs(1 - m_hier_info->extent_scaling_fac ) > 1e-4 )
		{
			temp2 = temp1 + m_hier_info->extent_scaling_fac*(temp2-temp1);
			temp3 = temp1 + m_hier_info->extent_scaling_fac*(temp3-temp1);
        }
		polymax[i].X = basex + temp2 * icos[i];
        polymax[i].Y = basey - temp2 * isin[i];;
		polymin[i].X = basex + temp3 * icos[i];
        polymin[i].Y = basey - temp3 * isin[i];;
	}
	polymax[m_dim_size] = polymax[0];
	polymin[m_dim_size] = polymin[0];

	for (i = 0; i < m_dim_size; i++) {
		double temp = (data[i] - m_dim_min[i]);
		polymean[i].X = basex + temp * icos[i];
		polymean[i].Y = basey - temp * isin[i];
	}
	polymean[m_dim_size] = polymean[0];

	std::vector<Vec2> band(4);

	m_canvas->setForeground(color, m_opacity);

    // Draw the lower band
    int d;
    for (d=0; d<m_dim_size+1; d++)
    {
		Vec2 lo, hi;

        lo = polymin[d];
		hi = polymean[d];

        if (d==0) {
            band[0] = lo;
            band[3] = hi;

        } else {

            band[1] = lo;
            band[2] = hi;
            m_canvas->drawTransparentBand(band, 0.0, m_hier_info->density, m_canvas->fg_color, m_canvas->fg_color);

            band[0] = band[1];
            band[3] = band[2];
        }
    }

    // Draw the upper band
    for (d=0; d<m_dim_size+1; d++)
    {
        Vec2 lo, hi;

		lo = polymean[d];
		hi = polymax[d];

        if (d==0) {
            band[0] = lo;
            band[3] = hi;
        } else {
            band[1] = lo;
            band[2] = hi;

            m_canvas->drawTransparentBand(band, m_hier_info->density, 0.0, m_canvas->fg_color, m_canvas->fg_color);

            band[0] = band[1];
            band[3] = band[2];
        }
    }
}

void VisGlyph::inputMouseMotion(int x, int y)
{
	// If the visual mapping result is NULL, do noting and return
	if (!this->getOkcVisualMapResult()) {
		return;
	}

	int pointNo;
	Vec2 p = m_canvas->mapScreenToData(Vec2(x, y));
	inputGlyphFindPointNear(p.X, p.Y, pointNo);

	if(pointNo == -1){
		return;
	}

	std::vector<double> dataItem(m_dim_size);
	OkcVisualMapResult* vmr = getOkcVisualMapResult();
	Data* data = vmr->getData();
	OkcData* okcdata = dynamic_cast<OkcData*>(data);
	okcdata->getData(dataItem, pointNo);

	char buffer [5000];
	buffer[0] = 0;

	char glyphNo [10];
	sprintf (glyphNo, "%d", pointNo);

	double dimValue;
	char dimValueChar [30];


	strcat(buffer, "Glyph:");
	strcat(buffer, glyphNo);
	strcat(buffer, " [");

	for(int i=0; i<m_dim_size; i++){
		char *temp = m_dim_names[i];
		strcat(buffer, temp);
		strcat(buffer, "=");
		dimValue  = dataItem[i];
		sprintf(dimValueChar,"%.2lf",dimValue);
		strcat(buffer, dimValueChar);
		if(i==m_dim_size-1){
			break;
		}
		strcat(buffer, ",");
	}



	strcat(buffer, "]");

	this->getViewManager()->getMainWnd()->statusBar->showMessage(QString(buffer));

}

void VisGlyph::inputGlyphFindPointNear(double X, double Y, int& pointNo){
	double halfbsize = 0.5 / (1.0 + sqrt((double) (m_data_size)));

	int i, glyph_no = -1;
	double min = halfbsize * 3, temp;
	double basex,basey;
	for (i = 0; i < m_data_size; i++){

		basex = (*m_basex)[i];
		basey = (*m_basey)[i];

		if ((fabs(X - (*m_basex)[i]) < halfbsize) && (fabs(Y - (*m_basey)[i]) < halfbsize)) {
			temp = fabs(X - (*m_basex)[i]) + fabs(Y	- (*m_basey)[i]);
			if (temp < min) {
				glyph_no = i;
				min = temp;
			}
		}
	}

	pointNo = glyph_no;
}

