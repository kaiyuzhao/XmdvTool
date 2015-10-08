/*
 * MdiSubWindow.h
 *
 *  Created on: Oct 5, 2009
 *      Author: zyguo
 */

#ifndef MDISUBWINDOW_H_
#define MDISUBWINDOW_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QMdiSubWindow>
#include <QtOpenGL/QGLWidget>
#include <QScrollArea>
#include <view/GLDisplay.h>

class ViewManager;

class MdiSubWindow: public QMdiSubWindow {
	Q_OBJECT
public:
	MdiSubWindow(QWidget* parent, ViewManager* vm);
	virtual ~MdiSubWindow();
	void setGLDisplay(GLDisplay* glDisplay);
protected:
	void resizeEvent(QResizeEvent *event);
	// In this function, the instance of ViewWindow associated with
	// this sub window will be deleted.
	void closeEvent (QCloseEvent * event);

private:
	ViewManager* m_viewManager;
	GLDisplay* glDisplay;
};

#endif /* MDISUBWINDOW_H_ */
