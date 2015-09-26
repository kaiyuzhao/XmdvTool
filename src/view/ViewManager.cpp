/*
 * ViewManager.cpp
 *
 *  Created on: Apr 1, 2008
 *      Author: Zaixian Xie
 */

#include <QMessageBox>
#include <QScrollArea>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "view/ViewManager.h"

#include <assert.h>

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "main/HandleTools.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "view/MdiSubWindow.h"
#include "interface/dimstack_customization/DimstackCustomizationManager.h"
#include "interface/pixel_customization/PixelCustomizationManager.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "view/GLDisplay.h"
#include "view/ViewWindow.h"
#include "view/ViewDisplay.h"
#include "view/multidim/VisParcoord.h"
#include "view/MdiSubWindow.h"

#include <string>
using namespace std;

ViewManager::ViewManager(XmdvToolMainWnd* wnd)
{
	this->setMainWnd(wnd);
}

ViewManager::~ViewManager()
{
	// release the memory occupied by all View Windows
	int i;
	for (i=0; i<(int)m_viewWindows.size(); i++) {
		SAFE_DELETE(m_viewWindows[i]);
	}
}

void ViewManager::setMainWnd(XmdvToolMainWnd* wnd) {
	this->m_mainWnd = wnd;
}

XmdvToolMainWnd* ViewManager::getMainWnd() {
	return this->m_mainWnd;
}

PipelineManager* ViewManager::getPipelineManager() {
	return getMainWnd()->getPipelineManager();
}

void ViewManager::setMdiArea(QMdiArea* mdiArea) {
	this->m_mdiArea = mdiArea;
}

QMdiArea* ViewManager::getMdiArea() {
	return m_mdiArea;
}

int ViewManager::getActiveViewWindowNo() {
	return m_activeViewWindowNo;
}

ViewWindow* ViewManager::getActiveViewWindow(){
	int viewWinNo = getActiveViewWindowNo();
	if ( viewWinNo<0 || viewWinNo>=(int)m_viewWindows.size() ) {
		// If no active view window, return NULL
		return 0;
	} else {
		return m_viewWindows[viewWinNo];
	}
}

int ViewManager::getViewWindowsCount() {
	return m_viewWindows.size();
}

ViewWindow*  ViewManager::getViewWindow(int index) {
	return m_viewWindows[index];
}

int ViewManager::getViewWindowsCount(int pID) {
	int i, count=0;
	for ( i=0; i<(int)m_viewWindows.size(); i++) {
		if ( m_viewWindows[i] && m_viewWindows[i]->getPipelineID() == pID) {
			count++;
		}
	}
	return count;
}

// Return the bigger order in all windows associated with a dataset.
int ViewManager::getMaxWindowOrder(Data* origData) {
	int i, max = -1;
	for ( i=0; i<(int)m_viewWindows.size(); i++) {
		// If this is null pointer, skip it
		if ( !m_viewWindows[i] ) {
			continue;
		}
		// Get the input dataset of this view
		Data* inputData = m_viewWindows[i]->getInputData();
		// Check whether this data is the data in the parameter
		if ( inputData == origData ) {
			int order = m_viewWindows[i]->getOrder();
			if ( order > max ) {
				max = order;
			}
		}
	}

	return max;
}

ViewWindow* ViewManager::createViewWindow(int pipelineID, QString title, XmdvTool::VISTYPE visType) {
	QMdiArea* mdiArea = getMdiArea();

	// create the sub window
	MdiSubWindow* subWin = new MdiSubWindow(mdiArea, this);


	// create the scroll area
	QScrollArea* scrollArea = new QScrollArea(subWin);
	// create the view window
	ViewWindow* viewWindow = new ViewWindow();
	// create the GLDisplay, which is the derived class of QWidget
	GLDisplay* glDisplay = new GLDisplay(scrollArea, viewWindow);

	if ( !( glDisplay->isValid() ) ) {
		QMessageBox::warning(getMainWnd(),
				"Invalid OpenGL support",
				"XmdvTool cannot create QGLWidget, probably because your current configuration does not have OpenGL support!");
	}

	// Get the current max order of the sub windows associated with the input data
	Pipeline* pl = getPipelineManager()->getPipeline(pipelineID);
	int maxOrder = getMaxWindowOrder( pl->getOrigDataset() );
	int winOrder = maxOrder+1;
	viewWindow->setOrder(winOrder);
	// Construct the window title
	QString winTitle = title.append(" - ").append(ViewWindow::createViewName(winOrder));

	// set the sub window
	subWin->setGLDisplay(glDisplay);
	subWin->setWidget(scrollArea);
	subWin->setAttribute(Qt::WA_DeleteOnClose);
	subWin->setWindowTitle( winTitle );
	subWin->setWindowIcon(QIcon(QString::fromUtf8(
			":/new/prefix2/icon/imgpa.png")));
	mdiArea->addSubWindow(subWin, Qt::Window);

	// set the scroll area
	scrollArea->setWidget(glDisplay);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	// set the GLDisplay
	glDisplay->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	glDisplay->setMinimumSize(200, 200);

	// set the view window
	viewWindow->setPipelineManager(getPipelineManager());
	viewWindow->setPipelineID(pipelineID);
	viewWindow->setViewManager(this);
	viewWindow->setSubWindow(subWin);
	viewWindow->setTitle(winTitle);
	viewWindow->setGLDisplay(glDisplay);
	viewWindow->setVisType(visType);
	viewWindow->createViewDisplay();
	m_viewWindows.push_back(viewWindow);
	viewWindow->setViewWinNo(m_viewWindows.size()-1);

	// show the sub window
	subWin->showMaximized();
	mdiArea->setActiveSubWindow(subWin);
	glDisplay->setFocus();
	return viewWindow;
}

void ViewManager::refreshDisplayByPID(int pID) {
	int i;
	for (i=0; i<(int)m_viewWindows.size(); i++) {
		ViewWindow* curWin = m_viewWindows[i];
		// If this is a null pointer, skip it
		if (!curWin) {
			continue;
		}
		if (curWin->getPipelineID()==pID) {
			curWin->getViewDisplay()->setVisualMapResult(
					getPipelineManager()->getVisualMapResult(pID));
			curWin->updateWindow();
		}
	}

	getMainWnd()->updateMenuToolbarDock();
}

void ViewManager::refreshAllDisplays() {
	int i;
	for (i=0; i<(int)m_viewWindows.size(); i++) {
		ViewWindow* curWin = m_viewWindows[i];
		// If this is a null pointer, skip it
		if (!curWin) {
			continue;
		}
		curWin->getViewDisplay()->setVisualMapResult(
				getPipelineManager()->getPipeline(curWin->getPipelineID())->assemble(0));
		curWin->updateWindow();
	}

	getMainWnd()->updateMenuToolbarDock();
}

void ViewManager::changeToParallel(){
	int i = getActiveViewWindowNo();
	if (i>=0 && i<(int)m_viewWindows.size()) {
		changeToVis(i, XmdvTool::VISTYPE_PA);
	}
}

void ViewManager::changeToScatter() {
	int i = getActiveViewWindowNo();
	if (i>=0 && i<(int)m_viewWindows.size()) {
		changeToVis(i, XmdvTool::VISTYPE_SC);
	}
}

void ViewManager::changeToGlyph() {
	int i = getActiveViewWindowNo();
	if (i>=0 && i<(int)m_viewWindows.size()) {
		changeToVis(i, XmdvTool::VISTYPE_GL);
	}
}

void ViewManager::changeToDimstack() {
	int i = getActiveViewWindowNo();
	if (i>=0 && i<(int)m_viewWindows.size()){
		if (changeToVis(i, XmdvTool::VISTYPE_DI)) {
			getMainWnd()->updateMenuToolbarDock();
		}
	}
}

void ViewManager::changeToPixel() {
	int i = getActiveViewWindowNo();
	if (i>=0 && i<(int)m_viewWindows.size()) {
		changeToVis(i, XmdvTool::VISTYPE_PI);
		getMainWnd()->getHandleTools()->getPixelCustomizationManager()->getDialog()->applyChanges(m_viewWindows[i]->getViewDisplay());
		getMainWnd()->updateMenuToolbarDock();
	}
}

bool ViewManager::changeToVis(int viewWinNo, XmdvTool::VISTYPE visType) {
	// Get the pipeline corresponding to this view
	Pipeline* oldline = m_viewWindows[viewWinNo]->getPipeline();
	bool success = false;
	// Get the pipeline type
	switch (oldline->getPipelineType()) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		// This is a flat normal pipeline
		success = changeToFlatPipeline(viewWinNo, visType);
		break;
	case XmdvTool::PLTYPE_DIMR :
		// This pipeline is used to do dimension reduction
		success = changeToDimRPipeline(viewWinNo, visType);
		break;
	case XmdvTool::PLTYPE_SBB :
		// This is a hierarchical display mode pipeline.
		// We change back to flat normal mode.
		success = changeToFlatPipeline(viewWinNo, visType);
		break;
	}

	return success;
}

bool ViewManager::changeToFlatPipeline() {
	int viewWinNo = getActiveViewWindowNo();

	if (viewWinNo<0 || viewWinNo>=(int)m_viewWindows.size()) {
		// If no active window, return immediately
		return true;
	}

	// get the visualization type for the current view
	XmdvTool::VISTYPE visType =  m_viewWindows[viewWinNo]->getVisType();

	// call the other function to complete the pipeline switch
	return changeToFlatPipeline(viewWinNo, visType);
}

bool ViewManager::changeToFlatPipeline(int viewWinNo, XmdvTool::VISTYPE visType) {
	Pipeline* oldline = m_viewWindows[viewWinNo]->getPipeline();
	int oldID = oldline->getPipelineID();
	int newID;

	// If users want to switch from a non Flat mode to Flat mode,
	// we need to retrieve back the flat pipeline, which must exist
	// because the default mode is a flat one when the dataset was opened.
	if ( oldline->getPipelineType()!=XmdvTool::PLTYPE_FLATNORMAL ) {
		if (m_viewWindows[viewWinNo]->getFlatPipelineID()<0) {
			// We should never get here
			assert(0);
		} else {
			newID = m_viewWindows[viewWinNo]->getFlatPipelineID();
		}
	} else {
		newID = oldID;
	}

	bool success = m_viewWindows[viewWinNo]->changeVis(newID, visType);

	return success;
}

void ViewManager::changeToHierParallel() {
	int i = getActiveViewWindowNo();

	if (i>=0 && i<(int)m_viewWindows.size()) {
		changeToSBBPipeline(i, XmdvTool::VISTYPE_PA);
	}
}

void ViewManager::changeToHierScatter() {
	int i = getActiveViewWindowNo();

	if (i>=0 && i<(int)m_viewWindows.size()) {
		changeToSBBPipeline(i, XmdvTool::VISTYPE_SC);
	}
}

void ViewManager::changeToHierGlyph() {
	int i = getActiveViewWindowNo();

	if (i>=0 && i<(int)m_viewWindows.size()) {
		changeToSBBPipeline(i, XmdvTool::VISTYPE_GL);
	}
}


void ViewManager::changeToHierStacking() {
	int i = getActiveViewWindowNo();

	if (i>=0 && i<(int)m_viewWindows.size()) {
		if (changeToSBBPipeline(i, XmdvTool::VISTYPE_DI)) {
			getMainWnd()->updateMenuToolbarDock();
		}
	}
}

void ViewManager::changeToHierPixel() {
	int i = getActiveViewWindowNo();

	if (i>=0 && i<(int)m_viewWindows.size()) {
		changeToSBBPipeline(i, XmdvTool::VISTYPE_PI);
	}
}

bool ViewManager::changeToSBBPipeline(int viewWinNo, XmdvTool::VISTYPE visType) {
	PipelineManager* pm = getPipelineManager();

	Pipeline* oldline = m_viewWindows[viewWinNo]->getPipeline();
	Data* dataset = oldline->getOrigDataset();
	OkcData* okcdata =  dynamic_cast<OkcData*>(dataset);
	int newID;

	VisualMapResult *vmr;

	// If users want to switch from a non SBB mode to SBB mode,
	// we need to create a new SBB pipeline or retrieve back
	// a used one.
	if ( oldline->getPipelineType()!=XmdvTool::PLTYPE_SBB ) {
		// The ID of the SBB pipelines
		if (m_viewWindows[viewWinNo]->getSBBPipelineID()<0) {
			// If this view was never associated with a SBB pipeline.
			// create a new one.
			newID = pm->createSBBPipeline(okcdata);
			// Write a record for future switching back
			Data* inputData = m_viewWindows[viewWinNo]->getInputData();
			// Set this pipeline ID to all windows associated with the same data
			setSBBPipelineID(inputData, newID);

			// Replace the old vmr with the new one
		    vmr = pm->m_pipelines[newID]->assemble(0);
		    SAFE_DELETE(pm->m_visualMapResults[newID]);
		    pm->m_visualMapResults[newID] = vmr;

		} else {
			// If this view has been associated with a SBB pipeline,
			// use the old one
			newID = m_viewWindows[viewWinNo]->getSBBPipelineID();
			m_viewWindows[viewWinNo]->setSBBPipelineID(newID);
		    vmr = pm->m_visualMapResults[newID];
		}

	} else {
		newID = m_viewWindows[viewWinNo]->getSBBPipelineID();
	}

	bool success = m_viewWindows[viewWinNo]->changeVis(newID, visType);

    return success;
}

void ViewManager::setSBBPipelineID(Data* inputData, int newID) {
	// Get all windows associated with this input data
	vector<ViewWindow*> wins = getViewWindows(inputData);
	int i;
	for ( i=0; i<(int)wins.size(); i++) {
		wins[i]->setSBBPipelineID(newID);
	}
}

bool ViewManager::changeToDimRPipeline() {
	int viewWinNo = getActiveViewWindowNo();

	if (viewWinNo<0 || viewWinNo>=(int)m_viewWindows.size()) {
		// If no active window, return immediately
		return true;
	}

	// get the visualization type for the current view
	XmdvTool::VISTYPE visType =  m_viewWindows[viewWinNo]->getVisType();

	// call the other function to complete the pipeline switch
	return changeToDimRPipeline(viewWinNo, visType);
}

bool ViewManager::changeToDimRPipeline(int viewWinNo, XmdvTool::VISTYPE visType) {
	PipelineManager* pm = getPipelineManager();

	Pipeline* oldline = m_viewWindows[viewWinNo]->getPipeline();
	Data* dataset = oldline->getOrigDataset();
	OkcData* okcdata =  dynamic_cast<OkcData*>(dataset);
	int newID;

	VisualMapResult *vmr;

	// If users want to switch from a non SBB mode to SBB mode,
	// we need to create a new SBB pipeline or retrieve back
	// a used one.
	if ( oldline->getPipelineType()!=XmdvTool::PLTYPE_DIMR ) {
		// The ID of the SBB pipelines
		if (m_viewWindows[viewWinNo]->getDimRPipelineID()<0) {
			// If this view was never associated with a dimension reduction pipeline.
			// create a new one.
			newID = pm->createDimRPipeline(okcdata);
			// Write a record for future switching back
			Data* inputData = m_viewWindows[viewWinNo]->getInputData();
			// Set this pipeline ID to all windows associated with the same data
			setDimRPipelineID(inputData, newID);

			// Replace the old vmr with the new one
		    vmr = pm->m_pipelines[newID]->assemble(0);
		    SAFE_DELETE(pm->m_visualMapResults[newID]);
		    pm->m_visualMapResults[newID] = vmr;

		} else {
			// If this view has been associated with a dimension reduction pipeline,
			// use the old one
			newID = m_viewWindows[viewWinNo]->getDimRPipelineID();
			m_viewWindows[viewWinNo]->setDimRPipelineID(newID);
		    vmr = pm->m_visualMapResults[newID];
		}

	} else {
		newID = m_viewWindows[viewWinNo]->getDimRPipelineID();
	}

	bool success = m_viewWindows[viewWinNo]->changeVis(newID, visType);

    return success;
}

void ViewManager::setDimRPipelineID(Data* inputData, int newID) {
	// Get all windows associated with this input data
	vector<ViewWindow*> wins = getViewWindows(inputData);
	int i;
	for ( i=0; i<(int)wins.size(); i++) {
		wins[i]->setDimRPipelineID(newID);
	}
}

void ViewManager::mdiSubWindowActivated(QMdiSubWindow* window) {
	if (!window) {
		// if window==0, no window is active currently
		return ;
	}
	int i;
	for (i=0; i<(int)m_viewWindows.size(); i++) {
		if ( !m_viewWindows[i] ) {
			// If this pointer is 0, it is an old windows that has been removed
			continue;
		}
		if (m_viewWindows[i]->getSubWindow()==window) {
			m_activeViewWindowNo = i;
			break;
		}
	}
	getMainWnd()->updateMenuToolbarDock();
}

bool ViewManager::willOuputToVis(int pID, XmdvTool::VISTYPE visType) {
	int i, n = m_viewWindows.size();
	for (i=0; i<n; i++) {
		if ( !m_viewWindows[i] ) {
			// If this is null pointer, it is an expired window that has been removed
			continue;
		}
		if (m_viewWindows[i]->getPipelineID()==pID
				&& m_viewWindows[i]->getVisType()==visType) {
			return true;
		}
	}
	return false;
}

vector<ViewWindow*> ViewManager::getViewWindows(Data* origData) {
	int i;
	vector<ViewWindow*> result;
	for ( i=0; i<(int)m_viewWindows.size(); i++) {
		// If this is null pointer, skip it
		if ( !m_viewWindows[i] ) {
			continue;
		}

		// Get the input dataset of this view
		Data* inputData = m_viewWindows[i]->getInputData();
		// Check whether this data is the data in the parameter
		if ( inputData == origData ) {
			// If yes, close its sub window.
			result.push_back(m_viewWindows[i]);
		}
	}

	return result;
}

void ViewManager::activateView(ViewWindow* viewWin) {
	m_mdiArea->setActiveSubWindow( viewWin->getSubWindow() );
}

void ViewManager::closeViewWindow(ViewWindow *viewWin) {
	QMdiSubWindow* subWin = viewWin->getSubWindow();
	subWin->close();
}

void ViewManager::closeViewWindows(Data* origData) {
	// We search all windows and close them one by one.
	// We do not need to explicitly delete the dataset
	// because it will removed automatically when the last view
	// of this dataset is deleted.
	vector<ViewWindow*> wins = getViewWindows(origData);
	int i;
	for ( i=0; i<(int)wins.size(); i++) {
		QMdiSubWindow* subWin = wins[i]->getSubWindow();
		subWin->close();
	}
}


bool ViewManager::deleteViewWindow(MdiSubWindow* subWin) {
	int i, n = m_viewWindows.size();
	int pID = 0;
	for ( i=0; i<n; i++) {
		if ( m_viewWindows[i] &&
				m_viewWindows[i]->getSubWindow() ==	(QMdiSubWindow*)subWin ) {
			// If this is just the view window associated with the mdi sub windows,
			// Get the pipeline ID first
			pID = m_viewWindows[i]->getPipelineID();
			delete m_viewWindows[i];
			m_viewWindows[i] = 0;
		}
	}

	// Check whether all views associated with the data have been removed
	PipelineManager* pm = getMainWnd()->getPipelineManager();
	Data* origData = pm->getPipeline(pID)->getOrigDataset();
	// allPIDs contains all pipelines associated with this data
	vector<int> allPIDs = pm->getPipelinesByData(origData);
	int viewCount=0;
	// Count the number of views associated with the pipelines in this vector
	for ( i=0; i<(int)allPIDs.size(); i++) {
		viewCount += getViewWindowsCount( allPIDs[i] );
	}
	if ( viewCount == 0 ) {
		for ( i=0; i<(int)allPIDs.size(); i++) {
			pm->deletePipeline( allPIDs[i] );
		}
		pm->deleteOrigDataset(origData);
	}
	m_mainWnd->updateMenuToolbarDock();

	return true;
}

void ViewManager::saveImage(QString fileName, QString) {
	ViewWindow *activeWin = getActiveViewWindow();
	GLDisplay* glWidget = activeWin->getGLDisplay();
	QPixmap pixmap = glWidget->renderPixmap();
	if ( !pixmap.save(fileName) ) {
        QMessageBox::critical(m_mainWnd, QObject::tr("XmdvTool"),
            QObject::tr("Failed to save to %1").arg(fileName));
	}

}
