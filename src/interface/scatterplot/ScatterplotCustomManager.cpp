/*
 * ScatterplotCustomManager.cpp
 *
 *  Created on: Jul 27, 2009
 *      Author: zyguo
 */

#include "ScatterplotCustomManager.h"

#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>

#include "view/ViewManager.h"
#include "view/ViewWindow.h"

ScatterplotCustomManager::ScatterplotCustomManager(XmdvToolMainWnd *mainWnd) {
	this->mainWnd = mainWnd;

}

ScatterplotCustomManager::~ScatterplotCustomManager() {
}

ScatterplotCustom *ScatterplotCustomManager::getDialog(){
	//get the current window.
	ViewManager *viewManager = mainWnd->getViewManager();
	ViewWindow *activeWindow = viewManager->getActiveViewWindow();

	int pID = activeWindow->getPipelineID();
	ScatterplotCustom *dialog = 0;

	//a dialog has already been created for this pipeline.
	if (dialogs.count(pID)) {
		//the dialog already exists
		dialog = dialogs[pID];
	}
	//the dialog does not yet exist, make a new one
	else {
		QMdiSubWindow *window = viewManager->getMdiArea()->activeSubWindow();
		dialog = new ScatterplotCustom(mainWnd, window);
		dialogs[pID] = dialog;
	}
	return dialog;
}
