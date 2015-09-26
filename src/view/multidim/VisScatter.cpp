/*
 * VisScatter.cpp
 *
 *  Created on: Apr. 1, 2008
 *      Author: Zaixian Xie, Zhenyu Guo
 */

#include "view/multidim/VisScatter.h"

#include "main/XmdvTool.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "interaction/MouseKeyboardStatus.h"
#include "operator/brush/Brush.h"
#include "pipeline/PipelineManager.h"
#include "visualmap/VisualMapResult.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/ViewManager.h"
#include "view/GLCanvas.h"
#include "view/ViewDisplay.h"
#include "color/ColorManager.h"
#include "view/multidim/VisScatterDiagHisto.h"
#include "view/multidim/VisScatterDiagOneDim.h"
#include "view/multidim/VisScatterDiagTwoDim.h"
#include "view/multidim/VisScatterDiagTwoDesDim.h"
#include "view/multidim/VisScatterDiagTwoDimDerived.h"
#include "visualmap/multidim/ScatterVisAttr.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "view/ViewWindow.h"
#include "view/GLDisplay.h"

#include <typeinfo>
#include <assert.h>

VisScatter::VisScatter(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm)
: OkcViewDisplay(viewWindow, pm, pipelineID, vm) {
	dimx = -1;
	dimy = -1;
}

VisScatter::~VisScatter()
{
}

void VisScatter::drawAxes() {
	drawScatterGrid();
}

//draw single data
void VisScatter::drawSingleData(std::vector<double> &data, int) {
	int j, k;
	int shape = this->getShapeType();
	int size = this->getPointSize();

	if(shape == 0){
		glPointSize((float) (size));
		glBegin(GL_POINTS);
		for (j=0; j<m_dim_size; j++)
		{
			double x = data[j];
			float xScreen = (float) XMAP(j, x);
			for (k=0; k<m_dim_size; k++)
			{
				if(j==k&&this->getDiagType()!=XmdvTool::DIAGPLOT_ORIGINAL){
					continue;
				}
				double y = data[k];
				float yScreen = (float) YMAP(k, y);
				glVertex2f(xScreen, yScreen);

			}
		}

		glEnd();
	}else if (shape ==1){
		int width, height;
		width = XmdvTool::SUBWIN_WIDTH;
		height = XmdvTool::SUBWIN_HEIGHT;
		double radiusW = (double)size/(4.0*width);
		double radiusH = (double)size/(4.0*height);
		for (j=0; j<m_dim_size; j++) {
			double x = (double) XMAP(j,data[j]);
			for (k=0; k<m_dim_size; k++) {
				if(j==k&&this->getDiagType()!=XmdvTool::DIAGPLOT_ORIGINAL){
					continue;
				}
				double y = (double) YMAP(k,data[k]);
				m_canvas->drawCircle(x, y, radiusW, radiusH);
			}
		}
	}

}

void VisScatter::drawSingleBand(RGBt & color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int) {

	m_canvas->setForeground(color, m_opacity);

	std::vector<Vec2> p(5);

	//draw band
	for (int j = 0; j < m_dim_size; j++) {
		for (int k = 0; k < m_dim_size; k++) {
			p[0].X = XMAP(j, data[j]);
			p[0].Y = YMAP(k, data[k]);
			p[1].X = XMAP(j, min[j]);
			p[1].Y = YMAP(k, min[k]);
			p[2].X = XMAP(j, min[j]);
			p[2].Y = YMAP(k, max[k]);
			p[3].X = XMAP(j, max[j]);
			p[3].Y = YMAP(k, max[k]);
			p[4].X = XMAP(j, max[j]);
			p[4].Y = YMAP(k, min[k]);
			if (fabs(1 - m_hier_info->extent_scaling_fac) > 1e-4) {
				for (int i = 1; i <= 4; i++) {
					p[i].Y = (1 - m_hier_info->extent_scaling_fac) * p[0].Y
					+ (m_hier_info->extent_scaling_fac * p[i].Y);
					p[i].X = (1 - m_hier_info->extent_scaling_fac) * p[0].X
					+ (m_hier_info->extent_scaling_fac * p[i].X);
				}
			}

			m_canvas->drawTransparentSquare2(p, 0.0, m_hier_info->density, 0);
		}
	}

}

void VisScatter::drawScatterGrid()
{
	int i,j;

	glLineWidth(1.);
	glShadeModel(GL_SMOOTH);
	m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::GRID1));

	//draw vertical lines
	for (i = 0; i <= m_dim_size; i++)
	{
		m_canvas->drawLine( scatter_X_axes[i], scatter_Y_axes[0],
				scatter_X_axes[i], scatter_Y_axes[m_dim_size]);
	}
	//draw horizontal lines
	for (j = 0; j <= m_dim_size; j++)
	{
		m_canvas->drawLine( scatter_X_axes[0], scatter_Y_axes[j],
				scatter_X_axes[m_dim_size], scatter_Y_axes[j]);
	}

	drawScatterAxesName();

}

void VisScatter::drawScatterAxesName() {
	int i,j;
	m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::TEXT));

	for (i=0; i<m_dim_size; i++)
	{
		double x = scatter_X_axes[i];
		Vec2 pos;
		//this is the position to draw dimension names and maximum values
		pos = m_canvas->mapDataToScreen(Vec2(x,0.0));
		m_canvas->drawString((int)pos.X,
				(int)(pos.Y-0.5*g_globals.textFont.pointSize()), m_dim_names[i]);
	}

	// draw vertical labels
	// add letters one at a time until space runs out
	for (i=0; i<m_dim_size; i++)
	{
		// initial coordinates
		Vec2 posi, posi_1;
		posi = m_canvas->mapDataToScreen(Vec2(0.0, scatter_Y_axes[i]));
		posi_1 = m_canvas->mapDataToScreen(Vec2(0.0, scatter_Y_axes[i+1]));
		posi.X -= 1.5*g_globals.textFont.pointSize();
		posi.Y += 1.2*g_globals.textFont.pointSize();

		// This loop draw the dimension name letter by letter.
		for (j=0; j<int(strlen(m_dim_names[i])); j++)
		{
			// If the vertical position is beyond the range of this dimension,
			// skip the other letters
			if ( posi.Y > posi_1.Y - 0.2 * g_globals.textFont.pointSize() )
				break;
			m_canvas->drawChar((int)posi.X, (int)posi.Y, m_dim_names[i][j]);
			posi.Y += g_globals.textFont.pointSize();
		}
	}

}
void VisScatter::setShapeType(int shapeType){
	this->getOkcVisualMapResult()->getScatterVisAttr()->setShape(shapeType);
}

void VisScatter::setPointSize(int size){
	this->getOkcVisualMapResult()->getScatterVisAttr()->setPointSize(size);
}

void VisScatter::setDiagType(XmdvTool::DIAGPLOT_MODE _diag_type){
	this->getOkcVisualMapResult()->getScatterVisAttr()->setDiagType(_diag_type);
	//this->diagType = _diag_type;
}

XmdvTool::DIAGPLOT_MODE VisScatter::getDiagType(){
	return this->getOkcVisualMapResult()->getScatterVisAttr()->getDiagType();
}

int VisScatter::getShapeType(){
	return this->getOkcVisualMapResult()->getScatterVisAttr()->getShape();
	//return this->shape;
}

int VisScatter::getPointSize(){
	return this->getOkcVisualMapResult()->getScatterVisAttr()->getPointSize();
}



void VisScatter::setPlacement() {
	scatter_X_axes.resize(m_dim_size+1);
	scatter_Y_axes.resize(m_dim_size+1);
	scatter_X_interval.resize(m_dim_size);
	scatter_Y_interval.resize(m_dim_size);

	double width = 1./m_dim_size;
	scatter_X_axes[0] = scatter_Y_axes[0] = 0;
	for ( int i = 0; i < m_dim_size; i++ )
	{
		scatter_X_axes[i+1] = scatter_Y_axes[i+1] = scatter_X_axes[i] +  width;
		scatter_X_interval[i] = scatter_Y_interval[i] = width;
	}
}

// Set some initial parameters for scatterplot matrices, including:
// 1. The coefficients for XMAP and YMAP
void VisScatter::setParameters() {
	// call OkcViewDisplay::setParameters() to initialize
	// parameters about dataset
	OkcViewDisplay::setParameters();

	setPlacement();
	setMapFactors();
}

// Set the coefficients for XMAP and YMAP
void VisScatter::setMapFactors() {
	int DIM;

	xMapFactor.resize(m_dim_size);
	yMapFactor.resize(m_dim_size);
	for (DIM = 0; DIM < m_dim_size; ++DIM)
	{
		double inv = 1.0 / ( m_dim_max[DIM] - m_dim_min[DIM] );

		xMapFactor[DIM] = inv * ( scatter_X_axes[DIM+1] - scatter_X_axes[DIM] );
		yMapFactor[DIM] = inv * ( scatter_Y_axes[DIM+1] - scatter_Y_axes[DIM] );
	}

}

// Draw a single brush.
void VisScatter::drawSingleBrush(Brush *brush)
{
	// PAUL: draw_org
	// Move begin draw to the outside
	int j,k;

	m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::BRUSH_BG1));

	m_canvas->beginDraw();


	// PAUL: scatter_bounds
	// The number of dimensions drawn is bound by their visibility
	for (j=0; j<m_dim_size; j++)
	{
		Vec2 p, q;
		p.X = brush->min[j];
		p.X = CLAMP(p.X, m_dim_min[j], m_dim_max[j]);

		q.X = brush->max[j];
		q.X = CLAMP(q.X, m_dim_min[j], m_dim_max[j]);
		p.X = VisScatter::XMAP(j, p.X);
		q.X = VisScatter::XMAP(j, q.X);

		for (k=0; k<m_dim_size; k++)
		{
			if (j==k&&this->getDiagType()!=XmdvTool::DIAGPLOT_ORIGINAL) continue;
			p.Y = brush->max[k];
			p.Y = CLAMP(p.Y, m_dim_min[k], m_dim_max[k]);
			q.Y = brush->min[k];
			q.Y = CLAMP(q.Y, m_dim_min[k], m_dim_max[k]);
			p.Y = VisScatter::YMAP(k, p.Y);
			q.Y = VisScatter::YMAP(k, q.Y);
			m_canvas->fillRectangle(p.X, p.Y, q.X-p.X, q.Y-p.Y);
		}
	}
	m_canvas->endDraw();
}


//implemented for scatterplot special diagonal display
void VisScatter::drawSpecialCustomization(){
	if (this->getDiagType()==XmdvTool::DIAGPLOT_ORIGINAL){
		// do nothing if the display type is 0
		return;
	}else if(this->getDiagType()==XmdvTool::DIAGPLOT_HISTOGRAM){
		// the diagonal display type is histogram
		currentDiag = new VisScatterDiagHisto(this);
	}else if(this->getDiagType()==XmdvTool::DIAGPLOT_ONE_DIM){
		// the diagonal display type is one dimension display
		currentDiag = new VisScatterDiagOneDim(this);
	}else if(this->getDiagType()==XmdvTool::DIAGPLOT_TWO_DIM_IMPLICIT){
		// the diagonal display type is two dimension display
		currentDiag = new VisScatterDiagTwoDim(this);
	}else if(this->getDiagType()==XmdvTool::DIAGPLOT_TWO_DIM_DATA_DRIVEN){
		// the diagonal display type is two dimension display
		currentDiag = new VisScatterDiagTwoDesDim(this);
	}else if(this->getDiagType()==XmdvTool::DIAGPLOT_TWO_DIM_DERIVED){
		// the diagonal display type is two dimension display
		currentDiag = new VisScatterDiagTwoDimDerived(this);
	}
	currentDiag->DrawScatters();
}

void VisScatter::setOneDimSortDim(int sortDim){
	this->getOkcVisualMapResult()->getScatterVisAttr()->setOneDimSortDim(sortDim);
}

int VisScatter::getOneDimSortDim(){
	return this->getOkcVisualMapResult()->getScatterVisAttr()->getOneDimSortDim();
}

void VisScatter::setDataDrivenXDimNumber(int xDimNumber){
	this->getOkcVisualMapResult()->getScatterVisAttr()->setDataDrivenXDimNumber(xDimNumber);
}

int VisScatter::getDataDrivenXDimNumber(){
	return this->getOkcVisualMapResult()->getScatterVisAttr()->getDataDrivenXDimNumber();
}

void VisScatter::setDataDrivenYDimNumber(int yDimNumber){
	this->getOkcVisualMapResult()->getScatterVisAttr()->setDataDrivenYDimNumber(yDimNumber);
}

int VisScatter::getDataDrivenYDimNumber(){
	return this->getOkcVisualMapResult()->getScatterVisAttr()->getDataDrivenYDimNumber();
}

void VisScatter::setDerivedXDimNumber(int xDimNumber){
	this->getOkcVisualMapResult()->getScatterVisAttr()->setDerivedXDimNumber(xDimNumber);
}

int VisScatter::getDerivedXDimNumber(){
	return this->getOkcVisualMapResult()->getScatterVisAttr()->getDerivedXDimNumber();
}

void VisScatter::setDerivedYDimNumber(int yDimNumber){
	this->getOkcVisualMapResult()->getScatterVisAttr()->setDerivedYDimNumber(yDimNumber);
}

int VisScatter::getDerivedYDimNumber(){
	return this->getOkcVisualMapResult()->getScatterVisAttr()->getDerivedYDimNumber();
}

// Map the x coordinate from data to screen
double VisScatter::XMAP(int DIM, double VAL)
{
	double result = scatter_X_axes[DIM] + ( VAL - m_dim_min[DIM] ) * xMapFactor[DIM];
	return result;
}

// Map the y coordinate from data to screen
double VisScatter::YMAP(int DIM, double VAL)
{
	double y_axes = scatter_Y_axes[DIM];
	double max = m_dim_max[DIM];
	double factor = yMapFactor[DIM];
	double result = y_axes + ( max - VAL) * factor;
	return result;
}

//reverse of xmap. map screen back to data
double VisScatter::XINVMAP(int DIM, double VAL)
{
	return (VAL-scatter_X_axes[DIM]) * (m_dim_max[DIM] - m_dim_min[DIM])
	/ (scatter_X_axes[DIM+1] - scatter_X_axes[DIM] )
	+ m_dim_min[DIM];
}


double VisScatter::YINVMAP(int DIM, double VAL)
{
	return m_dim_max[DIM] -
	( VAL - scatter_Y_axes[DIM] ) * ( m_dim_max[DIM] - m_dim_min[DIM] )
	/ ( scatter_Y_axes[DIM+1] - scatter_Y_axes[DIM] );
}

void VisScatter::inputScatterFindPlot(GLCanvas*, int sx, int sy, int &X_dim, int &Y_dim)
{
	Vec2 p = m_canvas->mapScreenToData( Vec2(sx,sy) );
	int i = -1, j = -1;

	while ( i < m_dim_size && p.X > scatter_X_axes[i+1] )
		i++;
	while ( j < m_dim_size && p.Y > scatter_Y_axes[j+1] )
		j++;

	X_dim = i;
	Y_dim = j;
	return;
}

void VisScatter::findResizeCorners(double x,double y)
{
	if ( this->getDiagType()!=XmdvTool::DIAGPLOT_ORIGINAL
			&& dimx == dimy){
		return;
	}

	Brush *brush = getBrush();

	// There are four different variables that could be resized
	// (minx, maxx, miny, maxy) the user can resize at most
	// two at once

	// Reset all resize flags
	resize_minx = resize_maxx = false;
	resize_miny = resize_maxy = false;

	// Map mouse x/y location to data coordinates
	double mx = VisScatter::XINVMAP(dimx, x);
	double my = VisScatter::YINVMAP(dimy, y);

	// If this is a diagonal plot (same dimension on both axes)
	// then average the x and y positions
	//
	if (dimx == dimy) mx = my = .5*(mx+my);

	// Check if user is resizing in x direction
	if (mx <= brush->Pos(dimx) - .25*brush->Size(dimx))
		resize_minx = true;
	else
		if (mx >= brush->Pos(dimx) + .25*brush->Size(dimx))
			resize_maxx = true;

	// Check if user is resizing in y direction
	if (my <= brush->Pos(dimy) - .25*brush->Size(dimy))
		resize_miny = true;
	else
		if (my >= brush->Pos(dimy) + .25*brush->Size(dimy))
			resize_maxy = true;

	if((resize_maxy&&resize_maxx) || (resize_miny&&resize_minx)) {
		setCursorShape(Qt::SizeBDiagCursor);
	}
	else if((resize_maxy&&resize_minx) || (resize_miny&&resize_maxx)) {
		setCursorShape(Qt::SizeFDiagCursor);
	}
	else if(resize_maxy||resize_miny) {
		setCursorShape(Qt::SizeVerCursor);
	}
	else if (resize_maxx || resize_minx) {
		setCursorShape(Qt::SizeHorCursor);
	}
}


void VisScatter::inputButtonPress(int button, int x, int y)
{
	if (!getOkcVisualMapResult()) {
		return;
	}

	MouseKeyboardStatus* mouseKey = getMouseKeyboardStatus();
	Brush *brush = getBrush();

	if (mouseKey->button[2]==true) {
		m_input_mode = CANVAS_PAN;
		return;
	}

	Vec2 p = m_canvas->mapScreenToData( Vec2(x,y) );

	inputScatterFindPlot(m_canvas, x, y, dimx, dimy);

	if (dimx < 0 || dimx >= m_dim_size || dimy < 0 || dimy >= m_dim_size) {
		dimx = dimy = -1;
		return;
	}

	if ( this->getDiagType()!=XmdvTool::DIAGPLOT_ORIGINAL
			&& dimx == dimy){
		return;
	}

	if (mouseKey->shiftkey) {
		// Entering the painting mode.
		// Record the current position as ranges of the new brush.
		// When users move the mouse to other positions, the ranges
		// will be extended to contain new positions.
		m_input_mode = MULTI_SELECT;
		m_dimx_multi_select = dimx;
		m_dimy_multi_select = dimy;
		Vec2 mouse_pos = m_canvas->mapScreenToData(Vec2(x, y));
		double mouseX = VisScatter::XINVMAP(dimx, mouse_pos.X);
		double mouseY = VisScatter::YINVMAP(dimy, mouse_pos.Y);
		m_dimx_multi_select_min = mouseX;
		m_dimx_multi_select_max = mouseX;
		m_dimy_multi_select_min = mouseY;
		m_dimy_multi_select_max = mouseY;
		return;
	} else if (mouseKey->controlkey && brush->bound_type == Brush::RAMP && button == 0) {
		m_input_mode = RESIZE_RAMP;
		findResizeCorners(p.X,p.Y);
		return;
	}

	if (mouseKey->button[0] == true) {
		m_input_mode = RESIZE_BRUSH;
		findResizeCorners(p.X,p.Y);
	} else if (mouseKey->button[1] == true) {
		m_input_mode = MOVE_BRUSH;
	}

	inputMouseMotion(x,y);
}

void VisScatter::inputMouseMotion(int x, int y)
{
	if (!getOkcVisualMapResult()) {
		return;
	}

	int pdx, pdy, dx, dy;
	inputScatterFindPlot(m_canvas, x, y, pdx, pdy);
	dx = CLAMP(pdx, 0, m_dim_size - 1);
	dy = CLAMP(pdy, 0, m_dim_size - 1);
	if (pdx == dx && pdy == dy) {
		if (getDiagType() == XmdvTool::DIAGPLOT_ORIGINAL || dx != dy) {
			char buffer[100];
			buffer[0] = 0;
			char *dimXName = m_dim_names[dx];
			char *dimYName = m_dim_names[dy];

			Vec2 p = m_canvas->mapScreenToData( Vec2(x, y) );
			double mx = VisScatter::XINVMAP(dx, p.X);
			double my = VisScatter::YINVMAP(dy, p.Y);

			char dimValueXChar [30];
			char dimValueYChar [30];
			sprintf(dimValueXChar,"%.3lf",mx);
			sprintf(dimValueYChar,"%.3lf",my);

			strcat(buffer, dimXName);
			strcat(buffer, "=");
			strcat(buffer, dimValueXChar);
			strcat(buffer, ", ");
			strcat(buffer, dimYName);
			strcat(buffer, "=");
			strcat(buffer, dimValueYChar);
			this->getViewManager()->getMainWnd()->statusBar->showMessage(
					QString(buffer));
		}
	}else{
		this->getViewManager()->getMainWnd()->statusBar->showMessage(QString(""));
	}

	static int prev_sx, prev_sy;

	switch (m_input_mode) {
	case MULTI_SELECT:
		inputScatterMultiSelect(m_canvas,x,y);
		return;

	case RESIZE_BRUSH:
		inputScatterResizeBrush(m_canvas,x,y);
		break;

	case MOVE_BRUSH:
		setCursorShape(Qt::SizeAllCursor);
		inputScatterMoveBrush(m_canvas,x,y);
		break;

	case RESIZE_RAMP:
		break;

	case CANVAS_PAN:
		break;

	default: break;
	}
	prev_sx = x;
	prev_sy = y;
}

//jing this func is used by canvas->.. _proc
void VisScatter::inputScatterMoveBrush(GLCanvas *canvas, int sx, int sy)
{
	Vec2 p = canvas->mapScreenToData( Vec2(sx,sy) );

	OkcVisualMapResult *vmr = getOkcVisualMapResult();
	Brush *brush = vmr->getBrush();

	// Map mouse x/y location to data coordinates
	double mx = VisScatter::XINVMAP(dimx, p.X);
	double my = VisScatter::YINVMAP(dimy, p.Y);

	// If this is a diagonal plot (same dimension on both axes)
	// then average the x and y positions
	//
	if (dimx == dimy) mx = my = .5*(mx+my);

	// Bound mouse to plot edges
	//
	mx = MIN(mx, m_dim_max[dimx] - brush->Size(dimx)*.5);
	mx = MAX(mx, m_dim_min[dimx] + brush->Size(dimx)*.5);

	my = MIN(my, m_dim_max[dimy] - brush->Size(dimy)*.5);
	my = MAX(my, m_dim_min[dimy] + brush->Size(dimy)*.5);

	brush->SetPos(dimx, mx);
	brush->SetPos(dimy, my);

	drawScatterBrushBox(canvas, dimx, dimy, mx, my, brush->Size(dimx), brush->Size(dimy));
	// Do all other graphs along the horizontal
	for (int x=0; x<m_dim_size; x++)
	{
		drawScatterBrushBox(canvas, x, dimy, brush->Pos(x), my,
				brush->Size(x), brush->Size(dimy));
	}

	// Do all other graphs along the vertical
	for (int y=0; y<m_dim_size; y++)
	{
		drawScatterBrushBox(canvas, dimx, y, mx, brush->Pos(y),
				brush->Size(dimx), brush->Size(y));
	}

	this->refreshBrush();
}

void VisScatter::inputScatterResizeBrush(GLCanvas *canvas, int sx, int sy)
{
	Vec2 p = canvas->mapScreenToData( Vec2(sx,sy) );

	// Map mouse x/y location to data coordinates
	double mx = VisScatter::XINVMAP(dimx, p.X);
	double my = VisScatter::YINVMAP(dimy, p.Y);

	// If this is a diagonal plot (same dimension on both axes)
	// then average the x and y positions
	//
	if (dimx == dimy) mx = my = .5*(mx+my);

	// Bound mouse to plot edges
	//
	mx = MIN(mx, m_dim_max[dimx]);
	mx = MAX(mx, m_dim_min[dimx]);
	my = MIN(my, m_dim_max[dimy]);
	my = MAX(my, m_dim_min[dimy]);

	OkcVisualMapResult *vmr = getOkcVisualMapResult();
	Brush *brush = vmr->getBrush();

	// Check to make sure min<max
	if (resize_minx && mx >= brush->Max(dimx))
		mx = brush->Max(dimx);
	if (resize_maxx && mx <= brush->Min(dimx))
		mx = brush->Min(dimx);
	if (resize_miny && my >= brush->Max(dimy))
		my = brush->Max(dimy);
	if (resize_maxy && my <= brush->Min(dimy))
		my = brush->Min(dimy);

	// Find the new bounds based on what is being resized
	//
	double new_minx, new_maxx, new_miny, new_maxy;

	if (resize_minx) new_minx = mx;
	else new_minx = brush->Min(dimx);

	if (resize_maxx) new_maxx = mx;
	else new_maxx = brush->Max(dimx);

	if (resize_miny) new_miny = my;
	else new_miny = brush->Min(dimy);

	if (resize_maxy) new_maxy = my;
	else new_maxy = brush->Max(dimy);

	if (resize_minx || resize_maxx)
	{
		brush->SetMinMax(dimx,new_minx,new_maxx);
	}
	if (resize_miny || resize_maxy)
	{
		brush->SetMinMax(dimy,new_miny,new_maxy);
	}

	// Do the graph being manipulated
	//canvas->SetXorMode(true);
	drawScatterBrushBox(canvas, dimx, dimy,
			(new_maxx + new_minx)/2,
			(new_maxy + new_miny)/2,
			(new_maxx - new_minx),
			(new_maxy - new_miny));

	// Do all other graphs along the horizontal
	for (int x=0; x<m_dim_size; x++)
	{
		if ( this->getDiagType()!=XmdvTool::DIAGPLOT_ORIGINAL
				&&  (x==dimx || x==dimy)) continue;

		drawScatterBrushBox(canvas, x, dimy,
				brush->Pos(x),
				(new_maxy + new_miny)*.5,
				brush->Size(x),
				(new_maxy - new_miny));
	}

	// Do all other graphs along the vertical
	for (int y=0; y<m_dim_size; y++)
	{
		if ( this->getDiagType()!=XmdvTool::DIAGPLOT_ORIGINAL
				&& (y==dimx || y==dimy)) continue;

		drawScatterBrushBox(canvas, dimx, y,
				(new_maxx + new_minx)*.5,
				brush->Pos(y),
				(new_maxx - new_minx),
				brush->Size(y));
	}

	this->refreshBrush();
}

void VisScatter::inputScatterMultiSelect(GLCanvas *canvas, int sx, int sy)
{
	if ( dimx == m_dimx_multi_select && dimy == m_dimy_multi_select ) {
		Vec2 mouse_pos = canvas->mapScreenToData(Vec2(sx, sy));
		double mouseX = VisScatter::XINVMAP(dimx, mouse_pos.X);
		double mouseY = VisScatter::YINVMAP(dimy, mouse_pos.Y);

		m_dimx_multi_select_min = MIN(m_dimx_multi_select_min, mouseX);
		m_dimx_multi_select_max = MAX(m_dimx_multi_select_max, mouseX);
		m_dimy_multi_select_min = MIN(m_dimy_multi_select_min, mouseY);
		m_dimy_multi_select_max = MAX(m_dimy_multi_select_max, mouseY);
	}

}


void VisScatter::multiSelectAdjustBrush(){

	Brush *brush = getBrush();
	OkcVisualMapResult* vmr = getOkcVisualMapResult();
	int dataSize = vmr->getDataSize();
	int i,j;
	std::vector<double> data;

	// For each datapoint, if its values on dimension
	// m_dimx_multi_select and m_dimy_multi_select
	// fall into the ranges that users selected, we
	// use its values on other dimensions to set the position of the brush
	// on other dimensions.
	bool first = true;
	for (i=0; i<dataSize; i++) {
		vmr->getDataItem(data, i);
		if ( data[m_dimx_multi_select] > m_dimx_multi_select_min - XMDV_DBL_EPSILON
				&& data[m_dimx_multi_select] < m_dimx_multi_select_max + XMDV_DBL_EPSILON
				&& data[m_dimy_multi_select] > m_dimy_multi_select_min - XMDV_DBL_EPSILON
				&& data[m_dimy_multi_select] < m_dimy_multi_select_max + XMDV_DBL_EPSILON ) {
			// If this is the first datapoint falling into the range,
			// we use it to initialize the brush, and then
			// for other datapoints within the range, we use their values to
			// update the brush
			if ( first ) {
				for (j = 0; j < m_dim_size; j++) {
					brush->SetMinMax(j, data[j], data[j]);
				}
				first = false;
			} else {
				for (j = 0; j < m_dim_size; j++) {
					double min = MIN(brush->min[j], data[j]);
					double max = MAX(brush->max[j], data[j]);
					brush->SetMinMax(j, min, max);
				}
			}
		}

	}
}

bool VisScatter::betweenTwoPoint(double startPos, double endPos, double inputPos){
	if((endPos-startPos)*(inputPos-startPos)<0.0){
		return false;
	}else {
		if(abs(inputPos-startPos)>abs(endPos-startPos)){
			return false;
		}
	}
	return true;
}

double VisScatter::abs(double v){
	if(v>=0){
		return v;
	}else{
		return -1.0*v;
	}
}

void VisScatter::drawScatterBrushBox(GLCanvas *canvas, int dimx, int dimy, double data_x, double data_y,
		double size_x, double size_y)
{
	Vec2 p, q;

	// lower left
	p.X = VisScatter::XMAP(dimx, data_x - size_x/2);
	p.Y = VisScatter::YMAP(dimy, data_y - size_y/2);

	// upper right
	q.X = VisScatter::XMAP(dimx, data_x + size_x/2);
	q.Y = VisScatter::YMAP(dimy, data_y + size_y/2);

	canvas->drawRectangle(p.X,p.Y,q.X-p.X,q.Y-p.Y);
}

void VisScatter::inputButtonRelease(int, int, int) {

	if (!getOkcVisualMapResult()) {
		return;
	}
	switch (m_input_mode) {
	case CANVAS_PAN:
		break;

	case MULTI_SELECT:
		multiSelectAdjustBrush();
		break;

	case RESIZE_RAMP:
	case RESIZE_BRUSH_TOP:
	case RESIZE_BRUSH_BOTTOM:
	case TRANSLATE_BRUSH:
		break;

	default:
		break;
	}

	setCursorShape(Qt::ArrowCursor);

	this->refreshBrush();
	m_input_mode = NONE;

	dimx = -1;
	dimy = -1;
}
