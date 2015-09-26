/*
 * ViewWindow.cpp
 *
 *  Created on: Jan 21, 2009
 *      Author: Zaixian Xie
 */

#include "view/ViewWindow.h"

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "data/Data.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Pipeline.h"
#include "view/ViewManager.h"
#include "view/ViewDisplay.h"
#include "view/GLDisplay.h"
#include "view/multidim/VisParcoord.h"
#include "view/multidim/VisScatter.h"
#include "view/multidim/VisGlyph.h"
#include "view/multidim/VisDimstack.h"
#include "view/multidim/VisPixel.h"

#include <QMdiSubWindow>
#include <QGLWidget>
#include <QMessageBox>

ViewWindow::ViewWindow() {
	m_viewDisplay = 0;
	m_FlatPipelineID = -1;
	m_SBBPipelineID = -1;
	m_DimRPipelineID = -1;
}

ViewWindow::~ViewWindow() {
	// release memory occupied by ViewDisplay and GLDisplay
	SAFE_DELETE(m_viewDisplay);
	SAFE_DELETE(m_glDisplay);
}

QString ViewWindow::getVisTypeString(XmdvTool::VISTYPE visType) {
	QString result;
	switch ( visType ) {
	case XmdvTool::VISTYPE_PA :
		result = "Parallel coordinates";
		break;
	case XmdvTool::VISTYPE_SC :
		result = "Scatterplot matrices";
		break;
	case XmdvTool::VISTYPE_GL :
		result = "Glyphs";
		break;
	case XmdvTool::VISTYPE_DI :
		result = "Dimensions stackings";
		break;
	case XmdvTool::VISTYPE_PI :
		result = "Pixel-oriented displays";
		break;
	}
	return result;

}

QStringList ViewWindow::toStringList() {
	QStringList result;

	result << QString("View:");
	// Generate the string of vis type
	result << QString("-- Type : %1 ")
		.arg( ViewWindow::getVisTypeString( getVisType() ) );
	result << QString("-- Pipeline settings");

	Pipeline* pl = getPipeline();
	QStringList plStrList = pl->toStringList();
	plStrList.removeAt(0);
	int i;
	for ( i=0; i<plStrList.size(); i++) {
		plStrList[i].prepend( "  " );
	}

	result << plStrList;

	return result;
}

void ViewWindow::setSubWindow(QMdiSubWindow* subWindow) {
	this->m_subWindow = subWindow;
}

void ViewWindow::setPipelineManager(PipelineManager* pm) {
	this->m_pipelineManager = pm;
}

PipelineManager* ViewWindow::getPipelineManager() {
	return m_pipelineManager;
}


void ViewWindow::setPipelineID(int id) {
	m_pipelineID = id;
}

int ViewWindow::getPipelineID() {
	return m_pipelineID;
}

Data* ViewWindow::getInputData() {
	int pID = this->getPipelineID();
	Pipeline* pl = m_pipelineManager->getPipeline(pID);
	return pl->getOrigDataset();
}

// get the pointer to the pipeline associated with this window
Pipeline* ViewWindow::getPipeline() {
	return getPipelineManager()->getPipeline(getPipelineID());
}

void ViewWindow::setFlatPipelineID(int pid) {
	m_FlatPipelineID = pid;
}

int ViewWindow::getFlatPipelineID() {
	return m_FlatPipelineID;
}

void ViewWindow::setSBBPipelineID(int pid) {
	m_SBBPipelineID = pid;
}

int ViewWindow::getSBBPipelineID() {
	return m_SBBPipelineID;
}

void ViewWindow::setDimRPipelineID(int pid) {
	m_DimRPipelineID = pid;
}

int	ViewWindow::getDimRPipelineID() {
	return m_DimRPipelineID;
}


void ViewWindow::setViewManager(ViewManager* vm){
	m_viewManager = vm;
}

ViewManager* ViewWindow::getViewManager() {
	return m_viewManager;
}


QMdiSubWindow* ViewWindow::getSubWindow() {
	return this->m_subWindow;
}

// Set the order of this window
void ViewWindow::setOrder(int order) {
	m_order = order;
}

// get the order of this window
int ViewWindow::getOrder() {
	return m_order;
}

void ViewWindow::setTitle(QString& title) {
	m_title = title;
}

// get the title of sub window
QString ViewWindow::getTitle() {
	return m_title;
}

ViewDisplay* ViewWindow::getViewDisplay() {
	return this->m_viewDisplay;
}

void ViewWindow::setGLDisplay(GLDisplay *glDisplay) {
	this->m_glDisplay = glDisplay;
}

GLDisplay* ViewWindow::getGLDisplay() {
	return this->m_glDisplay;
}

void ViewWindow::setViewWinNo (int viewWinNo) {
	m_viewWinNo = viewWinNo;
}

int ViewWindow::getViewWinNo () {
	return m_viewWinNo;
}

void ViewWindow::createViewDisplay() {
	SAFE_DELETE(m_viewDisplay);
	PipelineManager* pm = getPipelineManager();
	int pID = getPipelineID();
	ViewManager* vm = getViewManager();
	if (m_visType == XmdvTool::VISTYPE_PA) {
		m_viewDisplay = new VisParcoord(this, pm, pID, vm);
	} else if  (m_visType == XmdvTool::VISTYPE_SC) {
		m_viewDisplay = new VisScatter(this, pm, pID, vm);
	} else if  (m_visType == XmdvTool::VISTYPE_GL) {
		m_viewDisplay = new VisGlyph(this, pm, pID, vm);
	} else if (m_visType == XmdvTool::VISTYPE_DI) {
		m_viewDisplay = new VisDimstack(this, pm, pID, vm);
	} else if (m_visType == XmdvTool::VISTYPE_PI) {
		m_viewDisplay = new VisPixel(this, pm, pID, vm);
	}
	m_viewDisplay->setGLDisplay(getGLDisplay());
}

void ViewWindow::setVisType(XmdvTool::VISTYPE visType) {
	m_visType = visType;
}

bool ViewWindow::changeVis(int pID, XmdvTool::VISTYPE visType) {
	//save the current vis type in case we need to roll back the change
	//if the vis type does not support the dataset for some reason.
	int oldPID = this->getPipelineID();
	XmdvTool::VISTYPE oldVisType = this->m_visType;

	// If pipeline ID and vis type keep no change, do nothing
	if ( pID==oldPID && oldVisType==visType )  {
		return false;
	}

	PipelineManager* pm = getPipelineManager();
	// We create a new view display no matter whether the vis type is changed
	this->setPipelineID(pID);
	this->setVisType(visType);
	createViewDisplay();

	// Re-assemble the pipeline but only visual mapping will be executed.
	pm->assemble(pID, -1);
	m_viewDisplay->setVisualMapResult(pm->getVisualMapResult(pID));

	//check if the ViewDisplay may be drawn or not
	//used to prevent switching to a display when the visualization style
	//breaks down or takes a long time to load given the dataset.
	if (!getViewDisplay()->render()){
		return false;
	}
	getViewManager()->getMainWnd()->updateMenuToolbarDock();

	// All views associated with this pipeline will be refreshed
	m_viewManager->refreshDisplayByPID(pID);
	//updateWindow();
	return true;
}

XmdvTool::VISTYPE ViewWindow::getVisType() {
	return this->m_visType;
}

void ViewWindow::updateWindow() {
	getGLDisplay()->updateGL();
}

QString ViewWindow::createViewName(int order) {
	return QString("view %1").arg(order);
}
