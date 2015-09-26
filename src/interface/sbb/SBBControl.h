/*
 * SBBControl.h
 *
 *  Created on: Aug 11, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class SBBControl provides a dialog to allow users to
 * change the parameters in structure-based brush,
 * such as non-brushed cluster radius, brushed cluster radius,
 * left handle position, right handle position,
 * whether to show band, dynamic masking and extent scaling.
 * This dialog also draws a contour to represent the cluster tree,
 * and a line on the contour to denote the current selected level.
 * Users can drag this line to change the selected level.
 *
 */

#ifndef SBBCONTROL_H_
#define SBBCONTROL_H_

#include <QtGui/QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QGLWidget>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>

class HierDisplayInformation;
class XmdvToolMainWnd;
class PipelineManager;
class HierNavDisplay;
class HierNav;
class SBBPipeline;

class SBBControl : public QDialog
{
    Q_OBJECT

public:
	SBBControl(XmdvToolMainWnd *mainWnd, PipelineManager* pm, int pID);
	virtual ~SBBControl();

private:
	// This is a pointer to the object in the pipeline to represent the
	// configuration of the hierarchical display.
	// All operations on this dialog will change the parameters in this dialog
	HierDisplayInformation* m_hier_info;

	// The rendering area to render the tree contour
	HierNavDisplay*  m_hierNavDisplay;
	// The helper object to draw the tree contour on GLWidget
	HierNav* m_hierNav;

private:
	QVBoxLayout* mainLayout;

	// The scroll area to hold the tree contour
	QScrollArea* scrollArea;

	// The label to show the what users can do while moving
	// the mouse to a specific location
	QLabel *labelCaption;

	// The layout for sliders below the tree contour
	QGroupBox* lowerGroupBox;
	QHBoxLayout* lowerLayout;
	// The left and right layout to hold sliders
	QGroupBox* lowerLGroupBox;
	QVBoxLayout* lowerLLayout;
	QGroupBox* lowerRGroupBox;
	QVBoxLayout* lowerRLayout;

	QLabel *label1;
	QLabel *label2;
	QLabel *label3;
	QLabel *label4;
	QLabel *label5;
	QLabel *label6;

	// The slider for fade factor
	QSlider	*slider_fade_factor;
	QLabel *labelFadeFactor;
	// The slider for non-brushed cluster radius
	QSlider *slider_nbradius;
	QLabel *labelNBRadius;
	// The slider for brushed cluster radius
	QSlider *slider_bradius;
	QLabel *labelBRadius;
	// The check box to choose whether to show bands
	QCheckBox *check_showBands;
	// The slider for extent scaling factor
	QSlider *slider_extent;
	QLabel *labelExtent;

	// The input box for left handle position
	QLineEdit *line_lHandlePos;
	// The input box for right handle position
	QLineEdit *line_rHandlePos;

private:
    // The pointer to the main window
    XmdvToolMainWnd* m_mainWnd;
    // The pointer to the pipeline manager
    PipelineManager* m_pipelineManager;
    // The pipeline ID linked to this dialog
	int m_pipelineID;

public:
	// set the pointer to the XmdvTool main window
	void setMainWnd(XmdvToolMainWnd* mainWnd);
	// set the pipeline corresponding to this dialog
	void setPipelineID(int pID);
	// setup the widgets and layout for the dialog
	void setupGui(SBBPipeline* pl);

private slots:
	// set the label for the fade factor
	void setFadeFactorLabel(int fade_factor);
	// update fade_factor in hier_info
	void updateFadeFactor(int fade_factor);

	// set the label for non-brushed cluster radius
	void setNBRadiusLabel(int nbradius);
	// update the cluster radius in hier_info
	void updateClusterRadius(int nbradius);

	// set the label for brushed cluster radius
	void setBRadiusLabel(int bradius);
	// update the brushed radius in hier_info
	void updateBrushedRadius(int bradius);

	// set the label for extent scaling factor
	void setExtentLabel(int extent);
	// update extent_scaling in hier_info
	void updateExtentScaling(int extent);

	// update the handle position (tree_brush[0..1]) in hier_info
	void updateHandlePosition();

	// update the view when users changed the states whether to show bands
	void updateShowBands(int state);

public:
	// Update the display, including cluster tree, sliders, text boxes,
	// and hier display
	void updateDisplay(int changeFlag);

	// Re-assemble the SBB pipeline after the brush parameters changed
	// The parameter changeFlag denotes what are changed in hier_info
	// It can be DIRTY_CLUSTER_RADIUS, DIRTY_BRUSHED_RADIUS, or DIRTY_HANDLE_POSITION
	void ReassemblePipeline(int changeFlag);
	// Update the hierarchical display
	void updateHierDisplay();

	// Update all of widgets, including slider and text boxes
	// This will be called when users modified sbb parametere
	// via the interaction with cluster tree
	void updateWidgets();
	// Show the caption below the cluster tree display
	void showCaption(QString caption);
};

#endif /* SBBCONTROL_H_ */
