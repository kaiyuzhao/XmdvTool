/*
 * PixelCustomizationManager.cpp
 *
 *  Created on: May 20, 2009
 *      Author: jstasik
 */

#include "interface/pixel_customization/PixelCustomizationManager.h"
#include "main/XmdvToolMainWnd.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/ViewDisplay.h"
#include "view/multidim/OkcViewDisplay.h"

PixelCustomizationManager::PixelCustomizationManager(XmdvToolMainWnd *mainWnd) {
	this->mainWnd = mainWnd;

}

PixelCustomizationManager::~PixelCustomizationManager() {
	map<int, PixelCustomizationSettings*>::iterator it;
	// release the memory occupied by all of the settings
	for ( it=settings.begin(); it != settings.end(); it++ ) {
		delete ((*it).second);
	}
}

PixelCustomization *PixelCustomizationManager::getDialog(){
	//get the current window.
	ViewManager *viewManager = mainWnd->getViewManager();
	//ViewWindow *activeWindow = viewManager->getActiveViewWindow();

	//int pID = activeWindow->getPipelineID();
	PixelCustomization *dialog = 0;

	//a dialog has already been created for this pipeline.
	/*
	if (dialogs.count(pID)) {
		//the dialog already exists
		dialog = dialogs[pID];
	}
	//the dialog does not yet exist, make a new one
	else {
	 */
	QMdiSubWindow *window = viewManager->getMdiArea()->activeSubWindow();
	dialog = new PixelCustomization(mainWnd, window, getSettings());
	//		dialogs[pID] = dialog;
	//	}
	return dialog;
}

PixelCustomizationSettings *PixelCustomizationManager::getSettings() {
	ViewManager *viewManager = mainWnd->getViewManager();
	ViewWindow *activeWindow = viewManager->getActiveViewWindow();

	int pID = activeWindow->getPipelineID();
	PixelCustomizationSettings *setting;

	//a dialog has already been created for this pipeline.
	if (settings.count(pID)) {
		//the dialog already exists
		setting = settings[pID];
	}
	//the dialog does not yet exist, make a new one
	else {
		setting = new PixelCustomizationSettings();
		settings[pID] = setting;
	}
	return setting;
}
