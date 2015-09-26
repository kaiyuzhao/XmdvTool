/*
 * ScatterplotCustom.cpp
 *
 *  Created on: Jul 27, 2009
 *      Author: zyguo
 */

#include "interface/scatterplot/ScatterplotCustom.h"

#include <typeinfo>

#include "view/ViewDisplay.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Transformation.h"

#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "view/GLDisplay.h"
#include "operator/diagonal/HistogramOperator.h"

ScatterplotCustom::ScatterplotCustom(XmdvToolMainWnd *mainwnd, QMdiSubWindow *window)
: QDialog(window)
{
	//save references to main window, viewManager and pID
	mainWnd = mainwnd;
	viewManager = mainWnd->getViewManager();
	pID = viewManager->getActiveViewWindow()->getPipelineID();

	setWindowTitle(tr("Scatterplot Customization"));

	QVBoxLayout *mainLayout = new QVBoxLayout();

	VisScatter* visScatter = getScatterplotViewDisplay();

	OkcData* okcdata;
	okcdata = visScatter->getDisplayDataset();
	okcdata->getDimNameArr(m_dim_names);

	//----------------------------------------------------------
	QGroupBox *pointShapeSizeGroup = new QGroupBox(tr("Point Shape"));
	QHBoxLayout *pointShapeSizeLayout = new QHBoxLayout();

	QGroupBox *pointSizeGroup = new QGroupBox(tr("Point Size"));

	radio_shape_square = new QRadioButton(tr("Square"));
	radio_shape_square->setChecked(true);
	radio_shape_circle = new QRadioButton(tr("Circle"));

	connect(radio_shape_square, SIGNAL(clicked()), this, SLOT(switchPointType()));
	connect(radio_shape_circle, SIGNAL(clicked()), this, SLOT(switchPointType()));


	pointShapeSizeGroup->setLayout(pointShapeSizeLayout);

	pointShapeSizeLayout->addWidget(radio_shape_square);
	pointShapeSizeLayout->addWidget(radio_shape_circle);



	mainLayout->addWidget(pointShapeSizeGroup);

	QHBoxLayout *pointSizeLayout = new QHBoxLayout();

	//now create the slider
	QSlider *pointSizeSlider = new QSlider();
	pointSizeSlider->setOrientation(Qt::Horizontal);
	pointSizeSlider->setTickInterval(1);
	pointSizeSlider->setMinimum(1);
	pointSizeSlider->setMaximum(20);
	pointSizeSlider->setValue(2);

	QLabel *pointSizeLabel = new QLabel();
	pointSizeLabel->setText(QString::number(pointSizeSlider->value()));

	//add the label to the container
	pointSizeLayout->addWidget(pointSizeLabel);

	//add the slider to the container
	pointSizeLayout->addWidget(pointSizeSlider);

	pointSizeGroup->setLayout(pointSizeLayout);

	pointShapeSizeLayout->addWidget(pointSizeGroup);

	//set up the connection between the slider and the displayed number
	connect(pointSizeSlider, SIGNAL(valueChanged(int)), pointSizeLabel,
			SLOT(setNum(int)));
	connect(pointSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointSize(int)));
	//----------------------------------------------------------

	//----------------------------------------------------------
	QGroupBox *diagonalTypeGroup = new QGroupBox(tr("Diagonal Graph/Plot"));
	QHBoxLayout *diagonalTypeLayout = new QHBoxLayout();

	QGroupBox *histogramSizeGroup = new QGroupBox(tr("Histogram Size"));

	radio_original = new QRadioButton(tr("Original"));
	radio_original->setChecked(true);
	connect(radio_original, SIGNAL(clicked()), this, SLOT(switchDiagDisplayType()));

	radio_histogram = new QRadioButton(tr("Histogram"));
	connect(radio_histogram, SIGNAL(clicked()), this, SLOT(switchDiagDisplayType()));

	radio_one_dimension = new QRadioButton(tr("1-Dimension"));
	connect(radio_one_dimension, SIGNAL(clicked()), this, SLOT(switchDiagDisplayType()));
	radio_two_dimension = new QRadioButton(tr("2-Dimension"));
	connect(radio_two_dimension, SIGNAL(clicked()), this, SLOT(switchDiagDisplayType()));

	diagonalTypeLayout->addWidget(radio_original);
	diagonalTypeLayout->addWidget(radio_histogram);
	diagonalTypeLayout->addWidget(radio_one_dimension);
	diagonalTypeLayout->addWidget(radio_two_dimension);

	diagonalTypeGroup->setLayout(diagonalTypeLayout);
	mainLayout->addWidget(diagonalTypeGroup);

	QHBoxLayout *histogramSizeLayout = new QHBoxLayout();

	//now create the slider
	slider_HistogramSize = new QSlider();
	slider_HistogramSize->setOrientation(Qt::Horizontal);
	slider_HistogramSize->setTickInterval(1);
	slider_HistogramSize->setMinimum(2);
	slider_HistogramSize->setMaximum(50);
	slider_HistogramSize->setValue(17);

	slider_HistogramSize->setEnabled(false);

	QLabel *histogramSizeLabel = new QLabel();
	histogramSizeLabel->setText(QString::number(slider_HistogramSize->value()));

	//add the label to the container
	histogramSizeLayout->addWidget(histogramSizeLabel);

	//add the slider to the container
	histogramSizeLayout->addWidget(slider_HistogramSize);

	histogramSizeGroup->setLayout(histogramSizeLayout);


	diagonalTypeLayout->addWidget(histogramSizeGroup);

	//set up the connection between the slider and the displayed number
	connect(slider_HistogramSize, SIGNAL(valueChanged(int)), histogramSizeLabel, SLOT(setNum(int)));
	connect(slider_HistogramSize, SIGNAL(valueChanged(int)), this, SLOT(setHistoSize(int)));
	//----------------------------------------------------------



	//----------------------------------------------------------
	QGroupBox *OneDOrderGroup = new QGroupBox(tr("1-Dimension Order"));
	QHBoxLayout *OneDOrderLayout = new QHBoxLayout();
	this->radio_one_D_order_original = new QRadioButton(tr("Original Order"));
	//connect(radio_one_D_order_original, SIGNAL(clicked()), this, SLOT(switchOneDOrder()));

	this->radio_one_D_order_data_driven = new QRadioButton(tr("Data Driven"));
	//connect(radio_one_D_order_data_driven, SIGNAL(clicked()), this, SLOT(switchOneDOrder()));

	combo_sort = new QComboBox();

	combo_sort->addItem("Original Order", 0);
	for(int i = 0; i < (int)m_dim_names.size(); i++) {
		combo_sort->addItem(m_dim_names[i], i+1);
	}

	connect(combo_sort, SIGNAL(activated(int)), this, SLOT(switchOneDOrder(int)));

	//OneDOrderLayout->addWidget(radio_one_D_order_original);
		//TwoDCoordinateLayout->addWidget(radio_from_file);
	//OneDOrderLayout->addWidget(radio_one_D_order_data_driven);
	OneDOrderLayout->addWidget(combo_sort);

	OneDOrderGroup->setLayout(OneDOrderLayout);

	mainLayout->addWidget(OneDOrderGroup);
	//----------------------------------------------------------


	//----------------------------------------------------------
	QGroupBox *TwoDCoordinateGroup = new QGroupBox(tr("2-Dimension Coordinates"));
	QHBoxLayout *TwoDCoordinateLayout = new QHBoxLayout();

	radio_Implicit = new QRadioButton(tr("Implicit"));
	connect(radio_Implicit, SIGNAL(clicked()), this, SLOT(switchDiagDisplayType()));
	radio_Implicit->setChecked(true);
	radio_from_file = new QRadioButton(tr("From Data File"));
	radio_from_file->setEnabled(false);
	//connect(radio_order, SIGNAL(clicked()), this, SLOT(switchRadio()));
	radio_data_drive = new QRadioButton(tr("Data Drive"));
	connect(radio_data_drive, SIGNAL(clicked()), this, SLOT(switchDiagDisplayType()));
	radio_derived = new QRadioButton(tr("Derived"));
	connect(radio_derived, SIGNAL(clicked()), this, SLOT(switchDiagDisplayType()));



	TwoDCoordinateLayout->addWidget(radio_Implicit);
	//TwoDCoordinateLayout->addWidget(radio_from_file);
	TwoDCoordinateLayout->addWidget(radio_data_drive);
	TwoDCoordinateLayout->addWidget(radio_derived);

	TwoDCoordinateGroup->setLayout(TwoDCoordinateLayout);

	mainLayout->addWidget(TwoDCoordinateGroup);
	//----------------------------------------------------------

	//----------------------------------------------------------
	QGroupBox *DataDrivenGroup = new QGroupBox(tr("Data Driven Settings"));
	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->setSpacing(6);
	gridLayout->setMargin(11);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	gridLayout->setContentsMargins(0, 0, 0, 0);

	pushButton_selectX = new QPushButton();
	pushButton_selectX->setObjectName(QString::fromUtf8("pushButton_selectX"));
	pushButton_selectX->setText("select X");

	connect(pushButton_selectX, SIGNAL(clicked()), this, SLOT(on_pushButton_selectX_clicked()));

	gridLayout->addWidget(pushButton_selectX, 0, 0, 1, 1);

	lineEdit_dimX = new QLineEdit();
	lineEdit_dimX->setObjectName(QString::fromUtf8("lineEdit_dimX"));
	lineEdit_dimX->setReadOnly(true);


	gridLayout->addWidget(lineEdit_dimX, 0, 1, 1, 1);

	pushButton_selectY = new QPushButton();
	pushButton_selectY->setObjectName(QString::fromUtf8("pushButton_selectY"));
	pushButton_selectY->setText("select Y");
	connect(pushButton_selectY, SIGNAL(clicked()), this, SLOT(on_pushButton_selectY_clicked()));


	gridLayout->addWidget(pushButton_selectY, 1, 0, 1, 1);

	lineEdit_dimY = new QLineEdit();
	lineEdit_dimY->setObjectName(QString::fromUtf8("lineEdit_dimY"));
	lineEdit_dimY->setReadOnly(true);

	gridLayout->addWidget(lineEdit_dimY, 1, 1, 1, 1);

	DataDrivenGroup->setLayout(gridLayout);

	mainLayout->addWidget(DataDrivenGroup);
	//----------------------------------------------------------

	//----------------------------------------------------------

	listWidget_dims = new QListWidget();
	listWidget_dims->setObjectName(QString::fromUtf8("listWidget_dims"));
	listWidget_dims->clear();



	/*int i, n=m_dim_names.size();
	for(int i = 0; i < m_dim_names.size(); i++) {
		new QListWidgetItem(m_dim_names[i], listWidget_dims);
	}*/



	listWidget_dims->setEnabled(true);

	//listWidget_dims->setGeometry(QRect(7, 250, 531, 161));

	pushButton_apply = new QPushButton();
	pushButton_apply->setObjectName(QString::fromUtf8("pushButton_apply"));
	//pushButton_apply->setGeometry(QRect(7, 420, 531, 26));

	pushButton_apply->setText("Apply");

	mainLayout->addWidget(listWidget_dims);
	mainLayout->addWidget(pushButton_apply);
	connect(pushButton_apply, SIGNAL(clicked()), this, SLOT(apply()));
	//----------------------------------------------------------

	setOneDimOptionsEnabled(false);
	setTwoDimOptionsEnabled(false);
	setTwoDimDataDrivenOptionEnabled(false);

	setLayout(mainLayout);

	m_derivedXDimNumber = 0;
	m_derivedYDimNumber = 1;
	m_dataDrivenXDimNumber = 0;
	m_dataDrivenYDimNumber = 1;

}

ScatterplotCustom::~ScatterplotCustom() {
}



// type = 0: point is square
// type = 1: point is circle
void ScatterplotCustom::switchPointType(){

	//get the currently active ViewWindow. We are displaying this window's dataset.
	ViewDisplay *vd = viewManager->getActiveViewWindow()->getViewDisplay();;
	//we must be looking at a dimstack visualization for this dialog to be relevant
	if(typeid(*vd) != typeid(VisScatter))
		return; //not scatterplot, get out

	//the current visualization is Scatterplot, we can change the parameters.
	VisScatter *visScatter = dynamic_cast<VisScatter *>(vd);

	int shapeType = -1;
	if(radio_shape_square->isChecked()) shapeType = 0;
	else if(radio_shape_circle->isChecked()) shapeType = 1;

	visScatter->setShapeType(shapeType);

	//refresh the display
	viewManager->refreshDisplayByPID(pID);
}

void ScatterplotCustom::setPointSize(int size){
	//get the currently active ViewDisplay Type.
	VisScatter *visScatter = getScatterplotViewDisplay();
	if(visScatter==0)
		//not Scatterplot, get out
		return;

	//the current visualization is Scatterplot, we can change the parameters.
	visScatter->setPointSize(size);

	//refresh the display
	viewManager->refreshDisplayByPID(pID);
}

void ScatterplotCustom::switchDiagDisplayType(){
	//get the currently active ViewDisplay Type.
	VisScatter *visScatter = getScatterplotViewDisplay();
	if (visScatter == 0)
		//not Scatterplot, get out
		return;

	XmdvTool::DIAGPLOT_MODE diagType =  XmdvTool::DIAGPLOT_ORIGINAL;

	setOneDimOptionsEnabled(false);
	setTwoDimOptionsEnabled(false);
	setTwoDimDataDrivenOptionEnabled(false);

	if(radio_original->isChecked()) {
		diagType = XmdvTool::DIAGPLOT_ORIGINAL;
		slider_HistogramSize->setEnabled(false);
		//setTwoDimOptionsEnabled(false);
	}
	else if(radio_histogram->isChecked()) {
		diagType = XmdvTool::DIAGPLOT_HISTOGRAM;
		slider_HistogramSize->setEnabled(true);
		//setTwoDimOptionsEnabled(false);
	}
	else if(radio_one_dimension->isChecked()) {
		diagType = XmdvTool::DIAGPLOT_ONE_DIM;
		slider_HistogramSize->setEnabled(false);
		setOneDimOptionsEnabled(true);
	}
	else if(radio_two_dimension->isChecked()) {
		setTwoDimOptionsEnabled(true);
		if(this->radio_Implicit->isChecked()){
			diagType = XmdvTool::DIAGPLOT_TWO_DIM_IMPLICIT;
			//setTwoDimDataDrivenOptionEnabled(false);
		}else if(this->radio_data_drive->isChecked()){
			diagType = XmdvTool::DIAGPLOT_TWO_DIM_DATA_DRIVEN;
			setTwoDimDataDrivenOptionEnabled(true);
			enableDerivedDims(false);
		}else if(this->radio_derived->isChecked()){
			diagType = XmdvTool::DIAGPLOT_TWO_DIM_DERIVED;
			setTwoDimDataDrivenOptionEnabled(true);
			enableDerivedDims(true);

		}
		slider_HistogramSize->setEnabled(false);

	}

	//the current visualization is Scatterplot, we can change the parameters.
	visScatter->setDiagType(diagType);

	apply();

	//refresh the display
	viewManager->refreshDisplayByPID(pID);
}

void ScatterplotCustom::enableDerivedDims(bool enableFlag){
	listWidget_dims->clear();
	int i;//, n = m_dim_names.size();
	int dimX, dimY;

	VisScatter *visScatter = getScatterplotViewDisplay();

	if(enableFlag==true){
		new QListWidgetItem("Principal_Component_0", listWidget_dims);
		new QListWidgetItem("Principal_Component_1", listWidget_dims);
		new QListWidgetItem("Principal_Component_2", listWidget_dims);
		dimX = visScatter->getDerivedXDimNumber();
		dimY = visScatter->getDerivedYDimNumber();
	}else{
		dimX = visScatter->getDataDrivenXDimNumber();
		dimY = visScatter->getDataDrivenYDimNumber();
	}
	for (i = 0; i < (int)m_dim_names.size(); i++) {
		new QListWidgetItem(m_dim_names[i], listWidget_dims);
	}

	lineEdit_dimX->setText(listWidget_dims->item(dimX)->text());
	lineEdit_dimY->setText(listWidget_dims->item(dimY)->text());


}

void ScatterplotCustom::setTwoDimDataDrivenOptionEnabled(bool enableFlag) {
	VisScatter *visScatter = getScatterplotViewDisplay();
	pushButton_selectX->setEnabled(enableFlag);
	pushButton_selectY->setEnabled(enableFlag);

	lineEdit_dimX->setEnabled(enableFlag);
	lineEdit_dimY->setEnabled(enableFlag);
	listWidget_dims->setEnabled(enableFlag);
	pushButton_apply->setEnabled(enableFlag);

	lineEdit_dimX->setText(m_dim_names[visScatter->getDataDrivenXDimNumber()]);
	lineEdit_dimY->setText(m_dim_names[visScatter->getDataDrivenYDimNumber()]);

}

void ScatterplotCustom::setTwoDimOptionsEnabled(bool enableFlag){
	radio_Implicit->setEnabled(enableFlag);
	radio_data_drive->setEnabled(enableFlag);
	this->radio_derived->setEnabled(enableFlag);
}

void ScatterplotCustom::setOneDimOptionsEnabled(bool enableFlag){
	this->combo_sort->setEnabled(enableFlag);
}

void ScatterplotCustom::setHistoSize(int histoSize){
	//get the currently active ViewDisplay Type.

	VisScatter *visScatter = getScatterplotViewDisplay();
	if(visScatter==0)
		//not Scatterplot, get out
		return;

	PipelineManager* pm = mainWnd->getPipelineManager();
	Transformation* mainT = pm->getPipeline(pID)->getMainTransformation();
	HistogramOperator* histogramO = dynamic_cast<HistogramOperator*>(mainT->getOperator(PL_FN_OP_HISTOGRAM));

	histogramO->setHistogramSize(histoSize);

	mainT->enableOperator(PL_FN_OP_HISTOGRAM, true);

	pm->assemble(pID, histogramO);

	//the current visualization is Scatterplot, we can change the parameters.
	//visScatter->setHistoSize(histoSize);

	//refresh the display
	viewManager->refreshDisplayByPID(pID);
}

void ScatterplotCustom::switchOneDOrder(int sortDim){
	viewManager->refreshDisplayByPID(pID);
	VisScatter *visScatter = getScatterplotViewDisplay();
	if (visScatter == 0)
		//not Scatterplot, get out
		return;
	visScatter->setOneDimSortDim(sortDim);
	viewManager->refreshDisplayByPID(pID);
}

void ScatterplotCustom::on_pushButton_selectX_clicked() {
	int row = listWidget_dims->currentRow();

	VisScatter *visScatter = getScatterplotViewDisplay();
	XmdvTool::DIAGPLOT_MODE diagType =  visScatter->getDiagType();
	if(diagType==XmdvTool::DIAGPLOT_TWO_DIM_DATA_DRIVEN){
		m_dataDrivenXDimNumber = row;
	}else if(diagType==XmdvTool::DIAGPLOT_TWO_DIM_DERIVED){
		m_derivedXDimNumber = row;
	}
	lineEdit_dimX->setText(listWidget_dims->currentItem()->text());


}

void ScatterplotCustom::on_pushButton_selectY_clicked() {
	int row = listWidget_dims->currentRow();
	VisScatter *visScatter = getScatterplotViewDisplay();
	XmdvTool::DIAGPLOT_MODE diagType =  visScatter->getDiagType();
	if (diagType == XmdvTool::DIAGPLOT_TWO_DIM_DATA_DRIVEN) {
		m_dataDrivenYDimNumber = row;
	} else if (diagType == XmdvTool::DIAGPLOT_TWO_DIM_DERIVED) {
		m_derivedYDimNumber = row;
	}
	lineEdit_dimY->setText(listWidget_dims->currentItem()->text());

}


//get the currently active ViewWindow. We are displaying this window's dataset.
VisScatter* ScatterplotCustom::getScatterplotViewDisplay(){

	ViewDisplay *vd = viewManager->getActiveViewWindow()->getViewDisplay();
	//we must be looking at a scatterplot visualization for this dialog to be relevant
	if (typeid(*vd) != typeid(VisScatter))
		return 0;

	VisScatter *visScatter = dynamic_cast<VisScatter *> (vd);
	return visScatter;
}

void ScatterplotCustom::apply() {

	VisScatter *visScatter = getScatterplotViewDisplay();

	PipelineManager* pm = mainWnd->getPipelineManager();
	Transformation* mainT = getOkcPipeline()->getMainTransformation();
	PCADeriveOperator* pcaO = dynamic_cast<PCADeriveOperator*>(mainT->getOperator(PL_FN_OP_PCADERIVE));

	if (visScatter == 0)
		//not Scatterplot, get out
		return;
	XmdvTool::DIAGPLOT_MODE diagType =  visScatter->getDiagType();
	if(diagType == XmdvTool::DIAGPLOT_TWO_DIM_DATA_DRIVEN){
		visScatter->setDataDrivenXDimNumber(m_dataDrivenXDimNumber);
		visScatter->setDataDrivenYDimNumber(m_dataDrivenYDimNumber);
	}else if (diagType == XmdvTool::DIAGPLOT_TWO_DIM_DERIVED) {
		visScatter->setDerivedXDimNumber(m_derivedXDimNumber);
		visScatter->setDerivedYDimNumber(m_derivedYDimNumber);
		// Enable the PCA operator
		mainT->enableOperator(PL_FN_OP_PCADERIVE, true);

		OkcPipeline* okcpl = getOkcPipeline();

		// Reassemble the pipeline
		if (okcpl->needRunPCA()) {
			// If the PCA algorithm has been run once,
			// assemble the pipeline from the pca operator
			pm->assemble(pID, pcaO);
			okcpl->setNeedRunPCA(false);
		} else {
			// If the PCA algorithm has been run once,
			// we only need to do visual mapping
			//pm->assemble(m_pipelineID, -1);
		}
	}
	//refresh the display
	viewManager->refreshDisplayByPID(pID);
}

OkcPipeline* ScatterplotCustom::getOkcPipeline() {
    PipelineManager* pm = mainWnd->getPipelineManager();
    Pipeline* pl = pm->getPipeline(pID);
    return dynamic_cast<OkcPipeline*>(pl);
}
