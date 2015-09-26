/*
 * PixelCustomizationManager.h
 *
 *  Created on: May 20, 2009
 *      Author: jstasik
 *
 *  Stores all the dialog boxes with a corresponding visualization
 *  in order to preserve the state
 */

#ifndef PIXELCUSTOMIZATIONMANAGER_H_
#define PIXELCUSTOMIZATIONMANAGER_H_

#include <map>
#include "interface/pixel_customization/PixelCustomization.h"
#include "interface/pixel_customization/PixelCustomizationSettings.h"

class XmdvToolMainWnd;

class PixelCustomizationManager {
public:
	PixelCustomizationManager(XmdvToolMainWnd *mainWnd);
	virtual ~PixelCustomizationManager();

	PixelCustomization *getDialog();
	PixelCustomizationSettings *getSettings();

private:
	XmdvToolMainWnd *mainWnd;
	map<int, PixelCustomization *> dialogs;
	map<int, PixelCustomizationSettings *> settings;
};

#endif /* PIXELCUSTOMIZATIONMANAGER_H_ */
