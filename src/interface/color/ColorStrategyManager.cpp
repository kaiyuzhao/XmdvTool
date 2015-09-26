/*
 * ColorStrategyManager.cpp
 *
 *  Created on: Jul 9, 2009
 *      Author: jstasik
 */

#include "interface/color/ColorStrategyManager.h"
#include "main/XmdvToolMainWnd.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/ViewDisplay.h"

ColorStrategyManager::ColorStrategyManager(XmdvToolMainWnd *main) {
	mainWnd = main;
}

ColorStrategyManager::~ColorStrategyManager() {
}

ColorStrategy* ColorStrategyManager::getDialog(){
	ViewManager *viewManager = mainWnd->getViewManager();
	ViewWindow *activeWindow = viewManager->getActiveViewWindow();

	int pID = activeWindow->getPipelineID();

	return dialogs[pID];
}

int ColorStrategyManager::getValueDim(int pid) {
	if(pid == -1) pid = mainWnd->getViewManager()->getActiveViewWindow()->getPipelineID();

	if (!dialogs.count(pid))
		dialogs[pid] = new ColorStrategy(mainWnd, pid);

	return dialogs[pid]->getValueDim();
}
