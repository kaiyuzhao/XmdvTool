/*
 * GLDisplay.cpp
 *
 *  Created on: Apr. 1, 2008
 *      Author: Zaixian Xie
 */

#include "view/GLDisplay.h"

#include "main/XmdvTool.h"
#include "pipeline/Pipeline.h"
#include "interaction/MouseKeyboardStatus.h"
#include "view/GLCanvas.h"
#include "view/ViewWindow.h"
#include "view/ViewDisplay.h"
#include "view/ViewManager.h"
#include "color/ColorManager.h"
#include "datatype/RGBt.h"

#include <QGLWidget>
#include <QtGui>
#include <QtOpenGL>
#include <QPalette>
#include <QMessageBox>
#include <QScrollArea>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFrame>
#include <QAbstractScrollArea>
#include <QWidget>
#include <QScrollBar>

#include <ios>
#include <fstream>
using namespace std;

GLDisplay::GLDisplay(QWidget *parent, ViewWindow *viewWindow):
	QGLWidget(parent) {
	scrollArea = (QScrollArea*)this->parentWidget();

	this->m_viewWindow = viewWindow;
	m_mouseKeyboardStatus = new MouseKeyboardStatus();
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);
	zoomFactorX = 1.00;
	zoomFactorY = 1.00;
	zoomDirection = 0;
}

GLDisplay::~GLDisplay()
{
}

void GLDisplay::initializeGL()
{
	RGBt bg(g_globals.colorManager->getColor(ColorManager::BG));
	glClearColor(bg.R,bg.G,bg.B,1.0);
}

void GLDisplay::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho ( (-CANVAS_BORD), (1+CANVAS_BORD), (1+CANVAS_BORD), (-CANVAS_BORD), -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void GLDisplay::paintGL()
{
    glViewport(0, 0, width(), height() );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho ( (-CANVAS_BORD), (1+CANVAS_BORD), (1+CANVAS_BORD), (-CANVAS_BORD), -1, 1);
    glMatrixMode(GL_MODELVIEW);

    RGBt bg(g_globals.colorManager->getColor(ColorManager::BG));
	glClearColor(bg.R,bg.G,bg.B,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getViewWindow()->getViewDisplay()->paint();
}

GLCanvas* GLDisplay::getGLCanvas(){
	return this->getViewWindow()->getViewDisplay()->getCanvas();
}

MouseKeyboardStatus* GLDisplay::getMouseKeyboardStatus(){
	return this->m_mouseKeyboardStatus;
}

ViewWindow* GLDisplay::getViewWindow() {
	return m_viewWindow;
}

ViewDisplay* GLDisplay::getViewDisplay() {
	return getViewWindow()->getViewDisplay();
}

void GLDisplay::keyPressEvent(QKeyEvent* event){

	MouseKeyboardStatus* mouseKey = this->getMouseKeyboardStatus();

	if( event->key() == Qt::Key_Alt){
		mouseKey->altkey = true;
	}
	if( event->key() == Qt::Key_Control){
		mouseKey->controlkey = true;
	}
	if( event->key() == Qt::Key_Shift){
		mouseKey->shiftkey = true;
		this->setCursorShape(Qt::PointingHandCursor);
	}
}

void GLDisplay::setCursorShape(Qt::CursorShape cursorShape){
	QCursor cursor = this->cursor();
	cursor.setShape(cursorShape);
	this->setCursor(cursor);
}

void GLDisplay::keyReleaseEvent(QKeyEvent* event){
	GLCanvas* canvas = this->getGLCanvas();
	if (canvas == NULL) {
		return;
	}

	MouseKeyboardStatus* mouseKeyboardStatus = this->getMouseKeyboardStatus();
	if (event->key() == Qt::Key_Alt) {
		mouseKeyboardStatus->altkey = false;
	}
	if (event->key() == Qt::Key_Control) {
		mouseKeyboardStatus->controlkey = false;
	}
	if (event->key() == Qt::Key_Shift) {
		mouseKeyboardStatus->shiftkey = false;
		this->setCursorShape(Qt::ArrowCursor);
	}
	if (event->key() == Qt::Key_1) {
		this->zoomFactorX *= 1.25;
		resizeOnWindow();
	}
	if (event->key() == Qt::Key_2) {
		this->zoomFactorX *= 0.8;
		resizeOnWindow();
	}
	if (event->key() == Qt::Key_3) {
		this->zoomFactorY *= 1.25;
		resizeOnWindow();
	}
	if (event->key() == Qt::Key_4) {
		this->zoomFactorY *= 0.8;
		resizeOnWindow();

	}
	if (event->key() == Qt::Key_5) {
		this->zoomFactorX = 1.0;
		this->zoomFactorY = 1.0;
		resizeOnWindow();

	}
}

void GLDisplay::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value()
                             + ((factor - 1) * scrollBar->pageStep()/2)));
}

int GLDisplay::getMouseButton(QMouseEvent *e){
	int num = -1;

	if (e->button() == Qt::LeftButton) {
		num = 0;
	}
	if (e->button() == Qt::MidButton) {
		num = 1;
	}
	if (e->button() == Qt::RightButton) {
		num = 2;
	}

	return num;
}

void GLDisplay::mousePressEvent(QMouseEvent *event){

	// If this is in the SBB pipeline, do nothing,
	// because mouse event is designed for multi-dimensional brush
	// and SBB mode does not support it.
	XmdvTool::PLTYPE plType = getViewWindow()->getPipeline()->getPipelineType();
	if ( plType == XmdvTool::PLTYPE_SBB  ) {
		return;
	}

	GLCanvas* canvas = this->getGLCanvas();
	if (canvas==NULL){
		return;
	}

	MouseKeyboardStatus* mouseKeyboardStatus = this->getMouseKeyboardStatus();
	int num = 0;
	num = this->getMouseButton(event);

	if(mouseKeyboardStatus->shiftkey){
		//this->setCursorShape(Qt::ClosedHandCursor);
	}

	if (mouseKeyboardStatus->altkey && num == 0) {
		num = 1;
	}

	int stoppan = true;
	if (num == 2) {
		if (!stoppan) {
			canvas->startPan = true;
		}
	}

	m_mousePress = event->pos();

	int x = (int)m_mousePress.x();
	int y = this->height() - (int)m_mousePress.y() - 1;

	for (int i=0; i<3; i++){
		mouseKeyboardStatus->button[i] = false;
	}
	mouseKeyboardStatus->button[num] = true;
	mouseKeyboardStatus->press_pos.X = mouseKeyboardStatus->curr_pos.X = x;
	mouseKeyboardStatus->press_pos.Y = mouseKeyboardStatus->curr_pos.Y = y;
	mouseKeyboardStatus->prev_pos = mouseKeyboardStatus->curr_pos;

	//How to set?
	if (getViewDisplay()){
		getViewDisplay()->inputButtonPress(num, x, y);
	}
}


void GLDisplay::mouseMoveEvent(QMouseEvent *event){
	if (getViewWindow()->getViewDisplay()) {
		GLCanvas* canvas = this->getGLCanvas();
		if (canvas == NULL) {
			return;
		}
		MouseKeyboardStatus* mouseKeyboardStatus = this->getMouseKeyboardStatus();
		m_mousePress = event->pos();

		int x = (int)m_mousePress.x();
		int y = this->height() - (int)m_mousePress.y() - 1;

		mouseKeyboardStatus->prev_pos = mouseKeyboardStatus->curr_pos;
		mouseKeyboardStatus->curr_pos.X = x;
		mouseKeyboardStatus->curr_pos.Y = y;

		getViewDisplay()->inputMouseMotion(x, y);
	}
}

void GLDisplay::mouseReleaseEvent(QMouseEvent *event){
	// If this is in the SBB pipeline, do nothing,
	// because mouse event is designed for multi-dimensional brush
	// and SBB mode does not support it.
	XmdvTool::PLTYPE plType = getViewWindow()->getPipeline()->getPipelineType();
	if ( plType == XmdvTool::PLTYPE_SBB ) {
		return;
	}

	int num = this->getMouseButton(event);
	GLCanvas* canvas = this->getGLCanvas();
	if(canvas==NULL){
		return;
	}
	MouseKeyboardStatus* mouseKeyboardStatus = this->getMouseKeyboardStatus();

	m_mousePress = event->pos();
	int x = (int)m_mousePress.x();
	int y = this->height() - (int)m_mousePress.y() - 1;

	mouseKeyboardStatus->press_pos.X = mouseKeyboardStatus->curr_pos.X = 0;
	mouseKeyboardStatus->press_pos.Y = mouseKeyboardStatus->curr_pos.Y = 0;

	if (getViewDisplay())
		getViewDisplay()->inputButtonRelease(num, x, y);
}

QScrollArea* GLDisplay::getScrollArea(){
	return scrollArea;
}

void GLDisplay::setZoomFactorX(double _zoomFactorX){
	this->zoomFactorX = _zoomFactorX;
}

void GLDisplay::setZoomFactorY(double _zoomFactorY){
	this->zoomFactorY = _zoomFactorY;
}

double GLDisplay::getZoomFactorX(){
	return this->zoomFactorX;
}

double GLDisplay::getZoomFactorY(){
	return this->zoomFactorY;
}

void GLDisplay::resizeOnWindow()
{
	makeCurrent();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	int ws = scrollArea->size().width();
	int hs = scrollArea->size().height();

	scrollArea->setWidgetResizable(false);

	int distanceToBorder = 21;

	// We do not implicitly call resizeGL.  When we call resize, the resizeGL
	// will be called based on Qt Logistics.
	//resizeGL((int) (zoomFactorX * (double) (ws-distanceToBorder)), (int) (zoomFactorY * (double) (hs-distanceToBorder)));
	resize((int) (zoomFactorX * (double) (ws-distanceToBorder)), (int) (zoomFactorY * (double) (hs-distanceToBorder)));
}

void GLDisplay::zoomReset(){

	if(zoomDirection==0){
		this->zoomFactorX = 1.0;
		this->zoomFactorY = 1.0;
	}else if(zoomDirection==1){
		this->zoomFactorX = 1.0;

	}else if(zoomDirection==2){
		this->zoomFactorY = 1.0;

	}

	resizeOnWindow();
}

void GLDisplay::zoomIn(){
	double zoomInScaleFactor = 1.25;
	if(zoomDirection==0){
		this->zoomFactorX *= zoomInScaleFactor;
		this->zoomFactorY *= zoomInScaleFactor;
	}else if(zoomDirection==1){
		this->zoomFactorX *= zoomInScaleFactor;

	}else if(zoomDirection==2){
		this->zoomFactorY *= zoomInScaleFactor;

	}

	resizeOnWindow();
}
void GLDisplay::zoomOut(){
	double zoomInScaleFactor = 0.8;
	if (zoomDirection == 0) {
		this->zoomFactorX *= zoomInScaleFactor;
		this->zoomFactorY *= zoomInScaleFactor;
	} else if (zoomDirection == 1) {
		this->zoomFactorX *= zoomInScaleFactor;

	} else if (zoomDirection == 2) {
		this->zoomFactorY *= zoomInScaleFactor;

	}

	resizeOnWindow();
}

int GLDisplay::getZoomDirection(){
	return this->zoomDirection;
}
void GLDisplay::setZoomDirection(int _zoomDirection) {
	this->zoomDirection = _zoomDirection;
}


