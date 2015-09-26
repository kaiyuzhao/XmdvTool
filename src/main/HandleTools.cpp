/*
 * HandleTools.cpp
 *
 *  Created on: May 14, 2009
 *      Author: Dan Spitz, Zaixian Xie
 */

#include "HandleTools.h"

#include "pipeline/PipelineManager.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "interface/dimstack_customization/DimstackCustomization.h"
#include "interface/pixel_customization/PixelCustomization.h"
#include "interface/pixel_customization/PixelCustomizationManager.h"
#include "interface/glyph/GlyphCustom.h"
#include "interface/color/ColorCustom.h"
#include "interface/color/ColorStrategyManager.h"
#include "interface/color/ColorStrategy.h"
#include "interface/scatterplot/ScatterplotCustomManager.h"
#include "interface/interring/DimReductControl.h"
#include "interface/dimooo/DimManualControl.h"
#include "interface/dimooo/DimAutoReorderControl.h"


HandleTools::HandleTools(XmdvToolMainWnd* mainWnd){
	this->m_mainWnd = mainWnd;
	dimManager = new DimstackCustomizationManager(mainWnd);
	pixelManager = new PixelCustomizationManager(mainWnd);
	stratManager = new ColorStrategyManager(mainWnd);
	colorCustom = new ColorCustom(mainWnd);
	scatterplotManager = new ScatterplotCustomManager(mainWnd);

	connect ( this, SIGNAL(dimReductModeEnabled()), this, SLOT(openDimReductDlg()) );
}

HandleTools::~HandleTools() {
	SAFE_DELETE(dimManager);
	SAFE_DELETE(pixelManager);
	SAFE_DELETE(colorCustom);
	SAFE_DELETE(stratManager);
}

void HandleTools::showDimManualDlg() {
	ViewManager* vm= m_mainWnd->getViewManager();
	ViewWindow* win = vm->getActiveViewWindow();

	DimManualControl *dimManualDlg = new DimManualControl(m_mainWnd,
			m_mainWnd->getPipelineManager(), win->getPipelineID(), win);
	dimManualDlg->show();
}

void HandleTools::showDimAutoDlg() {
	ViewManager* vm= m_mainWnd->getViewManager();
	ViewWindow* win = vm->getActiveViewWindow();

	DimAutoReorderControl *dimAutoDlg = new DimAutoReorderControl(m_mainWnd,
			m_mainWnd->getPipelineManager(), win->getPipelineID(), win);
	dimAutoDlg->show();
}

ScatterplotCustomManager *HandleTools::getScatterplotCustomManager(){
	return scatterplotManager;
}

DimstackCustomizationManager *HandleTools::getDimstackCustomizationManager(){
	return dimManager;
}

PixelCustomizationManager *HandleTools::getPixelCustomizationManager(){
	return pixelManager;
}

ColorStrategyManager *HandleTools::getColorStrategyManager(){
	return stratManager;
}


void HandleTools::openDimstackCustomization(){
	DimstackCustomization *dialog = dimManager->getDialog();
	dialog->show();
	dialog->activateWindow();

}

void HandleTools::openPixelCustomization(){
	PixelCustomization *dialog = pixelManager->getDialog();
	dialog->show();
	dialog->activateWindow();
}

void HandleTools::showGlyphCustom() {
	PipelineManager* pm = m_mainWnd->getPipelineManager();

	// Create a dialog and set some arguments
	GlyphCustom *dlg = new GlyphCustom(m_mainWnd);
	dlg->setMainWnd(m_mainWnd);
	dlg->setPipelineID(pm->getActivePipelineID());

	// Show this dialog
	dlg->show();
}

void HandleTools::ShowScatterplotCustom() {
	ScatterplotCustom *dialog = scatterplotManager->getDialog();
	dialog->show();
	dialog->activateWindow();
}

void HandleTools::showColorCustom() {
	colorCustom->show();
	colorCustom->activateWindow();
}

void HandleTools::showColorStrategy() {
	ColorStrategy *dialog = stratManager->getDialog();
	dialog->show();
	dialog->activateWindow();
}

void HandleTools::turnOnDimReductMode() {
	switchDimReductMode(true);
}

void HandleTools::turnOffDimReductMode() {
	switchDimReductMode(false);
}

void HandleTools::switchDimReductMode(bool checked) {
	// Obtain the pointer to the View Manager
	ViewManager *vm = m_mainWnd->getViewManager();
	if (checked) {
		// switch to dimension reduction pipeline if this mode is checked
		vm->changeToDimRPipeline();
		emit dimReductModeEnabled();
	} else {
		// switch to dimension reduction pipeline if this mode is checked
		vm->changeToFlatPipeline();
	}

	// Update the menu and tool bar
	m_mainWnd->updateMenuToolbarDock();
}

void HandleTools::openDimReductDlg() {
	ViewManager* vm= m_mainWnd->getViewManager();
	ViewWindow* win = vm->getActiveViewWindow();

	DimReductControl *sbbDlg = new DimReductControl(m_mainWnd,
			m_mainWnd->getPipelineManager(), win->getPipelineID());
	sbbDlg->show();

}
