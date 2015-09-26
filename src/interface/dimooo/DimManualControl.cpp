/*
 * DimManualControl.cpp
 *
 *  Created on: Sep 29, 2009
 *      Ported by Zaixian Xie
 */

#include "interface/dimooo/DimManualControl.h"

#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QStringList>

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/DimOOOMap.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Transformation.h"
#include "operator/dimooo/DimOOOOperator.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "interface/dimooo/DimOOOCheckBox.h"
#include "interface/dimooo/DimOOOButton.h"
#include "interface/dimooo/DimControl.h"

DimManualControl::DimManualControl(XmdvToolMainWnd *mainWnd, PipelineManager* pm, int pID, ViewWindow* win)
	: QDialog(mainWnd) {
	setWindowTitle( tr("Dimension On/Off/Reorder") );
	m_mainWnd = mainWnd;
	m_pipelineManager = pm;
	m_pipelineID = pID;
	m_dimOOOMap = DimControl::getDimOOOMap( pm->getPipeline(pID) );
	m_viewWindow = win;

	// Get the pointer to the pipeline to get the current dimension  map
	Pipeline* pl = pm->getPipeline(pID);

	// Save the dimension names to array m_dim_names;
	DimControl::getDimNames(pl, m_dim_names);
	// Setup the widgets
	setupGui(pl);
	this->resize(450,500);
}

DimManualControl::~DimManualControl() {
}

void DimManualControl::setupGui(Pipeline* pl) {
	// The input of dimension operator.
	// Actually, we are do dimension on/off/reordering on this data
	OkcData* inputOkc = DimControl::getInputOkcData(pl);

	// Get how many dimensions
	int dim_size = inputOkc->getDimSize();
	std::vector<bool> dim_on;
	std::vector<int> map_out_to_in;
	m_dimOOOMap->getOn(dim_on);
	m_dimOOOMap->getMapOutToIn(map_out_to_in);

	mainLayout = new QVBoxLayout();
	tableWidget = new QTableWidget(dim_size, 4, this);
	int i;
	//checkBoxList.clear();
	for (i=0; i<dim_size; i++) {
		char * dim_name = m_dim_names[map_out_to_in[i]];
		//char * source_name = inputOkc->names[i];
		QLabel *label_name = new QLabel(dim_name, this);
		DimOOOCheckBox *check_onoff = new DimOOOCheckBox(i, this);
		if (dim_on[map_out_to_in[i]]) {
			//if (1) {
			check_onoff->setCheckState ( Qt::Checked );
		} else {
			check_onoff->setCheckState ( Qt::Unchecked );
		}
		//checkBoxList.push_back(check_onoff);
		DimOOOButton *button_back = new DimOOOButton(i, DimOOOMap::BACKWARD, tr("<-"), this);
		DimOOOButton *button_forward = new DimOOOButton(i, DimOOOMap::FORWARD, tr("->"), this);

		tableWidget->setCellWidget(i, 0, label_name);
		tableWidget->setCellWidget(i, 1, check_onoff);
		tableWidget->setCellWidget(i, 2, button_back);
		tableWidget->setCellWidget(i, 3, button_forward);

		connect(check_onoff, SIGNAL( stateChanged(int, int) ),
				this, SLOT( toggleDim(int, int)) );
		connect(button_back, SIGNAL( clicked(int, int) ),
				this, SLOT( reorderDimension(int, int)) );
		connect(button_forward, SIGNAL( clicked(int, int) ),
				this, SLOT( reorderDimension(int, int)) );
	}

	// headerTitle is the list of column captions
	QStringList headerTitle;
	// Initialize the title strings and set it to TableWidget header.
	headerTitle << "Fieldname" << "On" << "Backward" << "Forward";
	tableWidget->setHorizontalHeaderLabels(headerTitle);

	mainLayout->addWidget(tableWidget);
	this->setLayout(mainLayout);

	tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
}

void DimManualControl::toggleDim( int rowID, int state ) {
	if (state==Qt::Checked) {
		m_dimOOOMap->setDimOnOff(rowID, true);
	} else if (state==Qt::Unchecked) {
		m_dimOOOMap->setDimOnOff(rowID, false);
	}
	DimControl::updateDisplay(m_mainWnd, m_pipelineManager, m_pipelineID, m_viewWindow);
}

void DimManualControl::reorderDimension( int rowID, int direction ) {
	std::vector<bool> dim_on;
	m_dimOOOMap->getOn(dim_on);

	m_dimOOOMap->reorderDimension(rowID, (DimOOOMap::DIRECTION)direction );

	updateDimNames();
	DimControl::updateDisplay(m_mainWnd, m_pipelineManager, m_pipelineID, m_viewWindow);
}

void DimManualControl::updateDimNames() {
	Pipeline* pl = m_pipelineManager->getPipeline(m_pipelineID);
	OkcData* inputOkc = DimControl::getInputOkcData(pl);
	// Get how many dimensions
	int i, dim_size = inputOkc->getDimSize();

	// The array map_out_to_in can provide the order of one row
	// starting from its position in the view
	std::vector<int> map_out_to_in;
	std::vector<bool> dim_on;

	m_dimOOOMap->getMapOutToIn(map_out_to_in);
	m_dimOOOMap->getOn(dim_on);

	for (i=0; i<dim_size; i++) {
		//
		char * dim_name = m_dim_names[map_out_to_in[i]];
		QLabel* label = dynamic_cast<QLabel*>
			( tableWidget->cellWidget(i, 0) );
		label->setText(dim_name);

		DimOOOCheckBox* check_onoff = dynamic_cast<DimOOOCheckBox*>
			( tableWidget->cellWidget(i, 1) );

		if ( dim_on[map_out_to_in[i]] == true) {
			check_onoff->setCheckState ( Qt::Checked );
		} else {
			check_onoff->setCheckState ( Qt::Unchecked );
		}
	}

	//TODO: The below code m_dimOOOMap->setOn(dim_on) should be able to removed
	//because any action in the above for loop does not impact the array "on".
	//However, an existing STRANGE bug indicated that the array "on" will be changed
	//based on the check status of checkboxes.  Since the checkboxes are in the
	//view order and the array "on" is in the original order, so the final view
	//is a mess.  After one day hard work, I gave up looking for the real reason
	//of this bug and use the following code to restore the array "on".  If you
	//want to fight with this bug, please remove the below code and try the dialog
	//"Manual Dimension on/off/reorder".
	m_dimOOOMap->setOn(dim_on);
}

