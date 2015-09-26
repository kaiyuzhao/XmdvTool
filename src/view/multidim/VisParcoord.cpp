#include "view/multidim/VisParcoord.h"

#include <math.h>
#include <typeinfo>
#include <assert.h>
#include <vector>
#include <QGLWidget>
#include <QtOpenGL/QtOpenGL>

#include "main/XmdvTool.h"
#include "datatype/RGBt.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "pipeline/PipelineManager.h"
#include "operator/brush/Brush.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "visualmap/VisualMapResult.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/GLCanvas.h"
#include "view/GLDisplay.h"
#include "view/ViewDisplay.h"
#include "view/ViewWindow.h"
#include "view/multidim/OkcViewDisplay.h"
#include "interaction/MouseKeyboardStatus.h"
#include "color/ColorManager.h"

#include "main/XmdvTool.h"

#include <QtGui>
#include <QtOpenGL>
#include <QPalette>

VisParcoord::VisParcoord(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm)
	: OkcViewDisplay(viewWindow, pm, pipelineID, vm) {
	m_firstTime = true;
}

VisParcoord::~VisParcoord()
{
}

void VisParcoord::setParameters() {
	// call OkcViewDisplay::setParameters() to initialize
	// parameters about dataset
	OkcViewDisplay::setParameters();

	int DIM;
    m_yMapFactor.resize(m_dim_size);
    for (DIM = 0; DIM < m_dim_size; ++DIM)
	{
		m_yMapFactor[DIM] =  1.0 / ( m_dim_max[DIM] - m_dim_min[DIM] );
	}

    // Call setPlacement after initializing m_dim_size,
    // because setPlacement needs this value
    setPlacement();
	start_multi_select = false;
}

bool VisParcoord::testDimensionality() {
	return (m_dim_size>=2);
}

void VisParcoord::setPlacement()
{
	// If the dataset has 0 dimensions, do nothing
	if (m_dim_size==0) return;

	//reset screen distortion parameters for parallel coordinate display
    paracoord_axes.resize(m_dim_size);
    paracoord_interval.resize(m_dim_size);

    double width = 0.0;
    width = 1.0/(double)m_dim_size;
    paracoord_axes[0] = width/2.0;

    int i;
    for ( i = 0; i < m_dim_size-1; i++ )
    {
        paracoord_axes[i+1] = paracoord_axes[i] +  width;
        paracoord_interval[i] = width;
    }
	paracoord_interval[i] = 1-paracoord_axes[i];
}


//draw the axes for parallel coordinates
void VisParcoord::drawAxes() {
	drawParcoordAxes();
}

//draw single data
void VisParcoord::drawSingleData(std::vector<double> &data, int) {
	int j;

	glLineWidth(1.0f);
	glBegin(GL_LINE_STRIP);
	for (j = 0; j < m_dim_size; j++) {
		glVertex2f((float) XMAP(j), (float) YMAP(j, data[j]));
	}
	glEnd();
}

void VisParcoord::drawSingleBand(RGBt & color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int) {
	std::vector<double> mapedX, mapedYMean, mapedYMax, mapedYMin;
	mapedX.resize(m_dim_size);
	mapedYMean.resize(m_dim_size);
	mapedYMax.resize(m_dim_size);
	mapedYMin.resize(m_dim_size);
	int d;

	for (d=0; d<m_dim_size; d++)
	{
		mapedX[d] = XMAP(d);
        mapedYMean[d] = YMAP(d, data[d]);
		mapedYMax[d] = YMAP(d, max[d]);
		mapedYMin[d] = YMAP(d, min[d]);
		if ( fabs(1 - m_hier_info->extent_scaling_fac ) > 1e-4 )
		{
			mapedYMax[d] = mapedYMean[d] + m_hier_info->extent_scaling_fac * (mapedYMax[d] - mapedYMean[d]);
			mapedYMin[d] = mapedYMean[d] + m_hier_info->extent_scaling_fac * (mapedYMin[d] - mapedYMean[d]);
		}
	}

	m_canvas->setForeground(color, m_opacity);

	std::vector<Vec2> band(4);
	Vec2 lo, hi;

	// Draw the lower band
    lo.X = mapedX[0];
    lo.Y = mapedYMin[0];

    hi.X = lo.X;
    hi.Y = mapedYMean[0];

    band[0] = lo;
    band[3] = hi;
    for (d=1; d<m_dim_size; d++)
    {
        lo.X = mapedX[d];
        lo.Y = mapedYMin[d];

        hi.X = lo.X;
        hi.Y = mapedYMean[d];

        band[1] = lo;
        band[2] = hi;
        RGBt fgcolor = m_canvas->fg_color;
        m_canvas->drawTransparentBand(band, 0.0, m_hier_info->density, m_canvas->fg_color, m_canvas->fg_color);

        band[0] = band[1];
        band[3] = band[2];
    }

    // Draw the upper band
    lo.X = mapedX[0];
    lo.Y = mapedYMean[0];

    hi.X = lo.X;
    hi.Y = mapedYMax[0];
    band[0] = lo;
    band[3] = hi;

    for (d=1; d<m_dim_size; d++)
    {
        lo.X = mapedX[d];
        lo.Y = mapedYMean[d];

        hi.X = lo.X;
        hi.Y = mapedYMax[d];

        band[1] = lo;
        band[2] = hi;

        m_canvas->drawTransparentBand(band, m_hier_info->density, 0.0, m_canvas->fg_color, m_canvas->fg_color);
        //m_canvas->drawTransparentBand(band, 0.0, m_hier_info->density, m_canvas->fg_color, m_canvas->fg_color);

        band[0] = band[1];
        band[3] = band[2];
	}


}


void VisParcoord::drawParcoordAxes()
{
    glLineWidth(1.);
    glShadeModel(GL_FLAT);
    m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::GRID1));

    // Draw the vertical axes
    int i;
    for (i=0; i<m_dim_size; i++) {
    	double x = XMAP(i);
        m_canvas->drawLine((float)x, 0.0f, (float)x, 1.0f);
    }

    drawParcoordAxesNameMinMax();
}

void VisParcoord::drawParcoordAxesNameMinMax()
{
    m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::TEXT));

    int i,j;
    char sTemp[100];

    Vec2 pos_a, pos_b;
    pos_a = m_canvas->mapDataToScreen(Vec2(paracoord_axes[0],0.0));
    pos_b = m_canvas->mapDataToScreen(Vec2(paracoord_axes[1],0.0));

    double dis_between_axes = pos_b.X - pos_a.X;

    double letter_dis_x = 2.8;
    double letter_dis_y = 0.5;

    for (i=0; i<m_dim_size; i++)
	{
    	double x = paracoord_axes[i];
    	Vec2 pos, pos_1;
    	pos = m_canvas->mapDataToScreen(Vec2(x,0.0));
    	pos_1 = m_canvas->mapDataToScreen(Vec2(x,0.0));


		// initial coordinates
    	pos.Y -= 2.0*g_globals.textFont.pointSize();
    	pos_1.Y -= 2.0*g_globals.textFont.pointSize();

		// This loop draw the dimension name letter by letter.
		for (j=0; j<int(strlen(m_dim_names[i])); j++)
		{
			// If the vertical position is beyond the range of this dimension,
			// skip the other letters
			if ( pos_1.X > pos.X + dis_between_axes + letter_dis_x * g_globals.textFont.pointSize() )
				break;
			m_canvas->drawChar((int)pos_1.X, (int)pos_1.Y, m_dim_names[i][j]);
			pos_1.X += g_globals.textFont.pointSize();
			pos_1.Y -= letter_dis_y*g_globals.textFont.pointSize();
		}
	}

    for (i=0; i<m_dim_size; i++)
    {
    	double x = paracoord_axes[i];

    	Vec2 pos;
    	//this is the position to draw dimension names and maximum values
    	pos = m_canvas->mapDataToScreen(Vec2(x,0.0));

       /* m_canvas->drawString((int)pos.X
        		, (int)(pos.Y-2.0*g_globals.textFont.pointSize()), m_dim_names[i]);*/

        sprintf(sTemp, "%g", m_dim_max[i]);
        m_canvas->drawString((int)pos.X,
        		(int)(pos.Y-0.5*g_globals.textFont.pointSize()), sTemp);

        sprintf(sTemp, "%g", m_dim_min[i]);
    	//this is the position to draw minimum values
    	pos = m_canvas->mapDataToScreen(Vec2(x,1.0));
        m_canvas->drawString((int)pos.X,
        		(int)(pos.Y+1.5*g_globals.textFont.pointSize()), sTemp);

    }
}

void VisParcoord::drawSingleBrush(Brush *brush)
{
	int j;

	m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::BRUSH_BG1));
	m_canvas->beginDraw();
	glBegin(GL_QUAD_STRIP);
	for (j = 0; j < m_dim_size; j++){
		Vec2 lo, hi;

		// x positions are fixed y positions normalized over axis height

		// top points
		hi.X = VisParcoord::XMAP(j);
		hi.Y = VisParcoord::YMAP(j, brush->max[j]);

		// bottom points
		lo.X = VisParcoord::XMAP(j);
		lo.Y = VisParcoord::YMAP(j, brush->min[j]);

		glVertex2f((float) lo.X, (float) lo.Y);
		glVertex2f((float) hi.X, (float) hi.Y);
	}
	glEnd();
	m_canvas->endDraw();

	// draw boundary coverage if necessary
	//
	if (brush->bound_type != Brush::STEP) {
		drawParcoordBrushBoundRamp(m_canvas,
								   brush,
								   g_globals.colorManager->getColor(ColorManager::BRUSH_BG1),
								   g_globals.colorManager->getColor(ColorManager::BG));
	}
}

// Draw the brush boundary in the parallel coordinate view.
void VisParcoord::drawParcoordBrushBoundRamp(GLCanvas*,
											   Brush *brush,
											   unsigned long color1,
											   unsigned long color2)
{
	int i;
	double factor = 0.1;
	RGBt c1( color1), c2(color2);
	c2.R = factor * c1.R + (1 - factor) * c2.R;
	c2.G = factor * c1.G + (1 - factor) * c2.G;
	c2.B = factor * c1.B + (1 - factor) * c2.B;

	m_canvas->beginDraw();

	glBegin(GL_QUAD_STRIP);
	for (i = 0; i < m_dim_size; i++) {
		double x = VisParcoord::XMAP(i);
		double y1 = VisParcoord::YMAP(i, brush->RampMax(i));
		double y2 = VisParcoord::YMAP(i, brush->Max(i));

		glColor3f((float) c2.R, (float) c2.G, (float) c2.B);
		glVertex2f((float) x, (float) y1);
		glColor3f((float) c1.R, (float) c1.G, (float) c1.B);
		glVertex2f((float) x, (float) y2);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (i = 0; i < m_dim_size; i++) {
		double x = VisParcoord::XMAP(i);
		double y1 = VisParcoord::YMAP(i, brush->RampMin(i));
		double y2 = VisParcoord::YMAP(i, brush->Min(i));

		glColor3f((float) c2.R, (float) c2.G, (float) c2.B);
		glVertex2f((float) x, (float) y1);
		glColor3f((float) c1.R, (float) c1.G, (float) c1.B);
		glVertex2f((float) x, (float) y2);
	}
	glEnd();

	m_canvas->endDraw();
}


double VisParcoord::XMAP(int I)
{
    return paracoord_axes[I];
}

double VisParcoord::YMAP(int X, double Y)
{
    return 1.0 - (Y - m_dim_min[X]) * m_yMapFactor[X];
}

void VisParcoord::inputButtonPress(int button, int x, int y){
	// If the visual mapping result is NULL, do noting and return
	if (!this->getVisualMapResult()) {
		return;
	}

	MouseKeyboardStatus* m_mouseKeyboard = this->getMouseKeyboardStatus();
	if ((!m_mouseKeyboard->altkey) && (!m_mouseKeyboard->controlkey) && m_mouseKeyboard->button[2] == true) {
		m_input_mode = CANVAS_PAN;
		// change cursor shape in GLDisplay
		//m_canvas->setHandCursor();
		return;
	}

	Vec2 p = m_canvas->mapScreenToData(Vec2(x, y));
	Brush *brush = getBrush();
	inputParacoordFindAxesNear(m_canvas, p.X, p.Y, axis);
	// If axis is not a valid value, do noting and return
	if (axis < 0 || axis >= m_dim_size) {
		axis = -1;
		return;
	}

	if (m_mouseKeyboard->shiftkey && button == 0) {
		m_input_mode = MULTI_SELECT;
		inputParcoordMultiSelect(m_canvas, x, y);
		return;
	} else if (m_mouseKeyboard->controlkey && brush->bound_type
			== Brush::RAMP && button == 0) {
		m_input_mode = RESIZE_RAMP;
		inputParcoordBoundRampResize(m_canvas, x, y);
		return;
	}

	if (m_mouseKeyboard->button[0] == true) {
		double lo = VisParcoord::YMAP(axis, brush->min[axis]);
		double hi = VisParcoord::YMAP(axis, brush->max[axis]);
		if ((hi - p.Y) > (p.Y - lo)) {
			m_input_mode = RESIZE_BRUSH_TOP;
		} else {
			m_input_mode = RESIZE_BRUSH_BOTTOM;
		}
	} else if (m_mouseKeyboard->button[1] == true) {
		m_input_mode = TRANSLATE_BRUSH;
	}

	inputMouseMotion(x, y);

}

void VisParcoord::inputMouseMotion(int x, int y)
{
	// If the visual mapping result is NULL, do noting and return
	if (!this->getOkcVisualMapResult()) {
		return;
	}

	char buffer [100];
	buffer[0] = 0;

	int dimAxis = -1;

	Vec2 p = m_canvas->mapScreenToData(Vec2(x, y));
	inputParacoordFindAxesNear(m_canvas, p.X, p.Y, dimAxis);
	if (dimAxis < 0 || dimAxis >= m_dim_size) {
		dimAxis = -1;
		//return;
	}else{
		char *temp = m_dim_names[dimAxis];
		strcat(buffer, temp);
		strcat(buffer, "=");
		double dimValue = VisParcoord::YINVMAP(dimAxis, p.Y);
		char dimValueChar [30];
		sprintf(dimValueChar,"%.3lf",dimValue);
		strcat(buffer, dimValueChar);
	}

	this->getViewManager()->getMainWnd()->statusBar->showMessage(QString(buffer));

	static int prev_sx, prev_sy;
	switch (m_input_mode) {
	case CANVAS_PAN:
		//appCanvasMousePan(canvas);
		return;

	case RESIZE_RAMP:
		/*if (xor_needs_undo)
			InputParcoordBoundRampResize(canvas, prev_sx, prev_sy);
		InputParcoordBoundRampResize(canvas, x, y);*/
		break;

	case RESIZE_BRUSH_TOP:
	case RESIZE_BRUSH_BOTTOM:
		//if (xor_needs_undo)
		//	InputParcoordResizeBrush(canvas, prev_sx, prev_sy);
		setCursorShape(Qt::SizeVerCursor);
		inputParcoordResizeBrush(m_canvas, x, y);
		break;

	case TRANSLATE_BRUSH:
		/*if (xor_needs_undo)
			InputParcoordMoveBrush(canvas, prev_sx, prev_sy);*/
		setCursorShape(Qt::SizeVerCursor);
		inputParcoordMoveBrush(m_canvas, x, y);
		break;

	case MULTI_SELECT:
		inputParcoordMultiSelect(m_canvas, x, y);
		return;

	default:
		break;
	}

	prev_sx = x;
	prev_sy = y;
}

//this function is used by InputParcoordMouseMotion
void VisParcoord::inputParcoordMoveBrush(GLCanvas *canvas, int sx, int sy)
{
	if (axis < 0 || axis >=m_dim_size) {
		return;
	}

	Vec2 p = m_canvas->mapScreenToData(Vec2(sx, sy));

	Brush *brush = getBrush();

	double mx = VisParcoord::XMAP(axis);
	double y = VisParcoord::YINVMAP(axis, p.Y);

	y = MIN(y, m_dim_max[axis] - brush->Size(axis) * .5);
	y = MAX(y, m_dim_min[axis] + brush->Size(axis) * .5);

	double mly = VisParcoord::YMAP(axis, y - brush->Size(axis) * .5);
	double mhy = VisParcoord::YMAP(axis, y + brush->Size(axis) * .5);

	if (axis > 0) {
		double lx = VisParcoord::XMAP(axis - 1);
		int leftDim = axis - 1;
		double lly = VisParcoord::YMAP(axis - 1,
										 brush->Min(leftDim));
		double lhy = VisParcoord::YMAP(axis - 1,
										 brush->Max(leftDim));
		canvas->drawLine((float)lx, (float)lly, (float)mx, (float)mly);
		canvas->drawLine((float)lx, (float)lhy, (float)mx, (float)mhy);
	}

	if (axis < m_dim_size-1) {
		double rx = VisParcoord::XMAP(axis + 1);
		int rightDim = axis + 1;
		double rly = VisParcoord::YMAP(axis + 1,
										 brush->Min(rightDim));
		double rhy = VisParcoord::YMAP(axis + 1,
										 brush->Max(rightDim));
		canvas->drawLine((float)rx, (float)rly, (float)mx, (float)mly);
		canvas->drawLine((float)rx, (float)rhy, (float)mx, (float)mhy);
	}

	brush->SetPos(axis, y);

	this->refreshBrush();
}

void VisParcoord::inputParcoordResizeBrush(GLCanvas *canvas, int sx, int sy) {
	if (axis < 0 || axis >= m_dim_size) {
		return;
	}
	Vec2 p = canvas->mapScreenToData(Vec2(sx, sy));

	// Clamp y coordinate to within dimension limits
	double y = VisParcoord::YINVMAP(axis, p.Y);
	y = MAX(y, m_dim_min[axis]);
	y = MIN(y, m_dim_max[axis]);

	double mx = VisParcoord::XMAP(axis);

	Brush *brush = getBrush();

	switch (m_input_mode) {
	case RESIZE_BRUSH_TOP: {
		double ly = brush->min[axis];
		double hy = MAX(y, ly);
		p.Y = VisParcoord::YMAP(axis, hy);

		brush->SetMinMax(axis, ly, hy);

		// Resize top
		//
		if (axis > 0) {
			double lx = VisParcoord::XMAP(axis - 1);
			int leftDim = axis - 1;
			double lhy = VisParcoord::YMAP(axis - 1, brush->max[leftDim]);
			canvas->drawLine((float) lx, (float) lhy, (float) mx, (float) p.Y);
		}

		if (axis < m_dim_size - 1) {
			double rx = VisParcoord::XMAP(axis + 1);
			int rightDim = axis + 1;
			double rhy = VisParcoord::YMAP(axis + 1, brush->max[rightDim]);
			canvas->drawLine((float) rx, (float) rhy, (float) mx, (float) p.Y);
		}
	}
		break;

	case RESIZE_BRUSH_BOTTOM: {
		double hy = brush->max[axis];
		double ly = MIN(y, hy);
		p.Y = VisParcoord::YMAP(axis, ly);

		brush->SetMinMax(axis, ly, hy);

		// Resize bottom
		//
		if (axis > 0) {
			double lx = VisParcoord::XMAP(axis - 1);
			int leftDim = axis - 1;
			double lly = VisParcoord::YMAP(axis - 1, brush->min[leftDim]);
			canvas->drawLine((float) lx, (float) lly, (float) mx, (float) p.Y);
		}

		if (axis < m_dim_size - 1) {
			double rx = VisParcoord::XMAP(axis + 1);
			int rightDim = axis + 1;
			double rly = VisParcoord::YMAP(axis + 1, brush->min[rightDim]);
			canvas->drawLine((float) rx, (float) rly, (float) mx, (float) p.Y);
		}
	}
		break;

	default:
		break;
	}

	this->refreshBrush();
}

double VisParcoord::YINVMAP(int X, double Y)
{
    return ((1.0-(Y)) * (m_dim_max[X] - m_dim_min[X])) + m_dim_min[X];
}

void VisParcoord::inputParacoordFindAxesNear(GLCanvas*, double X, double Y, int &dim)
{
	if ( Y < 0. || Y > 1.0 )
    {
    	dim = -1;
        return;
    }
    else
    {
        if ( X < 0. || X > 1.0 )
        {
        	dim = -1;
            return;
        }

        for ( int i = 0; i < m_dim_size-1; i++ )
        {
        	if ( X < ( paracoord_axes[i] + paracoord_interval[i]/2 ) )
            {
                dim = i;
                return;
            }
        }

        dim = m_dim_size-1;
        return;
    }
}

//this function is used by InputParcoordMouseMotion & InputParcoordButtonPress
void VisParcoord::inputParcoordMultiSelect(GLCanvas*,
		int sx,
		int sy)
{
	if (axis < 0 || axis >= m_dim_size) {
		return;
	}

	MouseKeyboardStatus* mouseKey = getMouseKeyboardStatus();
	Vec2 press_pos = m_canvas->mapScreenToData(mouseKey->press_pos);
	Vec2 p = m_canvas->mapScreenToData(Vec2(sx, sy));

	// Clamp y coordinate to within dimension limits
	double y = VisParcoord::YINVMAP(axis, p.Y);
	y = MAX(y, m_dim_min[axis]);
	y = MIN(y, m_dim_max[axis]);

	double pressY = VisParcoord::YINVMAP(axis, press_pos.Y);
	pressY = MAX(pressY, m_dim_min[axis]);
	pressY = MIN(pressY, m_dim_max[axis]);

	std::vector<double> dataItem(m_dim_size);
	start_multi_select = false;

	// scan all of data in the OkcData to be visualized
	// and select those falling between press_pos and current position
	OkcVisualMapResult* vmr = getOkcVisualMapResult();
	Data* data = vmr->getData();
	assert ( typeid(*data)==typeid(OkcData));
	OkcData* okcdata = dynamic_cast<OkcData*>(data);
	int i;
	for (i = 0; i < m_data_size; i++) {
		okcdata->getData(dataItem, i);

		double curDataY = dataItem[axis];
		if (betweenTwoPoint(pressY, y, curDataY)){
			multiSelectAdjustBrush(i);
		}
	}

}

void VisParcoord::multiSelectAdjustBrush(int point){
	OkcVisualMapResult* vmr = getOkcVisualMapResult();
	Brush *brush = vmr->getBrush();

	std::vector<double> data(vmr->getDataSize());

	double ly,hy;
	int i;
	vmr->getDataItem(data, point);
	if (!start_multi_select) {
		for (i = 0; i < m_dim_size; i++) {
			ly = data[i];
			hy = data[i];
			brush->SetMinMax(i, ly, hy);
		}
		start_multi_select = true;
	} else {
		for (i = 0; i < m_dim_size; i++) {
			ly = MIN(brush->min[i], data[i]);
			hy = MAX(brush->max[i], data[i]);
			brush->SetMinMax(i, ly, hy);
		}
	}
}


//this func is used by InputParcoordMouseMotion & InputParcoordButtonPress
void VisParcoord::inputParcoordBoundRampResize(GLCanvas*,int, int) {
}

void VisParcoord::inputButtonRelease(int, int, int){
	if (!this->getOkcVisualMapResult()) {
		return;
	}

	switch (m_input_mode) {
	case CANVAS_PAN:
		break;

	case MULTI_SELECT:
		start_multi_select = false;
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

	// re-assemble the pipeline starting from the BrushOperator
	this->refreshBrush();

	m_input_mode = NONE;
}
