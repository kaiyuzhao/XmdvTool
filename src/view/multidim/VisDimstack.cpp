/*
 * VisDimstack.cpp
 *
 *  Created on: May 12, 2009
 *      Author: Dan Spitz
 */

#include "VisDimstack.h"
#include "main/XmdvTool.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "data/storage/CardStorage.h"
#include <vector>
#include "pipeline/PipelineManager.h"
#include "operator/brush/Brush.h"
#include "visualmap/VisualMapResult.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/GLCanvas.h"
#include "view/GLDisplay.h"
#include "view/ViewDisplay.h"
#include "view/multidim/OkcViewDisplay.h"
#include "interaction/MouseKeyboardStatus.h"
#include "color/ColorManager.h"


VisDimstack::VisDimstack(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm)
: OkcViewDisplay(viewWindow, pm, pipelineID, vm) {

}

VisDimstack::~VisDimstack() {
}

XmdvTool::CHECK_RENDERABLE_RESULT VisDimstack::checkRenderable(){
	setParameters(); //recalculate block sizes
	bool result =  (x_block_size >= min_x_block_size()
						&& y_block_size >= min_y_block_size());
	if (result){
		//if both block side lengths are large enough to be visible, pass
		return XmdvTool::PASS_RENDERABLE;
	}
	//otherwise, fail
	return XmdvTool::FAIL_RENDERABLE;
}


QString VisDimstack::getRenderErrorMessage(){
	return "This dataset cannot be visualized using the Dimension Stacking view. "
			"To use the Dimension Stacking view, reduce the number of the dimensions "
			"being visualized or reduce the cardinality of the dimensions.";
}

void VisDimstack::setParameters() {

	OkcViewDisplay::setParameters();

	// PAUL: draw_org
	// Calculate values for sensible drawing of dimstack
	// When the number of horizontal divisions reaches a certain
	// point, the drawing ceases to limit the exponential nature
	// of drawing a dimstack
	int divisions = 1;
	sensible_X_blocksize = sensible_Y_blocksize = 1;

	sensible_limit = m_dim_size - 1;

	int i = 0;
	for (; i < m_dim_size; i += 2)
	{
		divisions *= m_dim_cardinality[i];
		// PAUL: change this value to allow deeper dimstack drawing
		if (divisions >= 256)
		{
			sensible_limit = i;
			break;
		}
		// These values are needed for properly drawing the brush
		// at a certain depth
		sensible_X_blocksize *= m_dim_cardinality[i];
		if (m_dim_size > i + 1) //get number of buckets
			sensible_Y_blocksize *= m_dim_cardinality[i+1];
	}

	// PAUL: draw_org
	// Is the number of dimensions odd?
	if (i > 2 && i >= m_dim_size && m_dim_size % 2)
	{
		sensible_X_blocksize /= m_dim_cardinality[i-2];
	}


	// blocksize is a fraction out of 1
	sensible_X_blocksize = 1/ sensible_X_blocksize;
	sensible_Y_blocksize = 1/ sensible_Y_blocksize;

	setPlacement();
}

void VisDimstack::setPlacement(){

	if ( m_dim_size < 2 )
		//really ought to give an error message here.
		//or implement 1-D dimstacking...
		return;

    dimstack_size.resize(m_dim_size);

    double odd=1, even=1;

    // Compute product of cardinalities in each orientation
    int i;
    for (i = 0;i < m_dim_size;i=i+2)
        odd *= m_dim_cardinality[i];

    for (i = 1;i < m_dim_size;i=i+2)
        even *= m_dim_cardinality[i];

    odd = 1.0/odd;
    even = 1.0/even;

    // Now compute size of a virtual pixel for each dimension
    for (i = 0;i < m_dim_size;i++)   {
        dimstack_size[i] = (i&1)?even:odd;
        for (int j = i+2; j < m_dim_size; j=j+2)
            dimstack_size[i] *= m_dim_cardinality[j];
    }

    x_block_size = odd;
    y_block_size = even;
    x_size = dimstack_size[0]*m_dim_cardinality[0];
    y_size = dimstack_size[1]*m_dim_cardinality[1];
}

void VisDimstack::drawAxes(){
	drawDimstackGrid();
}

void VisDimstack::drawDimstackGrid(){

	glLineWidth(1.);
	glShadeModel(GL_SMOOTH);
	m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::GRID1));

	// Draw up to 3 levels of grid marks, based on the number of dimensions
	if (m_dim_size >= 6)
	{
		m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::GRID3));
		// PAUL: draw_org
		// Move begindraw to the outside
		m_canvas->beginDraw();
		for (double x = 0;x <= x_size; x = x + dimstack_size[4])
			m_canvas->drawLine((float)x, 0.0f, (float)x, (float)y_size);

		for (double y = 0;y <= y_size; y = y + dimstack_size[5])
			m_canvas->drawLine(0.0f, (float)y, (float)x_size, (float)y);
		m_canvas->endDraw();
	}

	if (m_dim_size >= 4)
	{
		m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::GRID2));
		// PAUL: draw_org
		// Move begindraw to the outside
		m_canvas->beginDraw();
		for (double x = 0;x <= x_size; x = x + dimstack_size[2])
			m_canvas->drawLine((float)x, 0.0f, (float)x, (float)y_size);

		for (double y = 0;y <= y_size; y = y + dimstack_size[3])
			m_canvas->drawLine(0.0f, (float)y, (float)x_size, (float)y);
		m_canvas->endDraw();
	}

	m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::GRID1));

	// PAUL: draw_org
	// Move begindraw to the outside
	m_canvas->beginDraw();
	for (double x = 0;x <= x_size; x = x + dimstack_size[0])
		m_canvas->drawLine((float)x, 0.0f, (float)x, (float)y_size);

	for (double y = 0;y <= y_size; y = y + dimstack_size[1])
		m_canvas->drawLine(0.0f, (float)y, (float)x_size, (float)y);
	m_canvas->endDraw();

	glShadeModel(GL_FLAT);
	m_canvas->flush();
}

void VisDimstack::drawSingleData(std::vector<double> &data, int){
	// PAUL: lang_opt
	// Make contrib static, so that it is not reallocated every time the function
	// is called for a single drawing event
	static std::vector<int> contrib;
	contrib.resize(m_dim_size);
	int j;
	for (j=0; j<m_dim_size; j++)
		contrib[j] = get_contrib(j, data[j]);

	double endx = 0, endy = 0;
	for (j=0; j<m_dim_size; j++)
	{
		endx += contrib[j] * dimstack_size[j];
		j++;
		if (j < m_dim_size)
			endy += contrib[j] * dimstack_size[j];
	}
	m_canvas->fillRectangle(endx, y_size - endy - y_block_size, x_block_size, y_block_size);
}

void VisDimstack::drawSingleBand(RGBt &color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int){
	this->color = color;

	std::vector<int> adjustedMax, adjustedMin;
	adjustedMax.resize(m_dim_size);
	adjustedMin.resize(m_dim_size);

	//initializes the lengths of the contrib arrays
	contrib.resize(m_dim_size);
	contrib0.resize(m_dim_size);
	contrib1.resize(m_dim_size);
	contrib2.resize(m_dim_size);

	//get the adjusted max and min arrays
	for ( int i = 0; i < m_dim_size; i++ )
	{
		adjustedMax[i] = (int)((max[i]-data[i]) * m_hier_info->extent_scaling_fac + data[i]);
		adjustedMin[i] = (int)((min[i]-data[i]) * m_hier_info->extent_scaling_fac + data[i]);
	}

	//populate the contrib arrays for use when drawing the band
	int j;
	for (j=0; j<m_dim_size; j++){
		contrib0[j] = get_contrib(j, data[j]);
		contrib1[j] = get_contrib(j, adjustedMin[j]);
		contrib2[j] = get_contrib(j, adjustedMax[j]);
	}

	//begin the recursive call to traceBand to draw a band in all possible
	//directions
	traceBand(0);
}

//draw band of level k
void VisDimstack::traceBand(int k){
	int j;
	if (k == m_dim_size - 1)
	{
		for (j = contrib1[k]; j <= contrib2[k]; j++)
		{
			contrib[k] = j;
			fillBinInBand();
		}
		return;
	}
	else
	{
		for (j = contrib1[k]; j <= contrib2[k]; j++)
		{
			contrib[k] = j;
			traceBand(k+1);
		}
		return;
	}
}
//draw a bin in a band
void VisDimstack::fillBinInBand(){
	double density = distanceFactor();
	m_canvas->beginDraw();
	m_canvas->setForeground(color, density);
	double endx = 0, endy = 0;
	for (int j=0; j<m_dim_size; j++)
	{
		endx += contrib[j] * dimstack_size[j];
		j++;
		if (j < m_dim_size)
			endy += contrib[j] * dimstack_size[j];
	}
	m_canvas->fillRectangle(endx, y_size - endy - y_block_size,
							x_block_size, y_block_size);
	m_canvas->endDraw();
}
//calculate the distance factor of a bin in the band
double VisDimstack::distanceFactor(){
	double temp, temp1;
	double factor, sum = 0;
	for ( int i = 0; i < m_dim_size; i++ )
	{
		if ( contrib[i] > contrib0[i] )
		{
			temp = contrib[i] - contrib0[i];
			temp1 = contrib2[i] - contrib0[i];
		}
		else
		{
			temp = contrib0[i] - contrib[i];
			temp1 = contrib0[i] - contrib1[i];
		}
		if ( temp1 > 0 )
			factor = temp / temp1;
		else
			factor = 0;
		sum += factor;
	}
	factor = sum / m_dim_size;
	if ( factor < 0.0 )
		factor = -1.0 * factor;

	if ( fabs( factor ) < 1e-6 )
		return 0;
	else
		return ((1-factor)*0.9+0.1)*m_opacity;
}

//
// Compute the descretized value from an original data value for a particular
// dimension.  Display will be messed up if it is out of bounds.  You then
// should fix the range for that dimension in your data file.
//
int VisDimstack::get_contrib(int ind, double data)
{

    int res = int( ((data - m_dim_min[ind]) / (m_dim_max[ind] - m_dim_min[ind])) * (double)(m_dim_cardinality[ind]));

    return res;
}


// Display brush coverage by descretizing brush in each dimension and check
// where in range of that dimension it overlaps.  Roundoff error in the
// discretization process can reduce the visual effect of brush coverage.
//
void VisDimstack::drawSingleBrush(Brush *brush)
{

	m_canvas->setForeground( g_globals.colorManager->getColor(ColorManager::BRUSH_BG1));
	m_canvas->beginDraw();
	// For the outermost dimensions...
    for (int i=0; i<m_dim_cardinality[0]; i++)
    {
		// PAUL: draw_org
		// Move one of the inner is_withins to the outside
		if (is_within(i, 0, brush) == 1)
		{
			for (int j=0; j<m_dim_cardinality[1]; j++)
			{
				// if the brush intersects those dimensions recursively
				// call to fill
				if (is_within(j, 1, brush) == 1)
					rec_fill(i * dimstack_size[0], j * dimstack_size[1],
							2, 3, dimstack_size, brush);
			}
		}
    }
	m_canvas->endDraw();
}

//
// Determine if this value (val) for this dimension (dim) is covered by
// the brush.
//
bool VisDimstack::is_within(int val, int dim, Brush *brush)
{
    double fval = val;

    double range = m_dim_max[dim] - m_dim_min[dim];
    double b_pos = m_dim_cardinality[dim] * (brush->Pos(dim)-m_dim_min[dim]) / range;
    double b_size = .5 * m_dim_cardinality[dim] * brush->Size(dim) / range;
    double start = b_pos - b_size;
    double end = b_pos + b_size;
    if (end > fval && end <= (fval+1.0)) return true;
    if (start >= fval && start < (fval+1.0)) return true;
    if (start < fval && end > (fval+1.0)) return true;
    return false;
}

//
// Recursive check for brush coverage, 2 dimensions at a time
//
void VisDimstack::rec_fill(double basex, double basey, int nextx, int nexty, std::vector<double> &size, Brush *brush)
{
	// If we had an even number of dimensions and are at the bottom of the
    // recursion, draw a rectangle in that data spot, as it is covered.
    //
    if (nextx == m_dim_size)
    {
        m_canvas->fillRectangle(basex, y_size - basey - y_block_size,
							  x_block_size, y_block_size);
        return;
    }

	// PAUL: draw_org
	// If we have recursed enough, just fill the space rather than go down in near-endless recursion
	if (nextx >= sensible_limit)
	{
		for (int i=0; i<m_dim_cardinality[nextx]; i++)
		{
            if (is_within(i, nextx, brush))
				m_canvas->fillRectangle(basex, y_size - basey - sensible_Y_blocksize,
									  sensible_X_blocksize, sensible_Y_blocksize);
		}
		return;
	}

    // If we had an odd number of dimensions and are at the bottom of the
    // recursion, check each descrete value for brush overlap and draw rectangles
    // over covered spots.
    //
    int i;
    if (nexty == m_dim_size)
    {
        for (i=0; i<m_dim_cardinality[nextx]; i++)
		{
            if (is_within(i, nextx, brush))
                m_canvas->fillRectangle(basex + i * size[nextx],
                		y_size - basey - y_block_size, x_block_size, y_block_size);
        }
        return;
    }

    // Continue recursion for each bucket of the current 2 dimensions
    //
    for (i=0; i<m_dim_cardinality[nextx]; i++)
		// PAUL: draw_org
		// Move an is_within() call to the outside
		if (is_within(i, nextx, brush))	// Moved from the inner loop
			for (int j=0; j<m_dim_cardinality[nexty]; j++)
				if (is_within(j, nexty, brush))
					rec_fill(basex + i * size[nextx], basey + j * size[nexty], nextx+2, nexty+2, size, brush);
}
//input handling code - recenter the brush on a click.
void VisDimstack::inputButtonPress(int, int x, int y){
	// If the visual mapping result is NULL, do noting and return
	if (!this->getVisualMapResult()) {
		return;
	}

	MouseKeyboardStatus* m_mouseKeyboard = this->getMouseKeyboardStatus();

	if (m_mouseKeyboard->button[0])
		inputDimstackMoveBrush(x, y);

}

void VisDimstack::inputDimstackMoveBrush(int x, int y){

	Vec2 p = m_canvas->mapScreenToData(Vec2(x, y));

	//get the center-point for each dimension of the clicked bin.
	std::vector<double> centers(m_dim_size);
	bool result = dimstackMapScreenToData(p.X, p.Y, centers);
	if (result) {
		Brush *brush = getBrush();

		for (int i = 0; i < m_dim_size; i++){
			//check that the new center will not cause the
			//brush edge to go beyond the dimension range
			double offset = brush->Size(i) * .5;
			if (centers[i] + offset > m_dim_max[i])
				centers[i] = m_dim_max[i] - offset;
			else if (centers[i] - offset < m_dim_min[i])
				centers[i] = m_dim_min[i] + offset;
		}

		//all is in range. update the brush and refresh
		for (int i = 0; i < m_dim_size; i++)
			brush->SetPos(i, centers[i]);

		refreshBrush();
	}
}

bool VisDimstack::dimstackMapScreenToData(double x, double y, std::vector<double> &center)
{
    // Now find the bin that the user clicked in
    int i;
    for (i=0; i< m_dim_size; i++)
    {
        // If odd, it's in the y direction
        if (i%2)
        {
            int bin = int(y/dimstack_size[i]);

            // invert y
            int orig_bin = bin;
            bin = m_dim_cardinality[i] - 1 - bin;

            // check bounds
            if (bin < 0 || bin >= m_dim_cardinality[i])
                return false;

            // Increment data coords based on this
            center[i] = bin * (m_dim_max[i] - m_dim_min[i])/m_dim_cardinality[i];
            center[i] += m_dim_min[i];

            // use original bin # for y decrement
            y -= orig_bin*dimstack_size[i];
        }
        // If even, it's in the x direction
        else
        {
            int bin = int(x/dimstack_size[i]);

            // check bounds
            if (bin < 0 || bin >= m_dim_cardinality[i]){
                return false;
            }
            // Increment data coords based on this
            center[i] = bin * (m_dim_max[i] - m_dim_min[i])/m_dim_cardinality[i];
            center[i] += m_dim_min[i];

            x -= bin*dimstack_size[i];
        }
    }
    return true;
}

void VisDimstack::inputMouseMotion(int x, int y)
{
	// If the visual mapping result is NULL, do noting and return
	if (!this->getOkcVisualMapResult()) {
		return;
	}

	Vec2 p = m_canvas->mapScreenToData(Vec2(x, y));
	std::vector<double> centers(m_dim_size);
	//bool result = dimstackMapScreenToData(p.X, p.Y, centers);

	char buffer[5000];
	buffer[0] = 0;

	double dimValue;
	char dimValueChar[30];

	for (int i = 0; i < m_dim_size; i++) {
		char *temp = m_dim_names[i];
		strcat(buffer, temp);
		strcat(buffer, "=");
		dimValue = centers[i];
		if( dimValue > m_dim_max[i] || dimValue < m_dim_min[i]){
			buffer[0] = 0;
			break;
		}
		sprintf(dimValueChar, "%.2lf", dimValue);
		strcat(buffer, dimValueChar);
		if (i == m_dim_size - 1) {
			break;
		}
		strcat(buffer, ",");
	}

	this->getViewManager()->getMainWnd()->statusBar->showMessage(QString(buffer));
}

void VisDimstack::getDimstack_size(std::vector<double>& stack_size) {
	stack_size = dimstack_size;
}

double VisDimstack::getXBlockSize() {
	return x_block_size;
}

double VisDimstack::getYBlockSize() {
	return y_block_size;
}

void VisDimstack::refreshCardinality(std::vector<int> newCard) {
	PipelineManager* pm = getPipelineManager();

	CardStorage* newCardStorage = CardStorage::createCardStorageFromARR(newCard);
	int pID = getPipelineID();
	// update the assistant input by the reverse pipeline
	pm->updateAssisInput(pID, (OkcData*)newCardStorage, XmdvTool::ASSISDATA_CARD);
	delete newCardStorage;

	// Reassemble the pipeline but only do the visual mapping
	pm->assemble(pID, -1);
	// Since the new cardinality probably is not big to display,
	// we do not refresh display until we call checkRenderable(),
	// This will be done in  DimstackCustomization::applyChanges()
	//getViewManager()->refreshDisplayByPID(pID);

}
