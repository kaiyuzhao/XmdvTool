/*
 * VisGlyph.h
 *
 *  Created on: Apr 22, 2009
 *      Author: Zaixian Xie
 */

/*
 * class VisParcoord renders parallel coordinates
 * and manages its interaction
 */


#ifndef VISGLYPH_H_
#define VISGLYPH_H_

#include <vector>
#include "datatype/Vec2.h"
#include "view/multidim/OkcViewDisplay.h"

class ViewWindow;
class PipelineManage;
class ViewManager;

class VisGlyph : public OkcViewDisplay
{
public:
	VisGlyph(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm);
	virtual ~VisGlyph();

protected:
	//manage the screen distortion
    std::vector<double> star_angles;
    std::vector<double> star_intervals;
    std::vector<double> star_angles_cos_sized;
    std::vector<double> star_angles_sin_sized;

	double m_blocksize;
	std::vector<double>* m_basex;
	std::vector<double>* m_basey;

private:
	// Although these two variables are used only in drawSingleData().
	// we declare them here and initialize them within setParameters()
	// to reduce time cost.
	std::vector<double> m_norm;
    std::vector<Vec2> m_poly;


public:
	//Set the initial parameters
	void setParameters();

	//reset the position for each axis, which is used to distort parallel coordinates.
	void setPlacement();

	//draw single data
    void drawSingleData(std::vector<double> &data, int datano);

    void drawSingleBand(RGBt & color, std::vector<double> &data, std::vector<double> &max, std::vector<double> &min, int datano);

    void inputMouseMotion(int x, int y);

    void inputGlyphFindPointNear(double X, double Y, int &pointNo);

};

#endif /* VISGLYPH_H_ */
