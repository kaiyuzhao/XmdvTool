/*
 * DimAutoReorderControl.cpp
 *
 *  Created on: Nov 8, 2009
 *      Ported by Zaixian xie
 */

#include "interface/dimooo/DimAutoReorderControl.h"

#include <math.h>
#include <assert.h>

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/DimOOOMap.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Transformation.h"
#include "operator/dimooo/DimOOOOperator.h"
#include "view/GLDisplay.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/ViewDisplay.h"
#include "view/multidim/OkcViewDisplay.h"
#include "view/multidim/VisDimstack.h"
#include "interface/dimooo/DimControl.h"

DimAutoReorderControl::DimAutoReorderControl(XmdvToolMainWnd *mainWnd, PipelineManager* pm, int pID, ViewWindow* win)
	: QDialog(mainWnd) {
	setWindowTitle( tr("Dimension Reorder") );
	m_mainWnd = mainWnd;
	m_pipelineManager = pm;
	m_pipelineID = pID;
	m_dimOOOMap = getDimOOOMap();
	m_viewWindow = win;

	// create the widgets
	createWidgets();
	// Enable or disable widgets in terms of users' selection
	setupWidgets();
}

DimAutoReorderControl::~DimAutoReorderControl() {
}

DimOOOMap* DimAutoReorderControl::getDimOOOMap() {
	return getDimOOOOperator()->getDimOOOMap();
}

DimOOOOperator* DimAutoReorderControl::getDimOOOOperator() {
	DimOOOOperator* op = 0;

	Pipeline* pl = m_pipelineManager->getPipeline(m_pipelineID);
	switch (pl->getPipelineType()) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		// The normal pipeline type for flat display
		op = dynamic_cast<DimOOOOperator*>
			( pl->getMainTransformation()->getOperator(PL_FN_OP_DIMOOO) );
		break;
	case XmdvTool::PLTYPE_SBB :
		op = dynamic_cast<DimOOOOperator*>
			( pl->getMainTransformation()->getOperator(PL_SBB_OP_DIMOOO) );
		break;
	// The pipeline type to support dimension reduction (InterRing)
	case XmdvTool::PLTYPE_DIMR :
		// TODO: This should be enabled after we added dimension on/off/reorder
		// operator in the dimension reduction pipeline
		break;
	}

	return op;
}

void DimAutoReorderControl::createWidgets() {
	// Create the layout and radio button to allow users to
	// choose order method: similarity-base and clutter-based.
	orderMethodGroupBox = new QGroupBox();
	orderMethodLayout = new QHBoxLayout();
	SimilarityRadio = new QRadioButton("Similarity-Based");
	ClutterRadio = new QRadioButton("Clutter-Based");
	SimilarityRadio->setChecked(true);
	orderMethodLayout->addWidget(SimilarityRadio);
	orderMethodLayout->addWidget(ClutterRadio);
	orderMethodGroupBox->setLayout(orderMethodLayout);
	orderMethodGroupBox->setFlat(true);


	// Create the layout and radio button to allow users to
	// choose order algorithms
	orderAlgGroupBox = new QGroupBox();
	orderAlgLayout = new QGridLayout();
	OptimalRadio = new QRadioButton("Optimal Ordering");
	RandomRadio = new QRadioButton("Random Swapping");
	NearestRadio = new QRadioButton("Nearest Neighbor");
	GreedyRadio = new QRadioButton("Greedy Algorithm");
	OptimalRadio->setChecked(true);
	// Add these four radio boxes into the layout
	orderAlgLayout->addWidget(OptimalRadio, 0, 0, Qt::AlignLeft);
	orderAlgLayout->addWidget(RandomRadio, 0, 1, Qt::AlignLeft);
	orderAlgLayout->addWidget(NearestRadio, 1, 0, Qt::AlignLeft);
	orderAlgLayout->addWidget(GreedyRadio, 1, 1, Qt::AlignLeft);
	// Set the above layout for the groupbox
	orderAlgGroupBox->setLayout(orderAlgLayout);

	// Create the GroupBox and layout to list the starting dimension
	dimListGroupBox = new QGroupBox();
	dimListLayout = new QVBoxLayout();
	dimListLabel = new QLabel(tr("Starting Dimension"));
	dimList = new QListWidget();
	dimListLayout->addWidget(dimListLabel);
	dimListLayout->addWidget(dimList);
	dimListGroupBox->setLayout(dimListLayout);

	// Create the GroupBox and layout to allow users to
	// choose the method to measure the correlation from
	// the options: "Euclidean Distance", "Pearson's Correlation"
	// and "Cosine Angle Correlation".
	correlationGroupBox = new QGroupBox();
	correlationLayout = new QVBoxLayout();
	correlationLabel = new QLabel("Similarity measure");
	EuclideanRadio = new QRadioButton("Euclidean Distance");
	PearsonRadio = new QRadioButton("Pearson's Correlation");
	CosineRadio = new QRadioButton("Cosine Angle Correlation");
	EuclideanRadio->setChecked(true);
	correlationLayout->addWidget(correlationLabel);
	correlationLayout->addWidget(EuclideanRadio);
	correlationLayout->addWidget(PearsonRadio);
	correlationLayout->addWidget(CosineRadio);
	correlationGroupBox->setLayout(correlationLayout);

	// Create the GroupBox and layout to allow users to
	// choose the target for dimension distance under
	// "Similarity-based" mode.
	SimMinMaxGroupBox = new QGroupBox();
	SimMinMaxLayout = new QVBoxLayout();
	SimMinRadio = new QRadioButton("Minimize Dimension Distance");
	SimMaxRadio = new QRadioButton("Maximize Dimension Distance");
	SimMinRadio->setChecked(true);
	SimMinMaxLayout->addWidget(SimMinRadio);
	SimMinMaxLayout->addWidget(SimMaxRadio);
	SimMinMaxGroupBox->setLayout(SimMinMaxLayout);

	// Create the GroupBox and Layout for threshold slider
	//
	// Create the outer group box
	CluThresholdGroupBox = new QGroupBox();
	CluThresholdLayout = new QGridLayout();
	// Create the first inner group boxes
	CluThresholdGroupBox_1 = new QGroupBox();
	CluThresholdLayout_1 = new QHBoxLayout();
	CluThresholdLabel_1 = new QLabel("1.0");
	CluThresholdScale_1 = new QSlider(Qt::Horizontal);
	CluThresholdLayout_1->addWidget(CluThresholdLabel_1);
	CluThresholdLayout_1->addWidget(CluThresholdScale_1);
	CluThresholdGroupBox_1->setLayout(CluThresholdLayout_1);
	// Create the second inner group boxes
	CluThresholdGroupBox_2 = new QGroupBox();
	CluThresholdLayout_2 = new QHBoxLayout();
	CluThresholdLabel_2 = new QLabel("1.0");
	CluThresholdScale_2 = new QSlider(Qt::Horizontal);
	CluThresholdLayout_2->addWidget(CluThresholdLabel_2);
	CluThresholdLayout_2->addWidget(CluThresholdScale_2);
	CluThresholdGroupBox_2->setLayout(CluThresholdLayout_2);
	// Add two labels into the first row of the GridLayout
	CluThresholdLayout->addWidget( new QLabel(tr("Threshold 1")), 0, 0);
	CluThresholdLayout->addWidget( new QLabel(tr("Threshold 2")), 0, 1);
	// Add the two small group box into the outer group box
	CluThresholdLayout->addWidget(CluThresholdGroupBox_1, 1, 0);
	CluThresholdLayout->addWidget(CluThresholdGroupBox_2, 1, 1);
	CluThresholdGroupBox->setLayout(CluThresholdLayout);

	// Create the GroupBox and layout to allow users to
	// choose "4-connected" or "8-connected" for
	// Neighboring bin definition
	NeighborBinGroupBox = new QGroupBox();
	NeighborBinLayout = new QVBoxLayout();
	NeighborBinLabel = new QLabel(tr("Neighboring Bin Definition"));
	FourConnectedRadio = new QRadioButton("4-connected");
	EightConnectedRadio = new QRadioButton("8-connected");
	FourConnectedRadio->setChecked(true);
	NeighborBinLayout->addWidget(NeighborBinLabel);
	NeighborBinLayout->addWidget(FourConnectedRadio);
	NeighborBinLayout->addWidget(EightConnectedRadio);
	NeighborBinGroupBox->setLayout(NeighborBinLayout);

	// Create the GroupBox and layout to allow users to
	// choose "Minimize Clutter" or "Maximize Clutter" under
	// "Clutter-based" mode
	CluMinMaxGroupBox = new QGroupBox();
	CluMinMaxLayout = new QVBoxLayout();
	CluMinRadio = new QRadioButton("Minimize Clutter");
	CluMaxRadio = new QRadioButton("Maximize Clutter");
	CluMinRadio->setChecked(true);
	CluMinMaxLayout->addWidget(CluMinRadio);
	CluMinMaxLayout->addWidget(CluMaxRadio);
	CluMinMaxGroupBox->setLayout(CluMinMaxLayout);

	ApplyButton= new QPushButton(tr("Apply"));

	// Create the main layout
	mainLayout = new QGridLayout();
	mainLayout->addWidget(orderMethodGroupBox, 0, 0, 1, 2);
	mainLayout->addWidget(orderAlgGroupBox, 1, 0, 1, 2);
	mainLayout->addWidget(dimListGroupBox, 2, 0, 1, 2);
	mainLayout->addWidget(correlationGroupBox, 3, 0, 2, 1);
	mainLayout->addWidget(SimMinMaxGroupBox, 5, 0, 1, 1);
	mainLayout->addWidget(CluThresholdGroupBox, 3, 1, 1, 1);
	mainLayout->addWidget(NeighborBinGroupBox, 4, 1, 1, 1);
	mainLayout->addWidget(CluMinMaxGroupBox, 5, 1, 1, 1);
	mainLayout->addWidget(ApplyButton, 6, 0, 1, 2);

	this->setLayout(mainLayout);

	// Connect the slot functions for switching order methods
	connect( SimilarityRadio, SIGNAL( toggled (bool) ), this, SLOT( switchOrderMethod(bool) ) );
	connect( ClutterRadio, SIGNAL( toggled (bool) ), this, SLOT( switchOrderMethod(bool) ) );

	// Connect the slot functions for switching order algorithms
	connect( OptimalRadio, SIGNAL( toggled (bool) ), this, SLOT( switchOrderAlgorithm(bool) ) );
	connect( RandomRadio, SIGNAL( toggled (bool) ), this, SLOT( switchOrderAlgorithm(bool) ) );
	connect( NearestRadio, SIGNAL( toggled (bool) ), this, SLOT( switchOrderAlgorithm(bool) ) );
	connect( GreedyRadio, SIGNAL( toggled (bool) ), this, SLOT( switchOrderAlgorithm(bool) ) );

	connect(CluThresholdScale_1, SIGNAL(valueChanged(int)), this, SLOT(setSlider1Label(int)) );
	connect(CluThresholdScale_2, SIGNAL(valueChanged(int)), this, SLOT(setSlider2Label(int)) );

	connect(ApplyButton, SIGNAL(clicked()), this, SLOT( applyReorder() ) );

}

void DimAutoReorderControl::setupWidgets() {
	switchOrderMethod(true);
	setThresholdScale();
}


void DimAutoReorderControl::setupSimilarityDialog() {
	setOrderAlgorithm(OrderDimension::OA_OPTIMAL);
	NearestRadio->setEnabled(true);
	GreedyRadio->setEnabled(true);
	correlationGroupBox->setEnabled(true);
	SimMinMaxGroupBox->setEnabled(true);
	// Clear the dimension list and disable it
	dimList->clear();
	dimList->setEnabled(false);

	//offClutterDialog();
	CluThresholdGroupBox_1->setEnabled(false);
	CluThresholdGroupBox_2->setEnabled(false);
	// Disable the radio buttons to define the neighboring bin
	NeighborBinGroupBox->setEnabled(false);
	// Disable the radio buttons to choose to minimize or maximize clutter
	CluMinMaxGroupBox->setEnabled(false);
}

void DimAutoReorderControl::setupClutterDialog() {
	//setClutterDialog();
	setOrderAlgorithm(OrderDimension::OA_OPTIMAL);

	// Disable the widgets to choose similarity measure
	correlationGroupBox->setEnabled(false);
	// Disable the widgets to choose to minimize or maximize dimension distance
	SimMinMaxGroupBox->setEnabled(false);

	//enable the threshold 1
	CluThresholdGroupBox_1->setEnabled(true);
	XmdvTool::VISTYPE visType = m_viewWindow->getVisType();
	if ( visType == XmdvTool::VISTYPE_SC || visType == XmdvTool::VISTYPE_GL ) {
		CluThresholdGroupBox_2->setEnabled(true);
	} else {
		CluThresholdGroupBox_2->setEnabled(false);
	}

	if ( visType == XmdvTool::VISTYPE_DI ) {
		NeighborBinGroupBox->setEnabled(true);
	} else {
		NeighborBinGroupBox->setEnabled(false);
	}

	CluMinMaxGroupBox->setEnabled(true);

	if  ( visType == XmdvTool::VISTYPE_PA || visType == XmdvTool::VISTYPE_SC ) {
		NearestRadio->setEnabled(true);
		GreedyRadio->setEnabled(true);
	} else {
		NearestRadio->setEnabled(false);
		GreedyRadio->setEnabled(false);
	}

	setThresholdScale();
	// Clear the dimension list and disable it
	dimList->clear();
	dimList->setEnabled(false);
}

OrderDimension::ORDERMETHOD DimAutoReorderControl::orderMethod(){
	if ( SimilarityRadio->isChecked() ) {
		return OrderDimension::OM_SIMILARITY;
	} else if ( ClutterRadio->isChecked() ) {
		return OrderDimension::OM_CLUTTER;
	} else {
		// The code should never go here
		assert(0);
	}

}

OrderDimension::ORDERALGORITHM DimAutoReorderControl::orderAlgorithm() {
	if ( OptimalRadio->isChecked() ) {
		return OrderDimension::OA_OPTIMAL;
	} else if ( RandomRadio->isChecked() ) {
		return OrderDimension::OA_RANDOM;
	} else if ( NearestRadio->isChecked() ) {
		return OrderDimension::OA_NN;
	} else if ( GreedyRadio->isChecked() ) {
		return OrderDimension::OA_GREEDY;
	} else {
		// The code should never go here
		assert(0);
	}
}

void DimAutoReorderControl::setOrderAlgorithm(OrderDimension::ORDERALGORITHM method) {
	switch (method) {
	case OrderDimension::OA_OPTIMAL :
		OptimalRadio->setChecked(true);
		break;
	case OrderDimension::OA_RANDOM :
		RandomRadio->setChecked(true);
		break;
	case OrderDimension::OA_NN :
		NearestRadio->setChecked(true);
		break;
	case OrderDimension::OA_GREEDY :
		GreedyRadio->setChecked(true);
		break;
	}
}

void DimAutoReorderControl::setThresholdScale() {
	// threshold_1, threshold_2 are the default values for two sliders
	double threshold_1=0.0, threshold_2=0.0;
	XmdvTool::VISTYPE visType= m_viewWindow->getVisType();
	// Set the minimum and maxmum values for two sliders
	if( visType == XmdvTool::VISTYPE_PA )
	{
		m_min_1 = 0;
		m_max_1 = sqrt(1.0+1.0); // maximum normalized euclidean distance possible
		m_min_2 = 0;         // not used
		m_max_2 = 100;       // not used
		threshold_1 = 0.1;
		threshold_2 = 0;       // not used
	}
	else if( visType==XmdvTool::VISTYPE_SC )
	{
		m_min_1 = 0;
		m_max_1 = 2; // maximum pearson correlation difference possible
		m_min_2 = 0;
		m_max_2 = 1./2;
		threshold_1 = 0.5;
		threshold_2 = 0.25;
	}
	else if( visType==XmdvTool::VISTYPE_GL )
	{
		m_min_1 = 0;
		m_max_1 = 1;
		m_min_2 = 0;
		m_max_2 = 1;
		threshold_1 = 0.1;
		threshold_2 = 0.1;
	}
	else if( visType == XmdvTool::VISTYPE_DI )
	{
		std::vector<int> cardinality;
		OkcData* inputOkcData = DimControl::getInputOkcData(m_pipelineManager->getPipeline(m_pipelineID) );
		inputOkcData->getCardinality(cardinality);
		m_min_1 = 1;  // smallest group
		m_max_1 = pow((double)(cardinality[0]),(int)2);
		m_min_2 = 0;       // not used
		m_max_2 = 100;     // not used
		threshold_1 = 2;		// number of aggregated bins in one group
		threshold_2 = 0;		// not used
	}

	// Set the resolution for two sliders
	if( visType == XmdvTool::VISTYPE_DI ) {
		m_resolution_1 = 1.0;
		m_resolution_2 = 1.0;
	} else {
		m_resolution_1 = 0.1;
		m_resolution_2 = 0.1;
	}

	CluThresholdScale_1->setMinimum( (int)(m_min_1/m_resolution_1) );
	CluThresholdScale_1->setMaximum( (int)(m_max_1/m_resolution_1) );
	CluThresholdScale_2->setMinimum( (int)(m_min_2/m_resolution_2) );
	CluThresholdScale_2->setMaximum( (int)(m_max_2/m_resolution_2) );

	// Set the default values for two sliders;
	CluThresholdScale_1->setValue( (int)(threshold_1/m_resolution_1) );
	CluThresholdScale_2->setValue( (int)(threshold_2/m_resolution_2) );
}

// Slot functions
void DimAutoReorderControl::setSlider1Label(int threshold1 ) {
	CluThresholdLabel_1->setText( QString::number(threshold1*m_resolution_1, 'f', 1) );
}

void DimAutoReorderControl::setSlider2Label(int threshold2 ) {
	CluThresholdLabel_2->setText( QString::number(threshold2*m_resolution_2, 'f', 1) );
}

void DimAutoReorderControl::switchOrderMethod(bool) {
	switch ( orderMethod() ) {
	case OrderDimension::OM_SIMILARITY :
		setupSimilarityDialog();
		break;
	case OrderDimension::OM_CLUTTER :
		setupClutterDialog();
		break;
	}
}

void DimAutoReorderControl::switchOrderAlgorithm(bool) {
	switch ( orderAlgorithm() ) {
	case OrderDimension::OA_OPTIMAL : 	// Optimal ordering
	case OrderDimension::OA_RANDOM :	// Random Swapping
	case OrderDimension::OA_GREEDY :	// Greedy Algorithm
		// Clear and disable the dimension list
		clearDimList();
		break;
	case OrderDimension::OA_NN :		// Nearest Neighbor
		// Enable and setup dimension list for Nearest Neighbor
		setDimList();
		break;
	}
}

void DimAutoReorderControl::setDimList() {
	// Enable the dimension list
	dimList->setEnabled(true);

	// all_dim_names is the list for all dimensions.
	// We remove the invisible dimensions from all_dim_names
	// and put the result in dim_names
	std::vector<char*> all_dim_names, dim_names;
	Pipeline* pl = m_pipelineManager->getPipeline(m_pipelineID);
	// Save the dimension names to array all_dim_names;
	DimControl::getDimNames(pl, all_dim_names);

	// Remove those invisible dimensions
	DimOOOMap* map = DimControl::getDimOOOMap(pl);
	std::vector<bool> on;
	map->getOn(on);
	// dims is the number of dimension after turning off some of them
	int dims = map->getViewDimSize();
	// Initialize the length of dim_names
	dim_names.resize(dims);
	int i, idx = 0;
	// Scan each dimension and assign the attributes for those visible dimensions to Reorder_Parameters
	for (i=0; i<dims; i++) {
		// Scan forward until a visible dimension
		while (!on[idx]) {
			idx++;
		}
		dim_names[i] = all_dim_names[idx];
		idx++;
	}

	// Add the each element in the array dim_names to the dimension list widget
	dimList->clear();
	for (i=0; i<dims; i++) {
		dimList->addItem( dim_names[i] );
	}
	dimList->setCurrentRow(0);
}

void DimAutoReorderControl::clearDimList() {
	// Clear and disable the dimension list
	dimList->clear();
	dimList->setEnabled(false);
}

void DimAutoReorderControl::applyReorder() {
	Reorder_Parameters reorderPara;
	// Set the parameters for reordering
	setReorderParameters(&reorderPara);
	// pl is the pipeline associated with this dialog
	Pipeline* pl = m_pipelineManager->getPipeline(m_pipelineID);
	OkcData* inputOkcData = DimControl::getInputOkcData(pl);
	DimOOOMap* map = DimControl::getDimOOOMap(pl);
	// Call the reordering algorithms for "similarity-based" mode
	OrderDimension::DimensionReorder( inputOkcData, map, &reorderPara,
			orderMethod(), m_viewWindow->getVisType() );
	// Update the display
	std::vector<bool> on;
	std::vector<int> map_in_to_out;
	map->getOn(on);
	map->getMapInToOut(map_in_to_out);
	DimControl::updateDisplay(m_mainWnd, m_pipelineManager, m_pipelineID, m_viewWindow);
}

void DimAutoReorderControl::setReorderParameters(Reorder_Parameters* reorderPara) {
	// Setup the current algorithm
	switch ( orderAlgorithm() ) {
	case OrderDimension::OA_OPTIMAL :
		reorderPara->algorithm = 0;
		break;
	case OrderDimension::OA_RANDOM :
		reorderPara->algorithm = 1;
		break;
	case OrderDimension::OA_NN :
		reorderPara->algorithm = 2;
		break;
	case OrderDimension::OA_GREEDY :
		reorderPara->algorithm = 3;
		break;
	}

	switch ( orderMethod() ) {
	case OrderDimension::OM_SIMILARITY :
		//Setup the current correlation
		if ( EuclideanRadio->isChecked() ) {
			reorderPara->correlation = 1;
		} else if ( PearsonRadio->isChecked() ) {
			reorderPara->correlation = 2;
		} else if ( CosineRadio->isChecked() ) {
			reorderPara->correlation = 3;
		}

		// Setup the current option for maximizing or minimizing dimension distance
		// under "similarity-based" mode or
		if ( SimMinRadio->isChecked() ) {
			reorderPara->min_max = 0;
		} else if ( SimMaxRadio->isChecked() ) {
			reorderPara->min_max = 1;
		}
	case OrderDimension::OM_CLUTTER :
		// Setup the current option
		// for maximizing or minimizing clutter under "clutter-based" mode
		if ( CluMinRadio->isChecked() ) {
			reorderPara->min_max = 0;
		} else if ( CluMaxRadio->isChecked() ) {
			reorderPara->min_max = 1;
		}
		// Setup the threshold values for reorder parameters
		reorderPara->threshold_1 = CluThresholdScale_1->value()*m_resolution_1;
		reorderPara->threshold_2 = CluThresholdScale_2->value()*m_resolution_1;

		// Setup the neighboring bin for reorder parameters
		if ( FourConnectedRadio->isChecked() ) {
			reorderPara->eight_connected = 0;
		} else if ( EightConnectedRadio->isChecked() ) {
			reorderPara->eight_connected = 1;
		}

	}

	// If the current vis type is dimension stacking, set the
	// dimstack_size and block_size in the reorder parameter
	if ( m_viewWindow->getVisType() == XmdvTool::VISTYPE_DI ) {
		// Get the pointer to the VisDimstack
		VisDimstack* visDimStack = dynamic_cast<VisDimstack*>
			( m_viewWindow->getViewDisplay() );
		visDimStack->getDimstack_size(reorderPara->dimstack_size);
		reorderPara->dimstack_X_blocksize = visDimStack->getXBlockSize();
		reorderPara->dimstack_Y_blocksize = visDimStack->getYBlockSize();
	}

	// Get the input of this dimension on/off/reorder operator
	Pipeline* pl = m_pipelineManager->getPipeline(m_pipelineID);
	OkcData* inputOkcData = DimControl::getInputOkcData(pl);
	DimOOOMap* map = DimControl::getDimOOOMap(pl);
	std::vector<bool> on;
	map->getOn(on);
	// dims is the number of dimension after turning off some of them
	int dims = map->getViewDimSize();

	// Initialize the cardinality, dim_min and dim_max arrays in the Reorder_Parameters
	reorderPara->cardinality.resize(dims);
	reorderPara->dim_min.resize(dims);
	reorderPara->dim_max.resize(dims);
	int i, idx = 0;
	// Scan each dimension and assign the attributes for those visible dimensions to Reorder_Parameters
	for (i=0; i<dims; i++) {
		// Scan forward until a visible dimension
		while (!on[idx]) {
			idx++;
		}
		reorderPara->cardinality[i] = inputOkcData->cardinality[idx];
		reorderPara->dim_min[i] = inputOkcData->dim_min[idx];
		reorderPara->dim_max[i] = inputOkcData->dim_max[idx];
		idx++;
	}

	// Get the size of canvas and assign them to reorderPara
	GLDisplay* glDisplay = m_viewWindow->getGLDisplay();
	reorderPara->canvasWidth = glDisplay->size().width();
	reorderPara->canvasHeight = glDisplay->size().height();

	// Set the starting dimension for nearest neighbor algorithm
	reorderPara->nn_start_dim = dimList->currentRow();
}
