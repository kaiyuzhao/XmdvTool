/*
 * HandleZoom.cpp
 *
 *  Created on: Oct 7, 2009
 *      Author: zyguo
 */

#include "HandleZoom.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/GLDisplay.h"
#include "XmdvTool.h"
#include "view/auxiliary/ZoomLineEdit.h"

#include <QtGui/QComboBox>

HandleZoom::HandleZoom(XmdvToolMainWnd *mainWnd) {
	this->mainWnd = mainWnd;
}

HandleZoom::~HandleZoom() {
}

void HandleZoom::setZoomDirection(int _zoomDirection){
	GLDisplay* glDispaly = getGLDisplay();
	if(glDispaly==0){
		return;
	}
	glDispaly->setZoomDirection(_zoomDirection);
}

void HandleZoom::zoomIn(){
	GLDisplay* glDispaly = getGLDisplay();
	if (glDispaly == 0) {
		return;
	}
	glDispaly->zoomIn();
	changeLineEditValue();
}
void HandleZoom::zoomOut(){
	GLDisplay* glDispaly = getGLDisplay();
	if (glDispaly == 0) {
		return;
	}
	glDispaly->zoomOut();
	changeLineEditValue();
}

void HandleZoom::zoomReset(){
	GLDisplay* glDispaly = getGLDisplay();
	if (glDispaly == 0) {
		return;
	}
	glDispaly->zoomReset();
	changeLineEditValue();
}

void HandleZoom::updateZoomBarInfoFromGLDisplay(){
	GLDisplay* glDispaly = getGLDisplay();
	if (glDispaly == 0) {
		return;
	}

	int zoomFactorX = (int) (glDispaly->getZoomFactorX() * 100.0);
	int zoomFactorY = (int) (glDispaly->getZoomFactorY() * 100.0);
	ZoomLineEdit* zoomLineEditX = mainWnd->getLineEdit_ZoomX();
	ZoomLineEdit* zoomLineEditY = mainWnd->getLineEdit_ZoomY();

	zoomLineEditX->setText(QString::number(zoomFactorX));
	zoomLineEditY->setText(QString::number(zoomFactorY));

	int zoomDirection = glDispaly->getZoomDirection();
	QComboBox *comboBox_ZoomDirection = mainWnd->getComboBox_ZoomDirection();
	comboBox_ZoomDirection->setCurrentIndex(zoomDirection);
}

void HandleZoom::changeLineEditValue(){
	GLDisplay* glDispaly = getGLDisplay();
	if (glDispaly == 0) {
		return;
	}

	int zoomFactorX = (int)(glDispaly->getZoomFactorX() * 100.0);
	int zoomFactorY = (int)(glDispaly->getZoomFactorY() * 100.0);
	ZoomLineEdit* zoomLineEditX = mainWnd->getLineEdit_ZoomX();
	ZoomLineEdit* zoomLineEditY = mainWnd->getLineEdit_ZoomY();

	zoomLineEditX->setText(QString::number(zoomFactorX));
	zoomLineEditY->setText(QString::number(zoomFactorY));
}

GLDisplay* HandleZoom::getGLDisplay(){
	if (mainWnd->getViewManager() == 0) {
		return 0;
	}
	if (mainWnd->getViewManager()->getActiveViewWindow() == 0) {
		return 0 ;
	}
	if (mainWnd->getViewManager()->getActiveViewWindow()->getGLDisplay() == 0) {
		return 0 ;
	}

	return mainWnd->getViewManager()->getActiveViewWindow()->getGLDisplay();
}

void HandleZoom::setZoomFactors(){
	GLDisplay* glDispaly = getGLDisplay();
	if (glDispaly == 0) {
		return;
	}

	ZoomLineEdit* zoomLineEditX = mainWnd->getLineEdit_ZoomX();
	ZoomLineEdit* zoomLineEditY = mainWnd->getLineEdit_ZoomY();

	QString sValueZoomX = zoomLineEditX->text();
	QString sValueZoomY = zoomLineEditY->text();
	bool okZoomX, okZoomY;

	int zoomX = sValueZoomX.toInt(&okZoomX, 10);
	int zoomY = sValueZoomY.toInt(&okZoomY, 10);

	double zoomFactorX = (double)zoomX/100.0;
	double zoomFactorY = (double)zoomY/100.0;
	if(okZoomX&&okZoomY){
		glDispaly->setZoomFactorX(zoomFactorX);
		glDispaly->setZoomFactorY(zoomFactorY);
		glDispaly->resizeOnWindow();
	}else{
		changeLineEditValue();
	}
}

