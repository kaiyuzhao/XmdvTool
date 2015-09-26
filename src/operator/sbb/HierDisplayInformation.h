/*
 * HierDisplayInformation.h
 *
 *  Created on: Jun 16, 2009
 *      Ported by Zaixian Xie
 */

#ifndef HIERDISPLAYINFORMATION_H_
#define HIERDISPLAYINFORMATION_H_

//#include <xmdv/colormap.h>
//#include <stdio.h>
#define STYLE_NONE 0x01
#define STYLE_HIGHLIGHT 0x02
#define STYLE_COLORMAP 0x04
#define STYLE_THICK 0x08
#define STYLE_DIM 0x10

//#include <xmdv/ColorSetting.h>

class HierDisplayInformation
{
public:
	int tree_height;

	bool SHOW_BANDS;
	bool SHOW_MEANS;
	bool SHOW_BRUSH_BKGRD;
	bool COLORED_BANDS;
	bool EXTENT_BRUSH;

	double brushed_radius;
	double cluster_radius;
	/**Stores the radius of the root node*/
	double root_radius;

	double tree_brush[2];

	int normal_style;
	int highlight_style;

	double fade_factor;
	double extent_scaling_fac;
	double density;

	//colormap *CMAP;
	//colormap CMAP_SBB;

	//ColorSetting colorSetting;

	int dimension;

public:
	HierDisplayInformation();
	~HierDisplayInformation();

	void Reset();
	void SetBackend(bool backend);
};


#endif /* HIERDISPLAYINFORMATION_H_ */
