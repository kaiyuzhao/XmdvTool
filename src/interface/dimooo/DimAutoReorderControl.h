/*
 * DimAutoReorderControl.h
 *
 *  Created on: Nov 8, 2009
 *      Ported by : Zaixian Xie
 */
/* Class DimManualControl provides a dialog to allow users to use
 * the feature of automatic reordering dimensions to reduce clutter.
 *
 */

#ifndef DIMAUTOREORDERCONTROL_H_
#define DIMAUTOREORDERCONTROL_H_

#include <QDialog>
#include <QRadioButton>
#include <QSlider>
#include <QListWidget>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "util/algo/dimreorder/OrderDimension.h"

class XmdvToolMainWnd;
class PipelineManager;
class DimOOOMap;
class DimOOOOperator;
class ViewWindow;

class DimAutoReorderControl : public QDialog
{
    Q_OBJECT

public:
	DimAutoReorderControl(XmdvToolMainWnd *mainWnd, PipelineManager* pm, int pID, ViewWindow* win);
	virtual ~DimAutoReorderControl();

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

	// All of widgets
	//
private:
	QGridLayout* mainLayout;
	// To allow users to select similarity-base and clutter-based
	QGroupBox* orderMethodGroupBox;
	QHBoxLayout *orderMethodLayout;
	QRadioButton* SimilarityRadio;
	QRadioButton* ClutterRadio;

	// To choose the algorithm from
	// "Optimal Ordering", "Random Swapping", "Nearest Neighbor" and "Greedy Algorithm"
	QGroupBox* orderAlgGroupBox;
	QGridLayout *orderAlgLayout;
	QRadioButton* OptimalRadio;
	QRadioButton* RandomRadio;
	QRadioButton* NearestRadio;
	QRadioButton* GreedyRadio;

	// The list box for starting dimensions
	QGroupBox* dimListGroupBox;
	QVBoxLayout* dimListLayout;
	QLabel* dimListLabel;
	QListWidget* dimList;

	// The radio buttons to choose similarity measure
	QGroupBox* correlationGroupBox;
	QVBoxLayout* correlationLayout;
	QLabel* correlationLabel;
	QRadioButton* EuclideanRadio;
	QRadioButton* PearsonRadio;
	QRadioButton* CosineRadio;

	// The radio button to choose to minimize or maximize dimension distance
	// in "similarity-based" mode
	QGroupBox* SimMinMaxGroupBox;
	QVBoxLayout* SimMinMaxLayout;
	QRadioButton* SimMinRadio;
	QRadioButton* SimMaxRadio;

	// Two sliders to set the threshold
	// The GroupBox to hold two small GroupBoxes.
	QGroupBox* CluThresholdGroupBox;
	QGridLayout* CluThresholdLayout;
	// The first GroupBox to hold threshold slider 1
	QGroupBox* CluThresholdGroupBox_1;
	QHBoxLayout* CluThresholdLayout_1;
	QLabel* CluThresholdLabel_1;
	QSlider* CluThresholdScale_1;
	QGroupBox* CluThresholdGroupBox_2;
	QHBoxLayout* CluThresholdLayout_2;
	QLabel* CluThresholdLabel_2;
	QSlider* CluThresholdScale_2;

	// The radio buttons to define the neighboring bin
	QGroupBox* NeighborBinGroupBox;
	QVBoxLayout* NeighborBinLayout;
	QLabel* NeighborBinLabel;
	QRadioButton* FourConnectedRadio;
	QRadioButton* EightConnectedRadio;

	// The radio buttons to choose to minimize or maximize clutter
	// in "clutter-based" mode
	QGroupBox* CluMinMaxGroupBox;
	QVBoxLayout* CluMinMaxLayout;
	QRadioButton* CluMinRadio;
	QRadioButton* CluMaxRadio;

	QPushButton* ApplyButton;

private:
	// The range for two threshold sliders
	double m_min_1, m_max_1, m_min_2, m_max_2;
	// The resolution for two threshold sliders
	double m_resolution_1, m_resolution_2;

private:
	// Return the pointer to the DimOOOMap associated with this dialog
	DimOOOMap* getDimOOOMap();
	// Return the pointer to the dimension on/off/reorder operator
	// associated with this dialog
	DimOOOOperator* getDimOOOOperator();
	// Create the widgets in this dialog
	void createWidgets();
	// Enable or disable widgets in terms of users selection
	void setupWidgets();

	// Enable or disable widgets when users choose similarity-based order method
	void setupSimilarityDialog();
	// Enable or disable widgets when users choose clutter-based order method
	void setupClutterDialog();
	// Set the minimum, maximum values and resolution for sliders
	void setThresholdScale();
	// Get the current order method chosen by the user
	OrderDimension::ORDERMETHOD orderMethod();
	// Get the current order algorithm chosen by the user
	OrderDimension::ORDERALGORITHM orderAlgorithm();
	// Set the radio buttons based on the parameter.
	void setOrderAlgorithm(OrderDimension::ORDERALGORITHM method);
	// Set the reorder parameters in terms of users' selection
	void setReorderParameters(Reorder_Parameters* reorderPara);
	// Fill the dimList widget with dimension names
	void setDimList();
	// Clear the dimension names in the dimList widget
	void clearDimList();


private slots:
	// set the label for threshold of slider 1
	void setSlider1Label(int threshold1 );
	// set the label for threshold of slider 2
	void setSlider2Label(int threshold2 );
	// The slot function to enable or disable widgets in terms of the current order method
	void switchOrderMethod(bool checked);
	// The slot function to set or clear the dimension list when users switch among
	// order algorithms
	void switchOrderAlgorithm(bool checked);
	// Reorder the dimensions in terms of users' selection
	void applyReorder();
};

#endif /* DIMAUTOREORDERCONTROL_H_ */

