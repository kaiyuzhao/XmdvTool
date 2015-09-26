/*
 * GLDisplay.h
 *
 *  Created on: Apr. 1, 2008
 *      Author: Zaixian Xie
 */

/*
 * class GLDisplay is a derived class of QGWidget.
 * It has two functions:
 * (1) rewrites the functions initializeGL(), resizeGL() and paintGL()
 * (2) records MouseKeyboardStatus
 */

#ifndef GLDISPLAY_H_
#define GLDISPLAY_H_

#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>

#include "main/XmdvToolMainWnd.h"

class XmdvToolMainWnd;
class GLCanvas;
class MouseKeyboardStatus;
class ViewWindow;
class ViewDisplay;

class GLDisplay: public QGLWidget
{
	Q_OBJECT

public:
	GLDisplay(QWidget *parent, ViewWindow *viewWindow);
	virtual ~GLDisplay();

protected:
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

public:
	void setCursorShape(Qt::CursorShape cursorShape);

private:
	// m_viewWindow is the object to wrap this class
	ViewWindow *m_viewWindow;
	QPointF m_mousePress;
	// m_mouseKeyboard is used to record the status of mouse and keyboard
	MouseKeyboardStatus* m_mouseKeyboardStatus;

	QScrollArea* scrollArea;
	double zoomFactorX, zoomFactorY;

	int zoomDirection;

protected:
	//Three virtual functions defined by QGLWidget
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	GLCanvas* getGLCanvas();

	int getMouseButton(QMouseEvent *e);

public:
	MouseKeyboardStatus* getMouseKeyboardStatus();
	ViewWindow* getViewWindow();
	ViewDisplay* getViewDisplay();
	QScrollArea* getScrollArea();
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	double getZoomFactorX();
	double getZoomFactorY();
	void setZoomFactorX(double _zoomFactorX);
	void setZoomFactorY(double _zoomFactorY);

	void resizeOnWindow();
	void zoomIn();
	void zoomOut();
	void zoomReset();

	int getZoomDirection();


	void setZoomDirection(int _zoomDirection);
};

#endif /*GLDISPLAY_H_*/
