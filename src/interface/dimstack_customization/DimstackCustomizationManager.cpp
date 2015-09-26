/*
 * DimstackCustomizationManager.cpp
 *
 *  Created on: May 20, 2009
 *      Author: Dan Spitz
 */

#include "DimstackCustomizationManager.h"

#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>

#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "pipeline/Pipeline.h"

DimstackCustomizationManager::DimstackCustomizationManager(XmdvToolMainWnd *mainWnd) {
	this->mainWnd = mainWnd;
}

DimstackCustomizationManager::~DimstackCustomizationManager() {

}

DimstackCustomization *DimstackCustomizationManager::getDialog(){
	//get the current window.
	ViewManager *viewManager = mainWnd->getViewManager();
	DimstackCustomization *dialog = 0;
	QMdiSubWindow *window = viewManager->getMdiArea()->activeSubWindow();
	dialog = new DimstackCustomization(mainWnd, window);
	return dialog;
}
