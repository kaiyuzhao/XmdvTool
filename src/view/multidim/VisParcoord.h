/*
 * VisParcoord.h
 *
 *  Created on: Apr 1, 2008
 *      Author: Zaixian Xie
 */

/*
 * class VisParcoord renders parallel coordinates
 * and manages its interaction
 */
#ifndef VISPARCOORD_H_
#define VISPARCOORD_H_

#include <vector>
#include "view/multidim/OkcViewDisplay.h"

class ViewWindow;
class VisualMapResult;
class PipelineManage;
class GLCanvas;
class ViewManager;

class VisParcoord : public OkcViewDisplay
{
public:
	VisParcoord(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm);
	virtual ~VisParcoord();

private:
	//manage the screen distortion
    std::vector<double> paracoord_axes;
    std::vector<double> paracoord_interval;
    int axis;
    bool start_multi_select;

    // The coefficients to map data to screen
    std::vector<double> m_yMapFactor;

    bool m_firstTime;


public:
	//draw the axes for parallel coordinates
	void drawAxes();
	void drawParcoordAxes();
	void drawParcoordAxesNameMinMax();
	//Set the initial parameters
	void setParameters();

	// Test whether the dataset has enough number of dimensions to render
	bool testDimensionality();

	//reset the position for each axis, which is used to distort parallel coordinates.
	void setPlacement();

	//draw single data
    void drawSingleData(std::vector<double> &data, int datano);

    // draw single band
	void drawSingleBand(RGBt & color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int datano);

    void drawSingleBrush(Brush *brush);
    void drawParcoordBrushBoundRamp(GLCanvas *canvas, Brush *brush, unsigned long color1, unsigned long color2);


    //map the values to screen coordinates
    double XMAP(int I);
    double YMAP(int X, double Y);
    double YINVMAP(int X, double Y);

    void multiSelectAdjustBrush(int point);
    void inputButtonPress(int button, int x, int y);
    void inputButtonRelease(int button, int x, int y);
    void inputParacoordFindAxesNear(GLCanvas *canvas, double X, double Y, int &dim);

    void inputParcoordMultiSelect(GLCanvas *canvas, int sx, int sy);
    void inputParcoordBoundRampResize(GLCanvas *canvas, int sx, int sy);
    void inputMouseMotion(int x, int y);
    void inputParcoordMoveBrush(GLCanvas *canvas, int sx, int sy);
    void inputParcoordResizeBrush(GLCanvas *canvas, int sx, int sy);
};

#endif /*VISPARCOORD_H_*/
