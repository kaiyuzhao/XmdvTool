/*
 * HierNavDisplay.h
 *
 *  Created on: Aug 14, 2009
 *      Author: Zaixian Xie
 */

/*
 * class HierNavDisplay is a derived class of QGLWidget.
 * It is the widget to show the cluster tree in the
 * structure-based brush dialog.  It rewrites the
 * functions initializeGL(), resizeGL() and paintGL()
 * in QGLWidget.
 */

#ifndef HIERNAVDISPLAY_H_
#define HIERNAVDISPLAY_H_

#include <QGLWidget>
#include <QWidget>

class HierNav;

class HierNavDisplay : public QGLWidget
{
    Q_OBJECT

public:
	HierNavDisplay(QWidget *parent);
	virtual ~HierNavDisplay();

protected:
	//Three virtual functions defined by QGLWidget
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

protected:
	// Overload these functions defined in QWidget
	// to enable the acceptance of mouse event
	void mouseReleaseEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	// HierNav is the class to handle the display and
	// interaction of cluster tree
	HierNav* m_hierNav;

public:
	void setHierNav(HierNav *hierNav);
	HierNav* getHierNav();
};

#endif /* HIERNAVDISPLAY_H_ */
