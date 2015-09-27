/*
 * InterRingDisplay.cpp
 *
 *  Created on: Sep 22, 2009
 *      Ported by Zaixian Xie
 */

#include "interface/interring/InterRingDisplay.h"

#include <QMouseEvent>
#include <string.h>

#include "main/XmdvTool.h"
#include "data/dimreduct/InterRingCluster.h"
#include "data/dimreduct/InterRingDimClusterTree.h"
#include "datatype/LinkList.h"
#include "color/ColorManager.h"
#include "view/GLCanvas.h"
#include "interface/interring/DimReductControl.h"

// RAD_DEGREE is used to convert degree to radian
#define RAD_DEGREE 0.0174532925199

InterRingDisplay::InterRingDisplay(QWidget *parent)
	:QGLWidget(parent) {

	// We suppose that the parent widget is the dimension reduction dialog
	m_dimReductDlg = dynamic_cast<DimReductControl*>(parent);

	// create the GLCanvas object
	m_canvas = new GLCanvas(this);
}


InterRingDisplay::~InterRingDisplay() {
	SAFE_DELETE(m_canvas);
}

void InterRingDisplay::initializeGL()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void InterRingDisplay::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho ( (-CANVAS_BORD), (1+CANVAS_BORD), (1+CANVAS_BORD), (-CANVAS_BORD), -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void InterRingDisplay::paintGL()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawInterRing();
    // We can enable mouse tracking only after the rendering is finished.
    // If we enable it earlier, the data member m_dimTree and m_current_list
    // probably has not been initialized, so the function findNode() will fail.
    setMouseTracking(true);
}

void InterRingDisplay::setDimTree(InterRingDimClusterTree* dtree) {
	m_dimTree = dtree;
}

void InterRingDisplay::setCurrentList(LinkList* current_list) {
	m_current_list = current_list;
}


void InterRingDisplay::drawInterRing() {
	setTreeRelatedParameters();

	// Enable smooth shading
    glShadeModel(GL_SMOOTH);
	//draw clusters of interRing;
    traceDrawClusters(m_dimTree->root);

	//draw cluster outlines of interRing;
	traceDrawClusterOutlines(m_dimTree->root);
    //draw names for selected clusters
    if ( m_dimReductDlg->m_show_selected_name )
        traceDrawClusterNames(m_dimTree->root);

	// Disable smooth shading
    glShadeModel(GL_FLAT);

}

//set parameters used for the drawing; to be rewritten by subclasses;
void InterRingDisplay::setTreeRelatedParameters()
{
	center.X = 0.5;
	center.Y = 0.5;
	radius.resize(m_dimTree->max_level+1);
	double radius_delta = 0.5 / (m_dimTree->max_level+1);
	radius[0] = radius_delta;
	for ( int i = 0; i < m_dimTree->max_level; i++ )
	{
		radius[i+1] = radius[i]+radius_delta;
	}

	circle_offset = 0.008/m_dimTree->max_level;
    min_radius = 4*circle_offset;
}

void InterRingDisplay::traceDrawClusters( InterRingCluster *node )
{
	if ( node == NULL )
		return;
    drawSingleCluster(node);
	InterRingCluster *sibling = node->siblings;
	// Recursively call this function itself to draw its sibling
	traceDrawClusters(sibling);

    if ( node->show_children == true )
    {
	    InterRingCluster *child = node->children;
		// Recursively call this function itself to draw the first child
	    traceDrawClusters(child);
    }
}

void InterRingDisplay::drawSingleCluster(InterRingCluster *node)
{
    m_canvas->setForeground(node->color);
	m_canvas->drawPartialDisk( center.X, center.Y, radius[node->level-1], radius[node->level],
					 node->start_angle, node->sweep_angle);
	// If the current dimension list contains this node, highlight it.
    if ( m_current_list->Contain(node) )  {
        m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::BRUSH_SELECTED1));
	    m_canvas->drawPartialDisk( center.X, center.Y, radius[node->level-1]+(radius[node->level]-radius[node->level-1])/3*1,
            radius[node->level-1]+(radius[node->level]-radius[node->level-1])/3*2, node->start_angle, node->sweep_angle);
    }

}

void InterRingDisplay::traceDrawClusterOutlines( InterRingCluster *node )
{
	if ( node == NULL )
		return;

	m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::TEXT));
    drawSingleClusterOutline(node);

	// Recursively call this function itself to draw its sibling
    InterRingCluster *sibling = node->siblings;
	traceDrawClusterOutlines(sibling);

    if ( node->show_children == true )
    {
    	// Recursively call this function itself to draw its first child
	    InterRingCluster *child = node->children;
	    traceDrawClusterOutlines(child);
    }
}

void InterRingDisplay::drawSingleClusterOutline(InterRingCluster *node)
{
	if ( fabs(node->sweep_angle - 360) > XMDV_DBL_EPSILON )
	{
		Vec2 p1 = mapData(radius[node->level-1], node->start_angle);
		Vec2 p2 = mapData(radius[node->level], node->start_angle);
		m_canvas->drawLine((float)p1.X, (float)p1.Y, (float)p2.X, (float)p2.Y);

		p1 = mapData(radius[node->level-1], node->getEndAngle());
		p2 = mapData(radius[node->level], node->getEndAngle());
		m_canvas->drawLine((float)p1.X, (float)p1.Y, (float)p2.X, (float)p2.Y);
	}

	m_canvas->drawArc( center.X, center.Y, radius[node->level-1], node->start_angle, node->sweep_angle);
	m_canvas->drawArc( center.X, center.Y, radius[node->level], node->start_angle, node->sweep_angle);
}


void InterRingDisplay::traceDrawClusterNames( InterRingCluster *node )
{
	if ( node == NULL )
		return;
	// draw the cluster name for the current node
    drawSingleClusterName(node);
	InterRingCluster *sibling = node->siblings;
	// Recursively call this function itself for node's sibling
	traceDrawClusterNames(sibling);

    if ( node->show_children == true )
    {
	    InterRingCluster *child = node->children;
	    // Recursively call this function itself for node's first child
	    traceDrawClusterNames(child);
    }
}

void InterRingDisplay::drawSingleClusterName(InterRingCluster *node)
{
    if ( node->name == NULL ) {
        return;
    }
    if ( !( m_current_list->Contain(node) ) )  {
    	// If the current dimension list does not contain this node, do nothing
        return;
    }
    m_canvas->setForeground(g_globals.colorManager->getColor(ColorManager::TEXT));
    double mid_ang = node->getMidAngle();
    double radi = (radius[node->level-1] + radius[node->level])/2.0;
	Vec2 pos = m_canvas->mapDataToScreen( mapData(radi, mid_ang) );

    m_canvas->drawString((int)pos.X, (int)pos.Y, node->name);
}

Vec2 InterRingDisplay::mapData(double radius, double angle)
{
	angle -= 90;
	angle *= RAD_DEGREE;
	Vec2 p;
	p.X = center.X + radius * cos(angle);
	p.Y = center.Y - radius * sin(angle);

	return p;
}

void InterRingDisplay::mouseReleaseEvent(QMouseEvent *) {

}

void InterRingDisplay::mousePressEvent(QMouseEvent *) {

}

void InterRingDisplay::mouseMoveEvent(QMouseEvent *event) {
	int x = event->x();
	int y = this->height() - event->y() - 1;
    interRingCaption(x, y);
}

void InterRingDisplay::mouseDoubleClickEvent ( QMouseEvent * event ) {
	int x = event->x();
	int y = this->height() - event->y() - 1;

	// Find the node in the current mouse position
	InterRingCluster *node = findNode(x, y);
	if (m_current_list->Contain(node)) {
		// If this node is in the current dimension list, remove it.
		m_current_list->DeleteElement( (void*)node );
	} else {
		// If this node is in the current dimension list, add it into the list.
		m_current_list->AddNodeAtEnd( (void*)node );
	}

	updateGL();
}

void InterRingDisplay::interRingCaption(int x, int y)
{
    InterRingCluster *node = findNode(x, y);
	if ( node == 0 )  {
		// Pass a empty string to show default text
		m_dimReductDlg->updateStatusBar("");
	}  else {
        if ( node->name == 0 ) {
    		m_dimReductDlg->updateStatusBar("");
        } else {
			m_dimReductDlg->updateStatusBar(node->name);
        }
	}
}

InterRingCluster* InterRingDisplay::findNode(double x, double y)
{
	int level;
	double angle;

	getLevelAndAngleAccordingXY(x, y, level, angle);

	if ( level <= 0 )
		return 0;

	return m_dimTree->findNodeAccordingLevelAndAngle(level, angle);
}

void InterRingDisplay::getLevelAndAngleAccordingXY(double x, double y, int &level, double &angle)
{
    Vec2 p = m_canvas->mapScreenToData( Vec2(x,y) );
	double distance = (p - center).norm();

	if ( distance > 0.5 ) {
		level = -1;
		return;
	}

	level = m_dimTree->max_level;
	while ( level > 0 && distance < radius[level-1] ) {
		level--;
	}

	if ( level <= 0 ) {
		return;
	}

	angle = acos((p.X - center.X)/distance);
	if ( ( center.Y - p.Y ) < 0 )
		angle *= -1;

	angle /= RAD_DEGREE;
	angle += 90;

	return;
}
