/*
 * VisDimstack.h
 *
 *  Created on: May 12, 2009
 *      Author: Dan Spitz
 *
 *  VisDimstack renders the dimension-stacking view and manages its interaction
 */

#ifndef VISDIMSTACK_H_
#define VISDIMSTACK_H_

#include "view/multidim/OkcViewDisplay.h"
#include "interface/dimstack_customization/DimstackCustomization.h"

class VisDimstack : public OkcViewDisplay{

	//make the customization dialog a friend class
	//to access the dimension names and cardinality
	friend class DimstackCustomization;

public:
	VisDimstack(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm);
	virtual ~VisDimstack();

public:
	//check whether the data can be viewed by DimStacking
	XmdvTool::CHECK_RENDERABLE_RESULT checkRenderable();

	//error message displayed when a dataset may not be viewed with dimstacking
	QString getRenderErrorMessage();

	//Set the initial parameters
	void setParameters();

	//position the axes for dimensional stacking
	void setPlacement();

	//draw the axes for dimensional stacking
	void drawAxes();
	void drawDimstackGrid();

	//draw single data (fill in a rectangle)
	void drawSingleData(std::vector<double> &data, int datano);

	//fill in all rectangles that are within a band
	void drawSingleBand(RGBt &color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int datano);

	//draw the brush
	void drawSingleBrush(Brush *brush);

	//handle mouse click- recenter the brush
	void inputButtonPress(int button, int x, int y);

	// track the mouse motion, used for display status bar and dragging operations
	void inputMouseMotion(int x, int y);

	// Return the private data member dimstack_size
	void getDimstack_size(std::vector<double>& stack_size);

	// Return the x_block_size
	double getXBlockSize();
	// Return the y_block_size
	double getYBlockSize();

private:
	//helper for determining which bucket a value falls into
	int get_contrib(int ind, double data);

	//helper for drawSingleBrush
	bool is_within(int val, int dim, Brush *brush);

	//helper for drawSingleBrush
	void rec_fill(double basex, double basey, int nextx, int nexty, std::vector<double> &size, Brush *brush);

	//recenter the brush on the mapped screen->data value of x and y
	void inputDimstackMoveBrush(int x, int y);

	//helper for mapping a 2d point on the screen to an n-dimensional datapoint
	bool dimstackMapScreenToData(double x, double y, std::vector<double> &center);



private:
	int sensible_limit;
	double sensible_X_blocksize;
	double sensible_Y_blocksize;

	std::vector<double> dimstack_size;
	double x_size, y_size, x_block_size, y_block_size;
	static double min_x_block_size(){
		return .001;
	}
	static double min_y_block_size(){
		return .001;
	}

	//hierarchical-related functions
	//draw band of level k
	void traceBand(int k);
	//draw a bin in a band
	void fillBinInBand();
	//calculate the distance factor of a bin in the band
	double distanceFactor();
	//hierarchical-related variables

	//color of the band to be drawn
	RGBt color;

	//contrib arrays-  hold the bin number for each dimension that a value falls
	//into- each for one of mean, min, max and current value to draw, of a band.
	//mean contrib
	std::vector<int> contrib0;
	//min contrib
	std::vector<int> contrib1;
	//max contrib
	std::vector<int> contrib2;
	//current bin in the band being filled.
	std::vector<int> contrib;

	// reassemble the pipeline from the DimOOOOperator and redraw the canvas
	// after users changed the cardinality numbers
	void refreshCardinality(std::vector<int> newCard);

};

#endif /* VISDIMSTACK_H_ */
