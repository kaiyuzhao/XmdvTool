#include "view/ViewDisplay.h"

#include <assert.h>
#include <Qt>
#include <QGLWidget>
#include <QMessageBox>

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "visualmap/VisualMapResult.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/GLDisplay.h"
#include "view/GLCanvas.h"
#include "interaction/MouseKeyboardStatus.h"

ViewDisplay::ViewDisplay(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm)
{
	// The PipelineManager cannot be NULL
	assert(pm);
	// The pipeline ID cannot be negative
	assert(pipelineID>=0);

	m_viewWindow = viewWindow;
	if (viewWindow) {
		m_glDisplay = viewWindow->getGLDisplay();
	}
	m_pipelineManager = pm;
	m_pipelineID = pipelineID;
	m_viewManager = vm;
	m_canvas = 0;
	m_visualMapResult = 0;

	// create the instance of class GLCanvas
	if (m_glDisplay) {
		SAFE_DELETE(m_canvas);
		m_canvas = new GLCanvas((QGLWidget*)m_glDisplay);
	}
}

ViewDisplay::~ViewDisplay()
{
	SAFE_DELETE(m_canvas);
}

ViewWindow* ViewDisplay::getViewWindow() {
	return m_viewWindow;
}

void ViewDisplay::setGLDisplay(GLDisplay *glDisplay) {
	// glWidget cannot be NULL
	assert(glDisplay);
	this->m_glDisplay = glDisplay;
	// create the instance of class GLCanvas
	if (this->m_glDisplay) {
		SAFE_DELETE(m_canvas);
		m_canvas = new GLCanvas((QGLWidget*)(this->m_glDisplay));
	}
}

GLDisplay* ViewDisplay::getGLDisplay() {
	return this->m_glDisplay;
}

GLCanvas *ViewDisplay::getCanvas() {
	return this->m_canvas;
}

void ViewDisplay::setVisualMapResult(VisualMapResult *vmr) {
	this->m_visualMapResult = vmr;
}

VisualMapResult* ViewDisplay::getVisualMapResult() {
	return this->m_visualMapResult;
}

PipelineManager* ViewDisplay::getPipelineManager() {
	return this->m_pipelineManager;
}

int ViewDisplay::getPipelineID() {
	return this->m_pipelineID;
}

void ViewDisplay::setPipelineID(int id) {
	m_pipelineID = id;
}

Pipeline* ViewDisplay::getPipeline() {
	return getPipelineManager()->getPipeline(getPipelineID());
}

ViewManager* ViewDisplay::getViewManager() {
	return this->m_viewManager;
}

XmdvTool::VISTYPE ViewDisplay::getVisType() {
	return getViewWindow()->getVisType();
}

bool ViewDisplay::render(){
	QMessageBox *msgBox;
	switch (checkRenderable()){
	case XmdvTool::FAIL_RENDERABLE:
		msgBox = new QMessageBox();
		msgBox->setText(getRenderErrorMessage());
		msgBox->setStandardButtons(QMessageBox::Ok);
		msgBox->setDefaultButton(QMessageBox::Ok);
		msgBox->exec();
		return false;
	case XmdvTool::PASS_RENDERABLE:
		return true;
	case XmdvTool::WARN_RENDERABLE:
		msgBox = new QMessageBox();
		msgBox->setText(getRenderWarningMessage());
		msgBox->setStandardButtons(QMessageBox::Open | QMessageBox::Cancel);
		msgBox->setDefaultButton(QMessageBox::Cancel);
		return (bool) msgBox->exec();
	default:
		return false;
	}
}

XmdvTool::CHECK_RENDERABLE_RESULT ViewDisplay::checkRenderable(){
	//by default, no check
	return XmdvTool::PASS_RENDERABLE;
}

QString ViewDisplay::getRenderErrorMessage(){
	return "";
}

QString ViewDisplay::getRenderWarningMessage(){
	return "";
}

void ViewDisplay::paint() {
}

void ViewDisplay::inputButtonPress(int, int , int ) {
}

void ViewDisplay::inputMouseMotion(int, int) {
}

void ViewDisplay::inputButtonRelease(int, int, int) {
}

void ViewDisplay::setCursorShape(Qt::CursorShape cursorShape){
	QCursor cursor = this->getGLDisplay()->cursor();
	cursor.setShape(cursorShape);
	m_glDisplay->setCursor(cursor);
}

MouseKeyboardStatus* ViewDisplay::getMouseKeyboardStatus() {
	return getViewWindow()->getGLDisplay()->getMouseKeyboardStatus();
}
