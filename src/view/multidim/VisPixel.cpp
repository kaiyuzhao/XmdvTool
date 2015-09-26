/*
 * VisPixel.cpp
 *
 *  Created on: May 13, 2009
 */

#include <qfontmetrics.h>
#include <qwidget.h>
#include <qgl.h>
#include <QScrollArea>

#include "view/multidim/VisPixel.h"
#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "data/multidim/OkcData.h"
#include "view/GLCanvas.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "operator/brush/Brush.h"
#include "interface/pixel_customization/PixelCustomization.h"
#include "interface/pixel_customization/PixelCustomizationSettings.h"
#include "util/algo/mds/Multidimensional_scaling.h"
#include "view/ViewManager.h"
#include "pipeline/Transformation.h"
#include "operator/Operator.h"
#include "operator/rowindex/SortRowsOperator.h"
#include "pipeline/multidim/OkcPipeline.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "color/ColorManager.h"
#include "view/ViewWindow.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "view/GLDisplay.h"
#include "data/cluster/OkcDataModifierClusterEntries.h"
#include "data/multidim/OkcDataModifierManager.h"

namespace
{
float prevX;
float prevY;

const int MAX_FONT_SIZE = 11;
const int MIN_FONT_SIZE = 5;
};

VisPixel::VisPixel(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm)
: OkcViewDisplay(viewWindow, pm, pipelineID, vm) {
	m_currDim = 0;
	m_display = Subwindow::PDT_SPIRAL;
	m_sortedDim = -1; m_sortAsc = true;
	reset = true;
	m_sampleFactor = 1.0f;
	m_sampleStart = 0; m_sampleEnd = 1.0f;
	m_hasDistortionChanged = true;
	m_placementChanged = true;

	m_movingDim = -1;

	m_placementType = VisPixel::PPT_NORMAL;
	m_dispPos = NULL;
	m_options = NULL;

	prevX = 0.0f;
	prevY = 0.0f;

	m_runOnce = true;
	m_oldWidth = m_glDisplay->width();
	m_oldHeight = m_glDisplay->height();
}

VisPixel::~VisPixel() {
	freeSpace();
}

void VisPixel::drawSpiral()
{
	// figure out the size of each subwindow in terms of pixels
	// new way with Qt
	int width, height;

	width = m_glDisplay->getScrollArea()->size().width();
	height = m_glDisplay->getScrollArea()->size().height();

	static int size				= 0;
	static float xsize = 0, ysize = 0;
	int numSubWindInRow = (int)(sqrt(m_dim_size-1) + 1.0 + 1e-6);
	// allocate the images to be used
	if ( (int)m_images.size() != m_dim_size ||
			m_images[0]->Width() != size ||
			reset)
	{
		if (m_hier_info && m_hier_info->SHOW_BANDS) {
			OkcDataModifierManager* manager = getDisplayDataset()->getOkcDataModifierManager();
			OkcDataModifierClusterEntries* modifierCE =
				dynamic_cast<OkcDataModifierClusterEntries*>
			(manager->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERENTRIES));
			size = (int) (sqrt(modifierCE->getTotalEntries()) + 0.5f) + 1;
		} else
			size = (int) ((sqrt(m_data_size * m_sampleFactor))+0.5f)+1;
		/*
		 * The following if statements are used to determine the sub image size.
		 * The basic rule is:
		 * (1) If the dataset size permits, we assign each data item to one pixel;
		 * (2) If the dataset size is too big, we fit all dimensions in the window.
		 */
		if ( size / (float) width > 0.9/numSubWindInRow
				|| size / (float) height > 0.9/numSubWindInRow ) {
			xsize = 0.9/numSubWindInRow;
			ysize = xsize;
		} else {
			xsize = size / (float) width; //0.9/numSubWindInRow;
			ysize = size / (float) height; //xsize;
		}
		m_hasDistortionChanged = true;
		m_placementChanged = true;
		// delete things before allocating new ones
		for (int i = 0; i < (int)m_images.size(); ++i)
		{
			delete (Image *) m_images[i];
			delete (Subwindow *) m_subwindows[i];
			m_images[i] = NULL; m_subwindows[i] = NULL;
		}
		m_images.clear();
		m_subwindows.clear();
		m_images.resize(m_dim_size);
		m_subwindows.resize(m_dim_size);
		if(m_distortions.size() == 0) {
			m_distortions.assign(m_dim_size, 0.0f);
		}
		// allocate new ones
		if (!m_dispPos) {
			m_dispPos = new double * [2];
			m_dispPos[0] = new double [m_dim_size];
			m_dispPos[1] = new double [m_dim_size];
		}
		// figure out the number of subwindows that would fit on each row

		for (int dim = 0; dim < m_dim_size; ++dim)
		{
			//Image *img	= new Image(size + 1, size + 1);
			Image *img	= new Image(size, size);
			m_images[dim] = img;

			Subwindow *subwnd	= new Subwindow(dim);
			m_subwindows[dim] = subwnd;
		}
	}

	// figure out where to place the subwindow
	float prev_width = xsize, prev_height = ysize;
	int k	= -1;

	float xoff[numSubWindInRow], yoff[numSubWindInRow];
	// reposition layout on distortion change
	if(m_placementType == PPT_NORMAL) {
		for(int p = 0; p < numSubWindInRow; p++) {
			xoff[p] = 0.0f;
			yoff[p] = 0.0f;
		}
		xoff[0] = m_distortions[0] * width / height;
		for(int p = 0; p < m_dim_size; p++) {
			if( p % numSubWindInRow == 0) k++;
			if (m_distortions[p] * width / height > xoff[ p % numSubWindInRow ] && p % numSubWindInRow != numSubWindInRow-1)
				xoff[ p % numSubWindInRow ] = m_distortions[p] * width / height;
			if ( m_distortions[p] * width / height > yoff[k] )
				yoff[k] = m_distortions[p] * width / height;
		}
	}

	if (reset || m_placementChanged || m_hasDistortionChanged)
	{
		const float xs		= (xsize + 5.0f / width);
		k = -1;
		for (int dim = 0; dim < m_dim_size ; ++dim)
		{
			Subwindow *subwnd	= m_subwindows[dim];
			float x = 0, y = 0;
			// app always starts on normal, so initialize here
			if (m_placementType == PPT_NORMAL)
			{
				if ( dim % numSubWindInRow == 0) k++;
				float xoffset = xoff[0], yoffset = yoff[0];
				for(int p = 0; p < dim % numSubWindInRow; p++) {
					xoffset += xoff[p];
				}
				for(int p = 0; p < k; p++) {
					yoffset += yoff[p];
				}

				m_dispPos[0][dim] = (( dim % numSubWindInRow) * xs) + 0.04f  + xoffset;
				m_dispPos[1][dim] = (k * (ysize + 18.0f / height)) + yoffset;
			}
			x = (float) m_dispPos[0][dim];
			y = (float) m_dispPos[1][dim];
			prev_width = xsize + m_distortions[dim] * width / height;
			prev_height = ysize + m_distortions[dim] * width / height;

			subwnd->SetWindow(x, y, prev_width, prev_height);
		}
		reset = false;
		m_hasDistortionChanged = m_placementChanged = false;
	}

	// here is the actual painting...
	// for all subwindows
	std::vector<int> priority	(m_dim_size);
	// After turning off some dimensions, m_currDim might be bigger than m_dim_size-1
	if ( m_currDim >= m_dim_size ) {
		m_currDim = m_dim_size - 1;
	}
	int dim;
	for (dim = 0; dim < m_currDim; ++dim) {
		priority[dim] = dim;
	}
	for (dim = m_currDim + 1; dim < m_dim_size; ++dim) {
		priority[dim - 1] = dim;
	}
	priority[m_dim_size-1] = m_currDim;

	// calculate font size
	int totalLength = 17;
	int totalWidth = 0;
	for(int i = 0; i < m_dim_size; i++) {
		totalLength += strlen(m_dim_names[i]);
		float max_str_size_f = m_subwindows[i]->GetWidth();
		Vec2 max_str_size = m_canvas->mapDataToScreen(Vec2(max_str_size_f, 0));
		max_str_size.X = (max_str_size.X * .85) - 100;
		totalWidth += (int)max_str_size.X;
	}

	int aveLength = totalLength/m_dim_size;
	int aveWidth = totalWidth/m_dim_size;
	QFont newFont = g_globals.textFont;
	for(int i = MAX_FONT_SIZE; i > MIN_FONT_SIZE; i--) {
		newFont.setPointSize(i);
		QFontMetrics metrics(newFont);
		if(metrics.averageCharWidth() * aveLength * 2 < aveWidth) break;
	}

	for (int i = 0; i < m_dim_size; ++i)
	{
		dim = priority[i];
		Subwindow *subwnd	= m_subwindows[dim];

		float max_str_size_f = m_subwindows[dim]->GetWidth();
		Vec2 max_str_size = m_canvas->mapDataToScreen(Vec2(max_str_size_f, 0));
		max_str_size.X = (max_str_size.X * .85) - 100;

		glColor3f(0.0f, 0.0f, 0.0f);
		Vec2 pos = m_canvas->mapDataToScreen(Vec2(subwnd->GetX(),subwnd->GetY() - (2.0f / width)));
		m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::TEXT));
		m_canvas->drawConfinedString(pos.X, pos.Y,
				m_dim_names[dim], max_str_size.X, &newFont);
		// paint the subwindow
		if(getViewWindow()->getPipeline()->getPipelineType() == XmdvTool::PLTYPE_SBB) {
			subwnd->PaintHier(m_images[dim], pixel_info, m_dim_size, m_hier_info, m_display, dim == m_currDim);
		} else {
			subwnd->Paint(m_images[dim], pixel_info, m_dim_size, m_sampleStart, m_sampleEnd, m_display, dim == m_currDim);
		}

	}

}

void VisPixel::drawLegend() {

	int i, n = 100;
	double x = 0.0, y = 1.0;
	for (i=0; i<n; i++) {
		double val = (double)i/n;
		RGBt color = g_globals.colorManager->getColor(ColorManager::BRUSH_UNSELECTED, val );
		m_canvas->setForeground(color);
		m_canvas->fillRectangle(x+val, y, 1.0/n, 0.05);
	}
}

void VisPixel::drawAxes()
{
/*
 * The commented code is used to adjust canvas size to allow
 * each value to occupy one pixel.  This causes conficts with
 * global zooming.  Thus we comment it but do not delete it
 * in case future developers need to restore this piece of code.
	if(m_runOnce) {
		m_runOnce = false;
		// adjust canvas size
		// TODO: adjust size more accurately, this is just a base estimate of the real size
		// does not take into account distortion, sampling, etc.
		double newSize = (int) ((sqrt(m_data_size)+0.5f)+10)+ ceil(sqrt(m_dim_size)) * (int) ((sqrt(m_data_size)+0.5f)+10);
		if(newSize+50 > m_oldWidth || newSize > m_oldHeight)
			m_glDisplay->setFixedSize(int(newSize+50), int(newSize));

	}
*/

	if (m_placementType == PPT_MDS)
	{
		if(!m_options->getSettings()->settings_mdsPos) {
			std::vector<double> databuf;
			databuf.resize(m_data_size*m_dim_size);
			OkcData* okcdata = getDisplayDataset();
			std::vector<double> buf;
			for (int i = 0; i < m_data_size; i++) {
				okcdata->getData(buf, i);
				for(int j = 0; j < m_dim_size; j++)
					databuf[i*m_dim_size+j] = buf[j];
			}

			CalcPos(m_data_size, m_dim_size, databuf);
			m_options->updateMDS(m_dispPos);
		} else {
			for(int i = 0; i < m_dim_size; i++) {
				m_dispPos[0][i] = m_options->getSettings()->settings_mdsPos[0][i];
				m_dispPos[1][i] = m_options->getSettings()->settings_mdsPos[1][i];
			}
		}
		m_placementChanged = true;
	}

	computePixelInfo();

	// draw the pixels
	drawSpiral();

	drawLegend();

}

void VisPixel::computePixelInfo()
{
	pixel_info.data_size = m_data_size;
	pixel_info.norm_values.resize( m_data_size*m_dim_size );
	pixel_info.randomNums.resize(m_data_size);
	pixel_info.indices.resize(m_data_size);
	pixel_info.sizes.assign(m_data_size, 1);

	// Get the pointer to the data
	OkcData *okcdata = getDisplayDataset();

	// Set the normalized values
	int idx, i, j;
	double ratio[m_dim_size];
	for (j=0; j<m_dim_size; j++) {
		ratio[j] = 1.0 / (m_dim_max[j]-m_dim_min[j]);
	}
	idx = 0;
	for (i=0; i<m_data_size; i++)
	{
		pixel_info.indices[i] = i;
		pixel_info.randomNums[i] = rand() / RAND_MAX+1.0;
		std::vector<double> data;
		okcdata->getData(data, i);
		for (j=0; j<m_dim_size; j++) {
			pixel_info.norm_values[idx] = (data[j]-m_dim_min[j]) * ratio[j];
			idx++;
		}
	}

	// For hierarchical display, set the cluster entries
	if  ( getPipeline()->getPipelineType() == XmdvTool::PLTYPE_SBB ) {
		OkcDataModifierManager* manager = getDisplayDataset()->getOkcDataModifierManager();
		OkcDataModifierClusterEntries* modifierCE =
			dynamic_cast<OkcDataModifierClusterEntries*>
		(manager->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERENTRIES));
		for(int i = 0; i < m_data_size; i++) {
			pixel_info.sizes[i] = modifierCE->getEntries(i);
		}
	}
}

double VisPixel::norm(int dim, double val)
{
	return (val - m_dim_min[dim]) / (m_dim_max[dim] - m_dim_min[dim]);
}

void VisPixel::freeSpace()
{
	m_sortedDim = 0; m_sortAsc = true;
	m_currDim = 0;
	m_movingDim = -1;

	for (int i = 0; i < (int)m_images.size(); ++i)
	{
		delete (Image *) m_images[i];
		delete (Subwindow *) m_subwindows[i];
		m_images[i] = NULL; m_subwindows[i] = NULL;
	}
	m_images.clear(); m_subwindows.clear();
	m_distortions.clear();
}

void VisPixel::inputMouseMotion(int sx, int sy)
{
	// not working currently
	showCaption(sx, sy);

	// drag/drop
	if(m_movingDim >= 0) {
		Vec2 p		= m_canvas->mapScreenToData(Vec2(sx, sy));
		m_dispPos[0][m_movingDim] += (p.X - prevX);
		m_dispPos[1][m_movingDim] += (p.Y - prevY);
		prevX = p.X;
		prevY = p.Y;
		m_placementChanged = true;
		refresh(false, false);
	}
}

void VisPixel::inputButtonPress(int button, int x, int y)
{
	if (button == 2) {
		return;
	}

	Vec2 p		= m_canvas->mapScreenToData(Vec2(x, y));
	int nearDim	= findNearestDim(p);

	if (button == 0) {
		if (nearDim != -1) {
			m_currDim = nearDim;
			m_movingDim = nearDim;
			prevX = p.X;
			prevY = p.Y;
			m_options->getSettings()->settings_selectedDim = m_currDim;
			m_options->update();
			refresh(false, false);
		}
	} else if (button == 1)	{
		if (m_currDim == nearDim) {
			if (m_sortedDim == nearDim) {
				m_sortAsc = !m_sortAsc;
				m_options->getSettings()->settings_asc = !m_options->getSettings()->settings_asc;
				m_options->update();
			} else {
				m_sortedDim = nearDim;
				m_options->getSettings()->settings_sortedDim = m_sortedDim;
				m_options->update();
			}
			refresh(true, false);
		}
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int VisPixel::findNearestDim(Vec2 p)
{
	for (int i = 0; i < m_dim_size; ++i)
	{
		if (m_subwindows[i]->Contains((float) p.X, (float) p.Y))
			return i;
	}
	return -1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void VisPixel::inputButtonRelease(int, int, int)
{
	//if (button == 2)
	//	isPanning = false;
	m_movingDim = -1;
}

void VisPixel::setPlacementType(VisPixel::PlacementType pt)
{
	if (m_placementType == pt)
		return;

	m_placementType = pt;

	m_placementChanged = true;
}

bool VisPixel::showCaption(int x, int y)
{
	Vec2 p	= m_canvas->mapScreenToData(Vec2(x, y));

	int n = -1, dim = -1;
	ostringstream oss;

	int rx, ry;

	for (int i = 0; i < (int)m_subwindows.size(); ++i)
	{
		if (m_subwindows[i]->Contains((float) p.X, (float) p.Y, n, this->m_display, rx, ry))
			dim = i;
	}

	if (dim < 0)
	{
		char buffer[10];
		buffer[0] = 0;
		this->getViewManager()->getMainWnd()->statusBar->showMessage(QString(buffer));
		return true;
	}

	std::vector<double> buf;
	buf.resize(m_dim_size);
	if(n >= 0 && n < m_data_size) {
		OkcData* okcdata = getDisplayDataset();
		okcdata->getData(buf, n);
	}

	if (dim >= 0)
	{
		for (int i = 0; i < m_dim_size; ++i)
		{
			if (dim == i)
				oss << "{";
			oss << m_dim_names[i];
			if (n >= 0 && n < m_data_size) {
				oss << "=" << buf[i];
			}
			if (dim == i)
				oss << "}";
			if (i != m_dim_size-1)
				oss << ", ";
		}
		this->getViewManager()->getMainWnd()->statusBar->showMessage(QString(oss.str().c_str()));

		return true;
	}

	return false;
}

static inline double dimnorm(double num, const std::vector<double> &data, int data_num, int dims,
		int dimension)
{
	double scale;
	int idx	= dimension;
	double max = num, min = num;

	////////////////////////////////////////get the max and min value of one dimension///////////////////////////////////////////
	for (int count = 0; count < data_num; ++count, idx += dims)
	{
		if (data[idx] > max)
			max = data[idx];
		if (data[idx] < min)
			min = data[idx];
	}

	if (max == min)
		scale = 1;
	else
		scale = (num - min) / (max - min);

	return scale;
}

static inline double simi(const std::vector<double> &data, int data_num, int dims,
		int dim_1, int dim_2)
{
	double scale_1, scale_2, total = 0;
	int count	= 0;
	int idx1 = dim_1, idx2 = dim_2;

	//////////////////////calculate distance between dimension 1 and dimension 2//////////////////////////////////////
	for (count = 0; count < data_num; ++count, idx1 += dims,
	idx2 += dims)
	{
		scale_1 = dimnorm(data[idx1], data, data_num, dims, dim_1);
		scale_2 = dimnorm(data[idx2], data, data_num, dims, dim_2);

		double diff	= scale_1 - scale_2;
		total += diff * diff;
	}

	return sqrt(total);
}

void VisPixel::CalcPos(int n, int m, const std::vector<double> &data)
{
	int row, column;
	double **simi_matrix= new double *[m]; // Similarity matrix of the n data points

	int count			= 0;
	for (count = 0; count < m; ++count)
		simi_matrix[count] = new double[m];

	/////////////////get the similarity matrix of the data set///////////////////////////
	for (row = 0 ; row < m; ++row)
		for (column = 0; column < m; ++column)
		{
			simi_matrix[row][column] = simi_matrix[column][row] = simi(data, n,
					m, row,
					column);
			//cout<<simi_matrix[row][column]<<endl;
		}

	double **mdsPlacement = new double * [2];
	mdsPlacement[0] = new double [m_dim_size];
	mdsPlacement[1] = new double [m_dim_size];

	multidimensional_scaling(m, 2, simi_matrix, mdsPlacement);

	for(int i = 0; i < m_dim_size; i++) {
		m_dispPos[0][i+1] = mdsPlacement[0][i];
		m_dispPos[1][i+1] = mdsPlacement[1][i];
	}
	m_dispPos[0][0] = 0.0;
	m_dispPos[1][0] = 0.0;
	delete (double*) mdsPlacement[0];
	delete (double*) mdsPlacement[1];
	delete (double**) mdsPlacement;
}

void VisPixel::setDialog(PixelCustomization *options) {
	// set the options
	m_options = options;
	PixelCustomizationSettings *settings = m_options->getSettings();
	// update the position of the images
	if(!settings->positionSet) {
		settings->settings_position = m_dispPos;
		settings->positionSet = true;
	} else {
		m_dispPos = settings->settings_position;
		m_placementChanged = true;
	}
	if(getViewWindow()->getPipeline()->getPipelineType() == XmdvTool::PLTYPE_SBB)
		m_options->disableGUI();
}

void VisPixel::setSamplingFactor(float val)
{
	assert(val >= 0 && val <= 1.0f);
	m_sampleFactor = val;
	m_sampleStart = (rand() / RAND_MAX+1.0) * (1-m_sampleFactor);
	m_sampleEnd = m_sampleStart + m_sampleFactor;
	reset = true;
}

void VisPixel::refresh(bool sort, bool) {
	ViewManager * vm = getViewManager();
	if(sort) {
		PipelineManager* pm = vm->getMainWnd()->getPipelineManager();
		Transformation* mainT = (dynamic_cast<OkcPipeline*>(pm->getPipeline(m_pipelineID)))->getMainTransformation();
		SortRowsOperator* sortO = dynamic_cast<SortRowsOperator*>(mainT->getOperator(PL_FN_OP_SORT));
		if(m_sortedDim < 0) {
			mainT->enableOperator(PL_FN_OP_SORT, false);
		} else {
			mainT->enableOperator(PL_FN_OP_SORT, true);
			sortO->setSortWay (XmdvTool::OKCDIM_ORIGINAL);
			sortO->setSortDimNo (m_sortedDim);
			sortO->setSortOrder (m_sortAsc ? XmdvTool::SORTORDER_INCREASE : XmdvTool::SORTORDER_DECREASE);
		}
		pm->assemble(m_pipelineID, sortO);
	}
	if (vm->willOuputToVis(m_pipelineID, XmdvTool::VISTYPE_PI))
		vm->refreshDisplayByPID(m_pipelineID);
}


bool VisPixel::testDimensionality() {
	return (m_dim_size>=1);
}
