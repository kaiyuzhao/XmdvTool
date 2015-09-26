/*
 * HierNav.h
 *
 *  Created on: Aug 14, 2009
 *      Ported by Zaixian Xie
 */

#ifndef HIERNAV_H_
#define HIERNAV_H_

/*
 * Class HierNav handles the display  of interaction
 * on the cluster tree display in the structure-based
 * brush dialog
 */

#include <vector>
#include <Qt>

class ClusterTree;
class HierNavDisplay;
class GLCanvas;
class HierDisplayInformation;
class TreeContour;
class HierNavDisplay;
class XmdvToolMainWnd;
class SBBControl;

class HierNav {
public:
	HierNav(XmdvToolMainWnd* mainWnd, SBBControl *sbbControl);
	virtual ~HierNav();

	enum MOUSE_MODE {
	    INACTIVE,
	    LEFT_BRUSH_HANDLE,
	    RIGHT_BRUSH_HANDLE,
	    MIDDLE_BRUSH,
	    INNER_CONTOUR,
	    OUTER_CONTOUR
	};

private:
	// The pointer to the main Window
	XmdvToolMainWnd* m_mainWnd;
	// The pointer to the class SBBControl
	SBBControl* m_sbbControl;
	// The pointer to the GLCanvas helper class
	GLCanvas* m_canvas;

	// The pointer to the hier info in the pipeline
	HierDisplayInformation* m_hier_info;

	// The pointer to he object to represent the tree contour
	TreeContour* m_tree_contour;

	// The previous and current mouse mode
	MOUSE_MODE m_near_mouse_mode;
	MOUSE_MODE m_mouse_mode;
	// The x coordinate clicked by mouse last time
	int m_last_pick_x;

	// When the mouse is pressed, the button will be stored to here,
	// while will be kept until the button is released.
	Qt::MouseButton m_button;

	HierNavDisplay* m_display;

public:
	// set the pointer to the XmdvTool main window
	void setMainWnd(XmdvToolMainWnd* mainWnd);
	// Initialize the object of GLCanvas
	void initDisplay(HierNavDisplay* display);
	// draw the cluster tree
	void paint();

	// Set the pointer to the hier info
	void setHierInfo(HierDisplayInformation* hier_info);
	// Return the pointer to the hier info
	HierDisplayInformation* getHierInfo();

	// Set the pointer to the tree contour
	void setTreeContour(TreeContour* tree_contour);
	// Return the pointer to the tree contour
	TreeContour* getTreeContour();

	// The event processing function to be called when mouse is moving
	void inputMouseMotion(int x, int y);
	// The event processing function to be called when a mouse button is pressed
	void inputButtonPress(Qt::MouseButton button, int x, int y);
	// The event processing function to be called when the mouse button is released
	void inputButtonRelease(Qt::MouseButton button, int x, int y);

private:
	// Draw the background of cluster tree,
	// including inner and outer triangles and color bar
	void drawBG();
	// Draw the tree contour
	void drawTreeContour();
	// Draw a segment of tree contour. This function will be called by drawTreeContour()
	void drawContours(double radius, double L, double R, double ypa, double width=1.0);

};

#endif /* HIERNAV_H_ */
