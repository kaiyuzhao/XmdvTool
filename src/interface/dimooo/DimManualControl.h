/*
 * DimManualControl.h
 *
 *  Created on: Sep 29, 2009
 *      Ported by Zaixian Xie
 */

/* Class DimManualControl provides a dialog to allow users to manually turn on/off
 * or reorder dimensions.
 *
 */
#ifndef DIMMANUALCONTROL_H_
#define DIMMANUALCONTROL_H_

#include <QDialog>
#include <QVBoxLayout>
#include <QTableWidget>

#include <vector>
using namespace std;

class XmdvToolMainWnd;
class PipelineManager;
class Pipeline;
class DimOOOOperator;
class DimOOOMap;
class OkcData;
class DimOOOCheckBox;
class ViewWindow;

class DimManualControl : public QDialog
{
    Q_OBJECT

public:
	DimManualControl(XmdvToolMainWnd *mainWnd, PipelineManager* pm, int pID, ViewWindow* win);
	virtual ~DimManualControl();

private:
    // The pointer to the main window
    XmdvToolMainWnd* m_mainWnd;
    // The pointer to the pipeline manager
    PipelineManager* m_pipelineManager;
    // The pipeline ID linked to this dialog
	int m_pipelineID;
	// The ViewWindows linked to this dialog
	ViewWindow* m_viewWindow;

	// This describes how the dimensions are turned on/off and reordered.
	// This pointer points to the data member m_dimOOOMap in DimOOOOperator
	DimOOOMap* m_dimOOOMap;
	// The string array to represent the dimension names
	std::vector<char*> m_dim_names;

private:
	// The list of all widgets
	QVBoxLayout* mainLayout;
	QTableWidget* tableWidget;

private:
	// Initialize the interface
	void setupGui(Pipeline* pl);

private slots:
	void toggleDim( int rowID, int state );
	void reorderDimension( int rowID, int direction );

public:
	// Update the dimension name label after users reorder the dimensions
	void updateDimNames();
};

#endif /* DIMMANUALCONTROL_H_ */
