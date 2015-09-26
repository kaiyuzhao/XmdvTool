/*
 * PixelCustomizationSettings.h
 *
 *  Created on: May 21, 2009
 *      Author: jstasik
 *
 *  Settings class for VisPixel to save data
 */

#ifndef PIXELCUSTOMIZATIONSETTINGS_H_
#define PIXELCUSTOMIZATIONSETTINGS_H_

#include <vector>
#include "view/auxiliary/Subwindow.h"

class PixelCustomizationSettings {

public:
	enum RADIO_TYPE {
		RA_NOR,
		RA_MDS,
	};

	PixelCustomizationSettings() {
		isSet = false;
		positionSet = false;
		settings_mdsPos = 0;
	}

	PixelCustomizationSettings(int dim_size) {
		//settings_weights.resize(dim_size);
		settings_distortion.resize(dim_size + 1);
		settings_position = new double * [2];
		settings_position[0] = new double [dim_size + 1];
		settings_position[1] = new double [dim_size + 1];
		isSet = false;
		positionSet = false;
	}

	int		settings_maxWeight;
	int		settings_selectedDim;
	int		settings_sortedDim;
	bool	settings_asc;
	bool	settings_scaleAll;
	bool	settings_enableSampling;
	RADIO_TYPE	settings_placement;
	//std::vector<int>	settings_weights;
	double	**settings_position;
	double	**settings_mdsPos;
	std::vector<int>	settings_distortion;
	int		settings_sampling;
	Subwindow::DisplayType settings_displayType;

	bool	isSet;
	bool	positionSet;

};

#endif /* PIXELCUSTOMIZATIONSETTINGS_H_ */
