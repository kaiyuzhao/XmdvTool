/*
 * InterRingDisplay.h
 *
 *  Created on: Sep 22, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class InterRingDisplay is a derived class of QGLWidget.
 * It is the widget to show the dimension cluster tree in the
 * dimension reduction dialog.  It rewrites the
 * functions initializeGL(), resizeGL() and paintGL()
 * in QGLWidget.
 */

#ifndef INTERRINGDISPLAY_H_
#define INTERRINGDISPLAY_H_

#include <QGLWidget>
#include <vector>
#include "datatype/Vec2.h"
#include "datatype/LinkList.h"

class InterRingDimClusterTree;
class InterRingCluster;
class GLCanvas;
class LinkList;
class DimReductControl;

class InterRingDisplay : public QGLWidget {
public:
	InterRingDisplay(QWidget *parent);
	virtual ~InterRingDisplay();

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
	void mouseDoubleClickEvent ( QMouseEvent * event );

private:
	// The pointer to the dimension reduction dialog
	DimReductControl* m_dimReductDlg;
	// The pointer to the GLCanvas helper class
	GLCanvas* m_canvas;
	// The pointer to the dimension cluster tree
	InterRingDimClusterTree* m_dimTree;
    // The pointer to the current dimension list chosen by the user.
	// It will be set by dimension reduction dialog
	LinkList* m_current_list;

	std::vector<double> radius;
	Vec2 center;
	double circle_offset;
    double min_radius;


public:
	// Set the pointer to the dimension cluster tree.  This pointer
	// should be copy of the output of the DimTreeOperator
	void setDimTree(InterRingDimClusterTree* dtree);
	// Set the current dimension list
	void setCurrentList(LinkList* current_list);

private:
	// The main function to draw interring, which will be called by paintGL()
	void drawInterRing();
	// Initialize parameters used to draw dimension cluster tree
	void setTreeRelatedParameters();
	// Recursively draw all dimension clusters.
	// The initial parameter is the tree root.
	// This function will traverse the whole dimension cluster tree
	void traceDrawClusters( InterRingCluster *node );
	// Draw a single cluster
	void drawSingleCluster(InterRingCluster *node);

	// Recursively draw cluster outlines of interRing;
	// The initial parameter is the tree root
	// This function will traverse the whole dimension cluster tree
	void traceDrawClusterOutlines( InterRingCluster *node );
	// Draw the outline for a single cluster
	void drawSingleClusterOutline(InterRingCluster *node);

	// Recursively draw cluster names for those selected dimensions;
	// The initial parameter is the tree root
	// This function will traverse the whole dimension cluster tree
	// but draw nothing for unselected cluster
	void traceDrawClusterNames( InterRingCluster *node );
	// Draw cluster name but do nothing if this node is not in the selected dimension list
	void drawSingleClusterName(InterRingCluster *node);

	// Update the status bar of interring dialog based on the current position of mouse.
	// If mouse is on a node, the node name will be shown; otherwise, a default text.
	void interRingCaption(int x, int y);
	// Get the pointer to the node that is in a specified position (x,y)
	InterRingCluster* findNode(double x, double y);
	// Calculate the level and angle for a node based on its position (x,y)
	void getLevelAndAngleAccordingXY(double x, double y, int &level, double &angle);

	// Calculate the position in terms of radius and angle
	Vec2 mapData(double radius, double angle);
};

#endif /* INTERRINGDISPLAY_H_ */
