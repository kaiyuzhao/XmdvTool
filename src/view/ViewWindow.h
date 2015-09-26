/*
 * ViewWindow.h
 *
 *  Created on: Jan 21, 2009
 *      Author: Zaixian Xie
 */

/*
 * class ViewWindow manages the interface element of a
 * window corresponding to an instance of class ViewDisplay.
 * Note that ViewDiaply is responsible for rendering, which
 * is the core of view section.  the instance of ViewWindow
 * should be created after instances of QMdiSubWindow and
 * ViewDisplay
 *
 */

#ifndef VIEWWINDOW_H_
#define VIEWWINDOW_H_

#include <QMdiSubWindow>
#include <QGLWidget>
#include <QString>
#include "main/XmdvTool.h"

class ViewDisplay;
class GLDisplay;
class PipelineManager;
class ViewManager;
class Pipeline;
class Data;

class ViewWindow {
public:
	ViewWindow();
	//ViewWindow(QMdiSubWindow* subWindow, QString title);
	virtual ~ViewWindow();

private:
	// The pointer to the pipeline manager
	PipelineManager* m_pipelineManager;
	// The ID of pipeline
	int m_pipelineID;
	// The pointer to the view manager
	ViewManager* m_viewManager;
	// The sub window
	QMdiSubWindow* m_subWindow;
	// The order in which this window was created for the corresponding pipeline
	int m_order;
	// The title of sub window
	QString m_title;
	// The widget to draw
	GLDisplay* m_glDisplay;
	//The current visualization techniques,
	XmdvTool::VISTYPE m_visType;
	// The index of this window in the vector m_viewWindows.
	int m_viewWinNo;

	ViewDisplay* m_viewDisplay;

	// The following three variables represent the ID of a specific mode pipeline
	// having been or being associated with this view. Their initial values are -1.
	// If this view is associated with one pipeline, e.g., a flat mode pipeline,
	// m_FlatPipelineID will be the ID of that pipeline.  If users switch to
	// SBB mode and then want to be back to flat mode, we do not need to recreate a
	// new flat mode pipeline.
	// for flat mode
	int m_FlatPipelineID;
	// for structure-based brush mode
	int m_SBBPipelineID;
	// for dimension reduction mode
	int m_DimRPipelineID;

public:
	// Create the view name for a window in terms of the order.
	// This will be used in window title and tree widget
	static QString createViewName(int order);

public:
	// Return a string representation of visualization type
	static QString getVisTypeString(XmdvTool::VISTYPE visType);

	// Return a string list to describe this window
	virtual QStringList toStringList();

	// set the pointer to the pipeline manager
	void setPipelineManager(PipelineManager* pm);
	// get the pointer to the pipeline manager
	PipelineManager* getPipelineManager();

	// set the id of the corresponding pipeline in the pipeline manager
	void setPipelineID(int id);
	// get the id of the corresponding pipeline in the pipeline manager
	int getPipelineID();

	// Return the input data of this window
	Data* getInputData();

	// Set and get pipeline IDs associated with a specific mode
	// for flat mode
	void setFlatPipelineID(int pid);
	int getFlatPipelineID();
	// for structure-based brush mode
	void setSBBPipelineID(int pid);
	int getSBBPipelineID();
	// for dimension reduction mode
	void setDimRPipelineID(int pid);
	int	getDimRPipelineID();

	// get the pointer to the pipeline associated with this window
	Pipeline* getPipeline();

	// set the pointer to the view manager
	void setViewManager(ViewManager* vm);
	// get the pointer to the view manager
	ViewManager* getViewManager();

	// set the pointer the sub window
	void setSubWindow(QMdiSubWindow* subWindow);
	// get the pointer to the sub window
	QMdiSubWindow* getSubWindow();

	// Set the order of this window
	void setOrder(int order);
	// get the order of this window
	int getOrder();

	// set the title of sub window
	void setTitle(QString& title);
	// get the title of sub window
	QString getTitle();

	void setWindowTitle(QString & title);

	// create a new view display based on the current configuration
	void createViewDisplay();
	ViewDisplay* getViewDisplay();

	void setGLDisplay(GLDisplay *glDisplay);
	GLDisplay* getGLDisplay();

	// set the vis type and not refresh the canvas
	void setVisType(XmdvTool::VISTYPE visType);
	// return the current vis type
	XmdvTool::VISTYPE getVisType();

	void setViewWinNo (int viewWinNo);
	int getViewWinNo ();
	// change to a new pipeline and a new vis type
	// and refresh the canvas when necessary
	// return whether the change took place
	// Param:
	//    pID : the pipeline ID
	//    visType: the new vis type
	bool changeVis(int pID, XmdvTool::VISTYPE visType);

	// paint the canvas based on the current settings
	// and the visual mapping result from the pipeline manager.
	// So the developers should set the visual mappinmg result
	// in the pipeline manager
	void updateWindow();
};

#endif /* VIEWWINDOW_H_ */
