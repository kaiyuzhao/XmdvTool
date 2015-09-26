/*
 * VisPixel.h
 *
 *  Created on: May 13, 2009
 *      OrigAuthor: anilkpatro
 *      Ported by: Jason Stasik
 *
 *     bug: when the sampling slider is moved below 15 and then
 *     		you switch to another vis then back to pixel, it crashes
 *     		tempfix: don't let the slider go below 15
 */

#ifndef VISPIXEL_H_
#define VISPIXEL_H_

#include <cstdlib>
#include <sstream>
#include <vector>
using namespace std;
#include <GL/glu.h>

#include <vector>
#include "datatype/Vec2.h"
#include "view/multidim/OkcViewDisplay.h"
#include "view/auxiliary/Image.h"
#include "view/auxiliary/Subwindow.h"
//#include "interface/pixel_customization/PixelBrush.h"

class ViewWindow;
class VisualMapResult;
class PipelineManage;
class PixelCustomization;

struct SortStruct{
	int number;
	double value;
};

extern int IncreaseSort( const void *elm1, const void *elm2);
extern int DecreaseSort( const void *elm1, const void *elm2);

class VisPixel : public OkcViewDisplay {
	friend class PixelCustomization;
	friend class PixelBrush;
	//friend struct PixelBrush::Marker;

public:

	///
	/// Constructor
	///
	VisPixel(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm);

	///
	/// Deconstructor
	///
	virtual ~VisPixel();

	enum PlacementType {
		PPT_NORMAL,
		PPT_MDS,
	};

	///
	/// Draw each dimension (subwindow) of pixel oriented display
	///
	void		drawAxes();

	//event handlers
	void		inputMouseMotion(int x, int y);
	void		inputButtonPress(int button, int x, int y);
	void		inputButtonRelease(int button, int x, int y);

	/// ACCESS
	void		setDisplay(Subwindow::DisplayType dt);
	Subwindow::DisplayType getDisplay(void) const;

	int			getSortedDim() const;
	void		setSortedDim(int dim);

	int			getSelectedDim() const;
	void		setSelectedDim(int dim);

	float		getDimDistortion(int dim) const;
	void		setDimDistortion(int dim, float dist);

	bool		getSortDimOrder() const;
	void		setSortDimOrder(bool isAscending);

	int			getScale() const;
	void		setScale(int scale);

	void		setPlacementType(PlacementType pt);
	PlacementType getPlacementType(void) const;

	float		getSamplingFactor() const;
	void		setSamplingFactor(float val);

	void		setWeight(int dim, int val);

	void		setBrushHasModified();
	void		setPlacementHasModified();
	void		setDistortionHasModified();

	void		setDialog(PixelCustomization *options);

	void		refresh(bool sort, bool weight);

	//return true if there is enough dimensions.
	bool testDimensionality();

private:

	bool 		showCaption(int x, int y);

	void		freeSpace();

	///
	/// Draw the square spiral subwindow visualization
	///
	void		drawSpiral();

	// Draw the color ramp as the legend
	void		drawLegend();

	double		norm(int dim, double val);

	void		CalcPos(int n, int m, const std::vector<double> &data);

	int			findNearestDim(Vec2 p);

	void		computePixelInfo();


protected:	// members


	bool		reset;

	bool		m_runOnce;
	int			m_oldWidth;
	int			m_oldHeight;

	PixelCustomization *m_options;

	int			m_movingDim;

	PixelInfo	pixel_info;

	int			m_currDim;
	Subwindow::DisplayType m_display;

	PlacementType m_placementType;
	double		**m_dispPos;

	int			m_sortedDim;
	bool		m_sortAsc;

	//bool		m_isBrushModified;
	bool		m_hasDistortionChanged;
	bool		m_placementChanged;

	float		m_sampleFactor; // [0..1.0f]
	double		m_sampleStart, m_sampleEnd;
	std::vector<float>	m_distortions; // [0..1.0f] distortion factor for each dim

	std::vector<Image*>		m_images;

	std::vector<Subwindow*> m_subwindows;
};

//-----------------------------------------------------------------------------
inline int VisPixel::getSortedDim() const
{
	return m_sortedDim;
}

inline void VisPixel::setSortedDim(int dim) {
	m_sortedDim = dim;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline bool VisPixel::getSortDimOrder() const
{
	return m_sortAsc;
}

inline void VisPixel::setSortDimOrder(bool isAscending) {
	m_sortAsc = isAscending;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline VisPixel::PlacementType VisPixel::getPlacementType(void) const
{
	return m_placementType;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline int VisPixel::getSelectedDim() const
{
	return m_currDim;
}

inline void VisPixel::setSelectedDim(int dim) {
	m_currDim = dim;
}

inline float VisPixel::getDimDistortion(int dim) const
{
	return m_distortions[dim];
}

inline void VisPixel::setDimDistortion(int dim, float dist)
{
	m_distortions[dim] = dist;
}

inline float VisPixel::getSamplingFactor() const
{
	return m_sampleFactor;
}

inline void VisPixel::setPlacementHasModified() {
	m_placementChanged = true;
}
inline void VisPixel::setDistortionHasModified() {
	m_hasDistortionChanged = true;
}

inline void VisPixel::setDisplay(Subwindow::DisplayType dt) {
	m_display = dt;
}

inline Subwindow::DisplayType VisPixel::getDisplay() const {
	return m_display;
}

#endif /* VISPIXEL_H_ */
