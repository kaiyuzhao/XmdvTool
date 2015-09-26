/*
 * MdiSubWindow.cpp
 *
 *  Created on: Oct 5, 2009
 *      Author: zyguo
 */

#include "view/MdiSubWindow.h"

#include <QCloseEvent>

#include "view/ViewManager.h"

MdiSubWindow::MdiSubWindow(QWidget* parent, ViewManager* vm)
	: QMdiSubWindow(parent) {
	m_viewManager = vm;
}

MdiSubWindow::~MdiSubWindow() {
}

void MdiSubWindow::setGLDisplay(GLDisplay* _glDisplay){
	this->glDisplay = _glDisplay;
}

void MdiSubWindow::resizeEvent(QResizeEvent *event) {
	QMdiSubWindow::resizeEvent(event);
	glDisplay->resizeOnWindow();
}

void MdiSubWindow::closeEvent (QCloseEvent * event) {
	bool closed = m_viewManager->deleteViewWindow(this);
	if (closed) {
		event->accept();
	} else {
		event->ignore();
	}
}
