#ifndef VIEWMANAGER_H_
#define VIEWMANAGER_H_

#include <string>
#include <vector>
#include <QString>
#include <QGLWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include "main/XmdvTool.h"
using namespace std;

class Data;
class XmdvToolMainWnd;
class ViewWindow;
class PipelineManager;
class MdiSubWindow;

class ViewManager : public QObject
{
	Q_OBJECT


public:
	ViewManager(XmdvToolMainWnd* wnd);
	virtual ~ViewManager();

public:
	bool hasInitialized;

private:
	XmdvToolMainWnd* m_mainWnd;
	QMdiArea* m_mdiArea;

	vector<ViewWindow*> m_viewWindows;
	// the index of active window in the vector m_viewWindows
	int m_activeViewWindowNo;

private:
	// Get all view windows associated with the input data
	vector<ViewWindow*> getViewWindows(Data* origData);

	public:
	void setMainWnd(XmdvToolMainWnd* wnd);
	XmdvToolMainWnd *getMainWnd();

	PipelineManager* getPipelineManager();

	void setMdiArea(QMdiArea* mdiArea);
	QMdiArea* getMdiArea();

	// get the order of active window in the vector m_viewWindow
	int getActiveViewWindowNo();

	// get the currently active window
	ViewWindow *getActiveViewWindow();

	// Return how many view windows have been created.
	int getViewWindowsCount();
	// Return how many view windows are associated with the specific pipeline
	int getViewWindowsCount(int pID);

	// Return the bigger order in all windows associated with a dataset.
	int getMaxWindowOrder(Data* origData);

	// Return the pointer to the ViewWindow with the index specified by the parameter
	ViewWindow* getViewWindow(int index);

	// create a window to draw the pipeline output specified by ID
	ViewWindow* createViewWindow(int pipelineID, QString title, XmdvTool::VISTYPE visType);

	// refresh all display for the pipeline specified by pipeline ID pID
	void refreshDisplayByPID(int pID);

	// refreshes all displays by recreating their VisualMaps
	void refreshAllDisplays();

	// Determine whether an arbitrary pipeline will be output to a specific visualization type
	// it is invoked by Pipeline::willOuputToVis(XmdvTool::VISTYPE visType)
	// It is used for optimization purpose.  For example, if the pipeline is not output
	// to glyphs, the instance of GlyphPlace can be deactivated in visual mapping
	bool willOuputToVis(int pID, XmdvTool::VISTYPE visType);

	// Change to a pipeline with type PLTYPE_SBB and refresh the view
	bool changeToSBBPipeline(int viewWinNo, XmdvTool::VISTYPE visType);
	// Find all windows associated with inputData and set their SBB Pipeline to newID
	void setSBBPipelineID(Data* inputData, int newID);


	// Change to a pipeline with type PLTYPE_DIMR (for dimension reduction) for the current view
	bool changeToDimRPipeline();
	// Change to a pipeline with type PLTYPE_DIMR (for dimension reduction) for a spcified view
	bool changeToDimRPipeline(int viewWinNo, XmdvTool::VISTYPE visType);
	// Find all windows associated with inputData and set their DimR Pipeline to newID
	void setDimRPipelineID(Data* inputData, int newID);

	// Change the vis type of a specified view
	// This function will be called when users clicked any vis button to switch vis type.
	// I will check the associated pipeline type and invoke different functions.
	// Para:
	//   viewWinNo -- the view window no
	//   visType -- the new vis type
	bool changeToVis(int viewWinNo, XmdvTool::VISTYPE visType);

	// Change the current view to a pipeline with type PLTYPE_FlATNORMAL,
	// keep its visualization type and refresh the view
	bool changeToFlatPipeline();
	// Change the specified view to a pipeline with type PLTYPE_FlATNORMA,
	// change its visualization type, and refresh the view
	bool changeToFlatPipeline(int viewWinNo, XmdvTool::VISTYPE visType);

	// Relink ViewWindows from a old pipeline to a new pipeline
	// This is used in switching between SBB pipeline and flat mode pipeline
	//void changeViewLinkToPipeline(int oldLineID, int newLineID);

	// Activate the window specified by the winID ( the index in the vector m_viewWindows )
	void activateView(ViewWindow* viewWin);

	// Close a sub window specified by windows ID.
	// This function can be called if a sub windows need to be closed programatically,
	// e.g., when users select the view item and click the button "delete"
	void closeViewWindow(ViewWindow *viewWin);
	// Close all sub windows associated with an input data
	void closeViewWindows(Data* origData);
	// Release the memory occupied by the instance of ViewWindow associated with the MdiSubWindow
	bool deleteViewWindow(MdiSubWindow* subWin);

	// Save the visualization in the current view to a physical file.
	// fileName: the name of the image file
	void saveImage(QString fileName, QString filter);

public slots:
	// The slot function to change to parallel coordinates
	void changeToParallel();
	// The slot function to change to scatterplot matrices
	void changeToScatter();
	// The slot function to change to glyphs
	void changeToGlyph();
	//the slot function to change to dimensional stacking
	void changeToDimstack();
	// The slot function to change to glyphs
	void changeToPixel();

	// The slot function to change to hierarchical parallel coordinates
	void changeToHierParallel();
	// The slot function to change to hierarchical scatterplot
	void changeToHierScatter();
	// The slot function to change to hierarchical glyph
	void changeToHierGlyph();
	// The slot function to change to hierarchical dimensional stacking
	void changeToHierStacking();
	// The slot function to change to hierarchical pixel-oriented display
	void changeToHierPixel();


	// The slot function to be called after a sub window has been activated
	void mdiSubWindowActivated(QMdiSubWindow* window);

};

#endif /*VIEWMANAGER_H_*/
