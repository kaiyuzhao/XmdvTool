/*
 * VisScatter.h
 *
 *  Created on:Apr 1, 2008
 *      Author: Zaixian Xie, Zhenyu Guo
 */

/*
 * class VisScatter renders scatterplot matrices
 * and manages its interaction
 */

#ifndef VISSCATTER_H_
#define VISSCATTER_H_

#include <vector>
#include "view/multidim/OkcViewDisplay.h"
#include "view/multidim/VisScatterDiag.h"
#include "main/XmdvTool.h"

class ViewWindow;
class VisualMapResult;
class PipelineManage;
class GLCanvas;
class ViewManager;
class VisScatterDiag;

class VisScatter : public OkcViewDisplay
{
	friend class ScatterplotCustom;
	friend class VisScatterDiag;
	friend class VisScatterDiagHisto;
	friend class VisScatterDiagOneDim;
	friend class VisScatterDiagTwoDim;
	friend class VisScatterDiagTwoDesDim;
	friend class VisScatterDiagTwoDimDerived;

public:
	VisScatter(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm);
	virtual ~VisScatter();

	int xMinDim, xMaxDim;
	int yMinDim, yMaxDim;

	int dimx, dimy;
	bool resize_minx, resize_maxx, resize_miny, resize_maxy;

	VisScatterDiag *currentDiag;

private:
	// The positions of axes
    std::vector<double> scatter_X_axes;
    std::vector<double> scatter_Y_axes;
    std::vector<double> scatter_X_interval;
    std::vector<double> scatter_Y_interval;

    // The coefficients to map data to screen
    std::vector<double> xMapFactor, yMapFactor;

    // The variables used under painting (multi select) mode
    //
    // The dimensions X and Y for the scatterplot under painting mode
	int m_dimx_multi_select;
	int m_dimy_multi_select;
	// The min and max values for dimensions X and Y
	double m_dimx_multi_select_min;
	double m_dimx_multi_select_max;
	double m_dimy_multi_select_min;
	double m_dimy_multi_select_max;

public:
	void drawScatterGrid();
	void drawScatterAxesName();
	void drawAxes();
    void drawSingleData(std::vector<double> &data, int datano);
    void drawSingleBand(RGBt & color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int datano);

    void drawSingleBrush(Brush *brush);
	//Set initial parameters
	void setParameters();

	//Set the position for each axis, which is used to distort scatterplot matrix.
	void setPlacement();
	//Set coefficients used for XMAP and YMAP
	void setMapFactors();

	// Map the x, y coordinate from data to screen
	double XMAP(int DIM, double VAL);
	double YMAP(int DIM, double VAL);
	double XINVMAP(int DIM, double VAL);
	double YINVMAP(int DIM, double VAL);
	bool betweenTwoPoint(double startPos, double endPos, double inputPos);
	double abs(double v);
	void findResizeCorners(double x,double y);
	void drawScatterBrushBox(GLCanvas *canvas, int dimx, int dimy, double data_x, double data_y,
	             double size_x, double size_y);

	void inputScatterFindPlot(GLCanvas *canvas, int sx, int sy, int &X_dim, int &Y_dim);
	void inputButtonPress(int button, int x, int y);
	void inputMouseMotion(int x, int y);
	void inputButtonRelease(int button, int x, int y);
	void inputScatterResizeBrush(GLCanvas *canvas, int sx, int sy);
	void inputScatterMoveBrush(GLCanvas *canvas, int sx, int sy);
	void inputScatterMultiSelect(GLCanvas *canvas, int sx, int sy);
	void multiSelectAdjustBrush();

	//implemented for scatterplot special diagonal display
	void drawSpecialCustomization();

	//Set the point shape type
	void setShapeType(int shapeType);

	//Set point size
	void setPointSize(int size);

	//Set dianonal display type
	void setDiagType(XmdvTool::DIAGPLOT_MODE diag_type);

	int getShapeType();
	int getPointSize();
	XmdvTool::DIAGPLOT_MODE getDiagType();


	void setDataDrivenXDimNumber(int xDimNumber);
	int getDataDrivenXDimNumber();

	void setDataDrivenYDimNumber(int yDimNumber);
	int getDataDrivenYDimNumber();

	void setDerivedXDimNumber(int xDimNumber);
	int getDerivedXDimNumber();

	void setDerivedYDimNumber(int yDimNumber);
	int getDerivedYDimNumber();

	void setOneDimSortDim(int sortDim);
	int getOneDimSortDim();

};

#endif /* VISSCATTER_H_ */
