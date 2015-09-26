/*
 * HierNavDisplay.cpp
 *
 *  Created on: Aug 14, 2009
 *      Author: xiezx
 */

#include "interface/sbb/HierNavDisplay.h"

#include <QMouseEvent>

#include "main/XmdvTool.h"
#include "interface/sbb/HierNav.h"

HierNavDisplay::HierNavDisplay(QWidget *parent)
	:QGLWidget(parent) {
	setMouseTracking (true);
}

HierNavDisplay::~HierNavDisplay() {
}

void HierNavDisplay::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void HierNavDisplay::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho ( (-CANVAS_BORD), (1+CANVAS_BORD), (1+CANVAS_BORD), (-CANVAS_BORD), -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void HierNavDisplay::paintGL()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_hierNav->paint();
}

void HierNavDisplay::setHierNav(HierNav *hierNav){
	m_hierNav = hierNav;
}

HierNav* HierNavDisplay::getHierNav() {
	return m_hierNav;
}

void HierNavDisplay::mouseReleaseEvent(QMouseEvent *event){
	Qt::MouseButton button = event->button();
	int x = event->x();
	//int y = event->y();
	int y = this->height() - event->y() - 1;
	m_hierNav->inputButtonRelease(button, x, y);
}

void HierNavDisplay::mousePressEvent(QMouseEvent *event) {
	Qt::MouseButton button = event->button();
	int x = event->x();
	int y = this->height() - event->y() - 1;
	//int y = event->y();
	m_hierNav->inputButtonPress(button, x, y);
}

void HierNavDisplay::mouseMoveEvent(QMouseEvent *event) {
	int x = event->x();
	int y = this->height() - event->y() - 1;
	//int y = event->y();
	m_hierNav->inputMouseMotion(x, y);
}
