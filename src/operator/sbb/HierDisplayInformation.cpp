/*
 * HierDisplayInformation.cpp
 *
 *  Created on: Jun 16, 2009
 *      Author: Zaixian Xie
 */

#include "HierDisplayInformation.h"

//#include <xmdv/colorramp.h>
//#include <xmdv/okcdata.h>



HierDisplayInformation::HierDisplayInformation()
	//: CMAP_SBB("HierBrudhed")
{
	Reset();
}

HierDisplayInformation::~HierDisplayInformation()
{
	//delete CMAP;
}

void HierDisplayInformation::Reset()
{
	cluster_radius = 0.5;
	brushed_radius= 0.5;
	fade_factor=0; // [-1,+1]
	extent_scaling_fac=1.0;
	density = 0.5;
	SHOW_BANDS = true;//show_bands
	SHOW_MEANS = true;
	SHOW_BRUSH_BKGRD = true;
	COLORED_BANDS = true;

	normal_style = STYLE_COLORMAP;
	highlight_style = STYLE_HIGHLIGHT | STYLE_THICK;

	tree_brush[0] = .25;
	tree_brush[1] = .75;
	tree_height=0;

	//CMAP = new colormap("HierDisplay");
	//colorSetting.setDefaultHierColorTheme();
	//CMAP_SBB = colormap("HierBrudhed", ColorRamp::GetPredefinedStructBrushed(CMAPSIZE));

}
