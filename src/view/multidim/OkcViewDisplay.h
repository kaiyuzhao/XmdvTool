/*
 * OkcViewDisplay.h
 *
 *  Created on: Jan 16, 2009
 *      Author: Zaixian Xie
 */

/*
 * class OkcViewDisplay does the rendering for multivariate data.
 * The developers should derive a new class for each type of
 * visualization technique.  For example, its derived class VisParcoord
 * is responsible for render parallel coordinates
 *
 */

#ifndef OKCVIEWDISPLAY_H_
#define OKCVIEWDISPLAY_H_

#include "view/ViewDisplay.h"
#include <vector>
#include "datatype/RGBt.h"

class PipelineManager;
class MouseKeyboardStatus;
class Brush;
class OkcVisualMapResult;
class OkcData;
class HierDisplayInformation;

class OkcViewDisplay: public ViewDisplay {
	friend class ViewManager;
	friend class DimControl;
public:
	OkcViewDisplay(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm);
	virtual ~OkcViewDisplay();

	enum INPUT_MODE {
			MULTI_SELECT,
			CANVAS_PAN,
			NONE,
			RESIZE_RAMP,

			//for parcoord
			RESIZE_BRUSH_TOP,
			RESIZE_BRUSH_BOTTOM,
			TRANSLATE_BRUSH,
			INCREASE_AXES_INTERVAL,
			DECREASE_AXES_INTERVAL,

			//for scatter
			RESIZE_BRUSH,
			MOVE_BRUSH,
			INCREASE_PLOT_SIZE,
			DECREASE_PLOT_SIZE
		};

	INPUT_MODE m_input_mode;

protected:
    // The minimum and maximum values for each dimension
    // During rendering, these values will be retrieved from visual mapping result
    // in the function setParameters()
    // and saved to here to promote efficiency.
    int m_dim_size;
    int m_data_size;
    std::vector<char*> m_dim_names;
    std::vector<double> m_dim_min;
    std::vector<double> m_dim_max;

    std::vector<int> m_dim_cardinality;

    // The settings for hierarchical display
    HierDisplayInformation* m_hier_info;

	// m_opacity is the alpha value of data and band color
	// which is used to fade brushed or unbrushed data
    double m_opacity;

public:
	// get the instance pointer of class OkcVisualMapResult
	OkcVisualMapResult* getOkcVisualMapResult();

	// return the OkcData to be visualized
	OkcData* getDisplayDataset();

	// Paint the canvas.  The derived class should implement this
	// function to do the specific rendering
	void paint();

	//some display requires minimum dimensionality, return true if there is enough dimensions.
	//Rewrite when necessary
	virtual bool testDimensionality();

	//set parameters used for the drawing
	//rewrite when necessary
	virtual void setParameters();

	//Set some positions for the visualization techniques
	//E.g. axes coordinate for parallel coordinates
	virtual void setPlacement();

	//draw axes, or grid
	//implement it in the derived class when necessary
	virtual void drawAxes();

	//draw all data, which will call drawSingleData multiple times
	//implement it in the derived class when necessary
	virtual void drawData();

	// draw all data when the pipeline is a flat normal one.
	// This function will be called by drawData();
	virtual void drawDataFlatNormal();

	// draw all data when the pipeline is a SBB one.
	virtual void drawDataSBB();

	//draw a single data
	//need to override it in the derived class
	virtual void drawSingleData(std::vector<double> &data, int datano);

	//draw a band for a single data
	//need to override it in the derived class
	virtual void drawSingleBand(RGBt & color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int datano);

	// return the brush associated with data
	Brush* getBrush();

	//draw a single brush
	virtual void drawSingleBrush(Brush *brush);

	//draw special customized display; implemented for scatterplot diagonal display
	virtual void drawSpecialCustomization();

	// reassemble the pipeline from the brushOperator and redraw the canvas
	void refreshBrush();

	bool betweenTwoPoint(double startPos, double endPos, double inputPos);

};

#endif /* OKCVIEWDISPLAY_H_ */
