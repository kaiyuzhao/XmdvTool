/*
 * HierNav.cpp
 *
 *  Created on: Aug 14, 2009
 *      Ported by Zaixian Xie
 */

#include "interface/sbb/HierNav.h"

#include <QGLWidget>
#include <cmath>

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "datatype/RGBt.h"
#include "datatype/Vec2.h"
#include "data/cluster/ClusterTree.h"
#include "data/cluster/TreeContour.h"
#include "interface/sbb/HierNavDisplay.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "view/GLCanvas.h"
#include "color/ColorManager.h"
#include "interface/sbb/SBBControl.h"
#include "interface/sbb/HierNavDisplay.h"

#include <stdio.h>	// for sprintf()

HierNav::HierNav(XmdvToolMainWnd* mainWnd, SBBControl *sbbControl) {
	m_mainWnd = mainWnd;
	m_sbbControl = sbbControl;
	// Initialize the mouse mode
	m_near_mouse_mode = INACTIVE;
	m_mouse_mode = INACTIVE;
}

HierNav::~HierNav() {
}

void HierNav::initDisplay(HierNavDisplay* display) {
	m_display = display;
	m_canvas = new GLCanvas(display);
}

void HierNav::paint() {
	// draw the background of cluster tree, including
	// inner, outer triangles and color bar
	drawBG();
	// draw the tree contour stored in static data members fo
	drawTreeContour();
}

void HierNav::setHierInfo(HierDisplayInformation* hier_info) {
	m_hier_info = hier_info;
}

void HierNav::setTreeContour(TreeContour* tree_contour) {
	m_tree_contour = tree_contour;
}

TreeContour* HierNav::getTreeContour() {
	return m_tree_contour;
}

HierDisplayInformation* HierNav::getHierInfo() {
	return m_hier_info;
}

void HierNav::drawBG() {
    // Draw the brush and brush handles
    //
    glLineWidth(1.0);
    glColor3f((float) .5, (float) .1, (float) .3);
    glBegin(GL_POLYGON);
    glVertex2f(0.5, 0);
    glVertex2f((float) (m_hier_info->tree_brush[0]), (float) (1-SBB_BARHIGHT));
    glVertex2f((float) (m_hier_info->tree_brush[1]), (float) (1-SBB_BARHIGHT));
    glEnd();

    glColor3f((float) .8, (float) .8, (float) .8);
    glPointSize(5.0);
    glBegin(GL_POINTS);
    glVertex2f((float) (m_hier_info->tree_brush[0]), (float) (1-SBB_BARHIGHT));
    glVertex2f((float) (m_hier_info->tree_brush[1]), (float) (1-SBB_BARHIGHT));
    glEnd();
    glPointSize(1.0);


    // Draw the outer triangle
    //
    glColor3f((float) .8, (float) .8, (float) 0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.5, 0);
    glVertex2f((float) (+1), (float) (1-SBB_BARHIGHT));
    glVertex2f((float) (0), (float) (1-SBB_BARHIGHT));
    glEnd();

    // Draw color bar
    //
	ColorManager *cm = g_globals.colorManager;
    glShadeModel(GL_SMOOTH);
    glLineWidth(5.0);
    glBegin(GL_LINE_STRIP);
    int cmap_size = 256;
    double step = 1/double(cmap_size-1);
    int i;
    for (i=0;i<cmap_size;i++)
    {
    	RGBt color = cm->getColor(ColorManager::BRUSH_UNSELECTED, (double)i/cmap_size);
        glColor3d(color.R, color.G, color.B);
        double k = i*step;
        glVertex2f((float) k,1);
    }
    glEnd();
    glShadeModel(GL_FLAT);
}

void HierNav::drawTreeContour() {
    // Draw the contours
    //
    glColor3f((float) .8, (float) .8, (float) .8);
    int i;
    // The scaling factor for vertical position
    double YPARA = (1-SBB_BARHIGHT)/m_tree_contour->max_display_height;
    glLineWidth(1.0);
    for (i=0;i<(int)m_tree_contour->tree_contour.size();i++)
    {
        i=m_tree_contour->tree_contour.size()-1; // only draw the last contour
        glBegin(GL_LINE_STRIP);
        for (int j=0;j<SBB_CONTOUR_RES;j++) {
            double y = m_tree_contour->tree_contour[i][j];
            double x = ( 2*j/(double)(SBB_CONTOUR_RES-1) - 1 ) * y/(double)m_tree_contour->max_display_height;
            glVertex2f((float) (x/2.+0.5), (float) (y * YPARA));
        }
        glEnd();
    }


    //
    // Draw the current contour
    //
	//Just for time being to see if there are any other places
	double r = m_hier_info->root_radius;

	drawContours(r*m_hier_info->cluster_radius, 0.,m_hier_info->tree_brush[0], YPARA, 2.0);
	drawContours(r*m_hier_info->brushed_radius, m_hier_info->tree_brush[0], m_hier_info->tree_brush[1], YPARA, 3.0);
	drawContours(r*m_hier_info->cluster_radius, m_hier_info->tree_brush[1], 1.0, YPARA, 2.0);

	// draw the string to show the tree height
    char str[100];
    sprintf(str,"Tree Depth:%d", m_hier_info->tree_height);
    m_canvas->setForeground(0xffffff);
    Vec2 strPos = m_canvas->mapDataToScreen(Vec2(0, 0.05));
    m_canvas->drawString((int)strPos.X, (int)strPos.Y, str);
}

void HierNav::drawContours(double radius, double L, double R, double ypa, double width)
{
    int i,j;

    //
    // Find the two bounding contour radii
    //
    for (i=0;i<(int)m_tree_contour->contour_radius.size();i++) {
        if (radius > m_tree_contour->contour_radius[i]) break;
    }
    if (i==0) i++;
    else if (i==(int)m_tree_contour->contour_radius.size()) i--;

    //
    // Interpolate between the two contours to
    // get the approximated contour for the current
    // radius. Show colormap along the contour.
    //

    glLineWidth((float) width);
    glBegin(GL_LINE_STRIP);

    // The color manager to help generate the color
    ColorManager *cm = g_globals.colorManager;

    /* SID -- another division by zero */
    double y_interp = 0;
    if (m_tree_contour->contour_radius[i-1] -
				m_tree_contour->contour_radius[i])
	{
		y_interp = (radius - m_tree_contour->contour_radius[i]) /
			(m_tree_contour->contour_radius[i-1]-m_tree_contour->contour_radius[i]);
	}

    double x_gap = 1.0 / (SBB_CONTOUR_RES-1);

    for (j=0;j<SBB_CONTOUR_RES;j++)
    {
        double y_hi = m_tree_contour->tree_contour[i][j];
        double y_lo = m_tree_contour->tree_contour[i-1][j];

        // x_interp is normalized between [0,1]
        double x_interp = j*x_gap;

        // Limit contour drawing to between L and R
        if (x_interp<L) {
            if ((x_interp+x_gap)<L) continue;
            double k = (L-x_interp)/x_gap;

            if (j<SBB_CONTOUR_RES-1) {
                y_hi = (1-k)*(m_tree_contour->tree_contour[i][j]) + k*m_tree_contour->tree_contour[i][j+1];
                y_lo = (1-k)*(m_tree_contour->tree_contour[i-1][j]) + k*m_tree_contour->tree_contour[i-1][j+1];
                x_interp = L;
            }
        }
        else
        if (x_interp>R) {
            if ((x_interp-x_gap)>R) continue;
            double k = (x_interp-R)/x_gap;

            if (j>0) {
                y_hi = (1-k)*(m_tree_contour->tree_contour[i][j]) + k*m_tree_contour->tree_contour[i][j-1];
                y_lo = (1-k)*(m_tree_contour->tree_contour[i-1][j]) + k*m_tree_contour->tree_contour[i-1][j-1];
                x_interp = R;
            }
        }

        double y = (1-y_interp)*y_hi + (y_interp)*y_lo;

        // xx is normalized between [-1,1]
        double xx = 2*x_interp - 1;

        // Now, normalize xx to triangular coordinates
        xx *= y/(double)m_tree_contour->max_display_height;

        if (m_tree_contour->tree_color[i][j] != -1) { x_interp = m_tree_contour->tree_color[i][j]; }
        RGBt color = cm->getColor(ColorManager::BRUSH_UNSELECTED, x_interp);
        glColor3d(color.R, color.G, color.B);

        glVertex2f((float) ((xx + 1) / 2.0), (float) (y * ypa));
    }

    glEnd();
    glLineWidth(1.0);
}


void HierNav::inputMouseMotion(int x, int y)
{
    Vec2 p = m_canvas->mapScreenToData( Vec2(x,y) );

    //
    // Map mouse coordinates to triangular coordinates [-1,1], [0,1]
    //
    double xx = 2 * p.X - 1;
    double yy = p.Y /(1-SBB_BARHIGHT);
    if (yy!=0) xx /= yy;

    //
    // Check if we are clicking on the brush handles
    //
    const float NEARHANDLES = 0.05;
    m_near_mouse_mode = INACTIVE;

    if (m_mouse_mode == INACTIVE) {

        double L = 2*m_hier_info->tree_brush[0]-1;
        double R = 2*m_hier_info->tree_brush[1]-1;

        if (fabs(yy-1)<NEARHANDLES) {
            if ( fabs(xx+1) < NEARHANDLES && fabs(L+1) < NEARHANDLES && fabs(R+1) < NEARHANDLES )
            {
                //canvas->SetCursor("left_side");
            	m_display->setCursor(Qt::SplitHCursor);
            	m_sbbControl->showCaption("Move right brush handle");
                m_near_mouse_mode = RIGHT_BRUSH_HANDLE;
            }
            else if ( xx>L && xx<R ) {
                //canvas->SetCursor("sb_h_double_arrow");
            	m_display->setCursor(Qt::SizeHorCursor);
            	m_sbbControl->showCaption("Shift brushed region");
                m_near_mouse_mode = MIDDLE_BRUSH;
            }
            else if ( fabs(xx-L) < NEARHANDLES ) {
                //canvas->SetCursor("right_side");
            	m_display->setCursor(Qt::SplitHCursor);
            	m_sbbControl->showCaption("Move left brush handle");
                m_near_mouse_mode = LEFT_BRUSH_HANDLE;
            }
            else if ( fabs(xx-R) < NEARHANDLES ) {
                //canvas->SetCursor("left_side");
            	m_display->setCursor(Qt::SplitHCursor);
            	m_sbbControl->showCaption("Move right brush handle");
                m_near_mouse_mode = RIGHT_BRUSH_HANDLE;
            }
        } else {
            if ( xx<-1 || xx>1 || yy<0 || yy>1) {
            	m_sbbControl->showCaption("Inactive area");
                //canvas->SetCursor("cross");
            	m_display->setCursor(Qt::ArrowCursor);
            } else
            if ( xx>L && xx< R ) {
                m_near_mouse_mode = INNER_CONTOUR;
                m_sbbControl->showCaption("Change brushed cluster radius");
                //canvas->SetCursor("sb_v_double_arrow");
                m_display->setCursor(Qt::SizeVerCursor);
            } else {
                m_near_mouse_mode = OUTER_CONTOUR;
                m_sbbControl->showCaption("Change non-brushed cluster radius");
                //canvas->SetCursor("double_arrow");
                m_display->setCursor(Qt::SizeVerCursor);
            }
        }
    }

    // If no mouse button is pressed, return immediately
    if (m_button != Qt::LeftButton) return;

    // Handle the case that users changed the brush handle position
	switch(m_mouse_mode) {
	case LEFT_BRUSH_HANDLE:
		m_hier_info->tree_brush[0] = (xx+1)*.5;
		if (m_hier_info->tree_brush[0] < 0)
			m_hier_info->tree_brush[0] = 0;
		if (m_hier_info->tree_brush[0] > m_hier_info->tree_brush[1])
			m_hier_info->tree_brush[0] = m_hier_info->tree_brush[1];

		m_sbbControl->updateDisplay(DIRTY_HANDLE_POSITION);
		return;

	case RIGHT_BRUSH_HANDLE:
		m_hier_info->tree_brush[1] = (xx+1)*.5;
		if (m_hier_info->tree_brush[1] > 1)
			m_hier_info->tree_brush[1] = 1;
		if (m_hier_info->tree_brush[1] < m_hier_info->tree_brush[0])
			m_hier_info->tree_brush[1] = m_hier_info->tree_brush[0];

		m_sbbControl->updateDisplay(DIRTY_HANDLE_POSITION);
		return;

	case MIDDLE_BRUSH: {

		Vec2 p1 = m_canvas->mapScreenToData( Vec2(m_last_pick_x,0.5) );
		double prev_x = 2*p1.X - 1;
		if (yy!=0) prev_x /= yy;

		double delta_x = (xx - prev_x)/2.0;

		m_hier_info->tree_brush[0] += delta_x;
		m_hier_info->tree_brush[1] += delta_x;

		if (m_hier_info->tree_brush[1]>1) {
			delta_x = m_hier_info->tree_brush[1]-1;
			m_hier_info->tree_brush[0] -= delta_x;
			m_hier_info->tree_brush[1] -= delta_x;
		} else
		if (m_hier_info->tree_brush[0]<0) {
			delta_x = m_hier_info->tree_brush[0];
			m_hier_info->tree_brush[0] -= delta_x;
			m_hier_info->tree_brush[1] -= delta_x;
		}

		m_last_pick_x = x;

		m_sbbControl->updateDisplay(DIRTY_HANDLE_POSITION);
		return;
		}

	default:
		break;
	}

//
    // Reject if picking outside triangle
    //
    if (xx<-1 || xx>1 || yy<0 || yy>1) return;

    //
    // Map to contour coordinates [0,SBB_CONTOUR_RES], [0,max_display_height]
    //
    yy *= m_tree_contour->max_display_height;
    xx = ((xx + 1)/2.0) * (SBB_CONTOUR_RES-1);

    int ixx = (int) xx;
    double ratio = xx-ixx;

    //
    // Find out which contour bounds "yy" at position "xx"
    //
    double cy = 0.0, prev_cy = 0.0;
    int i;
    for (i=0;i<(int)m_tree_contour->tree_contour.size();i++) {
        prev_cy = cy;
        cy = m_tree_contour->tree_contour[i][ixx];
        if (ixx<SBB_CONTOUR_RES-1) cy += ratio*
			(m_tree_contour->tree_contour[i][ixx+1] - m_tree_contour->tree_contour[i][ixx]);
        if (cy > yy) break;
    }

    double radius;
    if (i==(int)m_tree_contour->tree_contour.size()) {
        radius = 0;
    } else if (i==0){
        radius = m_hier_info->root_radius;
    } else{
        ratio = (yy-prev_cy)/(cy-prev_cy);
        radius = (1-ratio)*m_tree_contour->contour_radius[i-1] + (ratio)*m_tree_contour->contour_radius[i];
    }

    double root_radius = m_hier_info->root_radius;
    radius /= root_radius;

	switch(m_mouse_mode)
	{
		case INNER_CONTOUR:
			m_hier_info->brushed_radius = radius;
			m_sbbControl->updateDisplay(DIRTY_BRUSHED_RADIUS);
			break;

		case OUTER_CONTOUR:
			m_hier_info->cluster_radius = radius;
			m_sbbControl->updateDisplay(DIRTY_CLUSTER_RADIUS);
			break;
		default:
			break;
	}


}

void HierNav::inputButtonPress(Qt::MouseButton button, int x, int y)
{
	m_button = button;

    if (m_near_mouse_mode!=INACTIVE) m_mouse_mode = m_near_mouse_mode;
    m_last_pick_x = x;

    inputMouseMotion(x, y);
}

void HierNav::inputButtonRelease(Qt::MouseButton, int x, int y)
{
	m_button = Qt::NoButton;
    m_near_mouse_mode = m_mouse_mode = INACTIVE;
    inputMouseMotion(x, y);
}
