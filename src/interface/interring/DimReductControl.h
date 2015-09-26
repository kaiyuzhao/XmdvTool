/*
 * DimReductControl.h
 *
 *  Created on: Sep 22, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class DimReductControl provides a dialog to allow users to
 * do dimension reduction based on a dimension cluster tree.
 * This class uses class DimTreeDisplay to render an interring
 * representing a dimension cluster tree.  Users can interactive with
 * this cluster tree to select a part of dimensions.  The selection
 * result will be written to the current_list of the dimension reduction pipeline
 */
#ifndef DIMREDUCTCONTROL_H_
#define DIMREDUCTCONTROL_H_

#include <QtGui/QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QShowEvent>
#include <QCheckBox>

class XmdvToolMainWnd;
class PipelineManager;
class LinkList;
class InterRingDisplay;
class InterRingDimClusterTree;

class DimReductControl : public QDialog
{
    Q_OBJECT

    // Set InterRingDisplay as the friend class of this class
    // to enable it to access m_show_selected_name
    friend class InterRingDisplay;

public:
	DimReductControl(XmdvToolMainWnd *mainWnd, PipelineManager* pm, int pID);
	virtual ~DimReductControl();

	//
	// All widgets
	//
private:
	QVBoxLayout* mainLayout;

	// The button "deselect all"
	QPushButton* button_deselectAll;
	// The button "Apply"
	QPushButton* button_apply;
	// The check box to control whether to show selected cluster name
	QCheckBox *check_showName;
	// The label above the dimension cluster tree to show the hint
	QLabel* labelHint;
	// The scroll area to hold the tree contour
	QScrollArea* scrollArea;
	// The label below the dimension cluster tree
	QLabel* labelCaption;

	// The layout to hold buttons
	QHBoxLayout* layout_buttons;


private:
    // The pointer to the main window
    XmdvToolMainWnd* m_mainWnd;
    // The pointer to the pipeline manager
    PipelineManager* m_pipelineManager;
    // The pipeline ID linked to this dialog
	int m_pipelineID;

	// The pointer to the dimension cluster tree
	InterRingDimClusterTree* m_dimTree;
    // The pointer to the current dimension list chosen by the user.
	// The constructor function will obtain this pointer from the pipeline
	LinkList* m_current_list;
	// The QGLWidget to render the dimension cluster tree
	InterRingDisplay* m_interRingDisplay;
    // Whether to show cluster name for selected dimensions
    bool m_show_selected_name;

public:
	// setup the widgets and layout for the dialog
	void setupGui();
	// Show text in status bar, which actually is a label
	void updateStatusBar(char* info);

	// Reassemble the pipeline from the DimReductViewOperator.
	// This function will be called when users change the current dimension list
	void ReassemblePipeline();
	// Update all views associated with the current pipeline.
	void updateVisDisplay();


private slots:
	// Apply users' change on selected dimension list
	void applyChange();
	// Remove all selected dimensions in the cluster tree
	void removeAllDimensions();
	// This function will be invoked when users click the check box
	// to control whether to show selected cluster name
	void showNameCheckChanged(int state);
};

#endif /* DIMREDUCTCONTROL_H_ */
