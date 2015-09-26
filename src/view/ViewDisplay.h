/*
 * ViewDisplay.h
 *
 *  Created on: Apr 1, 2008
 *      Author: Zaixian Xie
 */

/*
 * class ViewDisplay is the base of all classes to do rendering
 */

#ifndef VIEWDISPLAY_H_
#define VIEWDISPLAY_H_

#include <qnamespace.h>

#include "main/XmdvTool.h"

class PipelineManager;
class Pipeline;
class ViewManager;
class GLDisplay;
class GLCanvas;
class VisualMapResult;
class Brush;
class ViewWindow;
class MouseKeyboardStatus;

class ViewDisplay
{
public:
	ViewDisplay(ViewWindow* viewWindow, PipelineManager* pm, int pipelineID, ViewManager* vm);
	virtual ~ViewDisplay();

private:

protected:
	GLDisplay* m_glDisplay;
	ViewWindow* m_viewWindow;
	GLCanvas *m_canvas;

	//The assembled result from the pipeline
	VisualMapResult *m_visualMapResult;
	PipelineManager* m_pipelineManager;
	int m_pipelineID;
	ViewManager* m_viewManager;

public:
	//when this function is invoked, glWidget and canvas both will be set
	//void setGLWidget(QGLWidget *glWidget);
	//QGLWidget* getGLWidget();

	// get the pointer to the instance of class ViewWindow
	// to manage this display
	ViewWindow* getViewWindow();

	void setGLDisplay(GLDisplay *glDisplay);
	GLDisplay* getGLDisplay();

	GLCanvas *getCanvas();
	void setVisualMapResult(VisualMapResult *vmr);
	VisualMapResult* getVisualMapResult();

	// return the pointer to the pipeline manager
	PipelineManager* getPipelineManager();
	// return the pipeline ID for this display
	int getPipelineID();
	// set the pipeline ID for this display
	void setPipelineID(int id);

	// Return the pipeline corresponding to this view
	Pipeline* getPipeline();

	//return the pointer to the view manager
	ViewManager* getViewManager();

	//return the visualization type for this instance
	XmdvTool::VISTYPE getVisType();

	//return whether to continue on with displaying this visualization or not.
	bool render();

	//check if this vis style can be rendered given the dataset
	virtual XmdvTool::CHECK_RENDERABLE_RESULT checkRenderable();

	//called when checkRenderable returns XmdvTool::FAIL_RENDERABLE
	//displayed in a message box to notify the user of why the visualization cannot
	//be displayed.
	virtual QString getRenderErrorMessage();

	//called when checkRenderable returns XmdvTool::WARN_RENDERABLE
	//displayed in a message box that gives the user the choice to either continue
	//with the visualization or cancel
	virtual QString getRenderWarningMessage();

	// Paint the canvas.  The derived class should implement this
	// function to do the specific rendering
	virtual void paint();

	// The functions to response to the mouse action.
	// The derived class should implement these functions
	// if it needs to handle mouse action
	virtual void inputButtonPress(int button, int x, int y);
    virtual void inputButtonRelease(int button, int x, int y);
    virtual void inputMouseMotion(int x, int y);

    // Set cursor shape
    void setCursorShape(Qt::CursorShape cursorShape);

    // get the pointer to the instance of MouseKeyboardStatus
    // corresponding to this display
    MouseKeyboardStatus* getMouseKeyboardStatus();
};

#endif /*VIEWDISPLAY_H_*/
