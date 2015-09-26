/*
 * SBBControl.cpp
 *
 *  Created on: Aug 11, 2009
 *      Author: Zaixian Xie
 */

#include "interface/sbb/SBBControl.h"

#include <QMessageBox>

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "pipeline/Transformation.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Pipeline.h"
#include "pipeline/multidim/SBBPipeline.h"
#include "operator/sbb/SBBOperator.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "view/ViewManager.h"
#include "interface/sbb/HierNavDisplay.h"
#include "interface/sbb/HierNav.h"
#include "data/cluster/TreeContour.h"

SBBControl::SBBControl(XmdvToolMainWnd *mainWnd, PipelineManager* pm, int pID)
	: QDialog(mainWnd) {
	setWindowTitle( tr("Structure-Based Brush") );
	m_mainWnd = mainWnd;
	m_pipelineManager = pm;
	m_pipelineID = pID;
	Pipeline* pl = pm->getPipeline(pID);
	SBBPipeline* sbbPL = dynamic_cast<SBBPipeline*>(pl);
	m_hier_info = sbbPL->getHierInfo();
	setupGui(sbbPL);
	this->resize(350,500);
}

SBBControl::~SBBControl() {
}

void SBBControl::setupGui(SBBPipeline* pl) {
	// set the GLwidget to render the cluster tree
	m_hierNavDisplay = new HierNavDisplay(this);
	m_hierNav = new HierNav(m_mainWnd, this);
	m_hierNav->initDisplay(m_hierNavDisplay);
	m_hierNav->setHierInfo(m_hier_info);
	m_hierNav->setTreeContour(pl->getTreeContour());
	m_hierNavDisplay->setHierNav(m_hierNav);

	scrollArea = new QScrollArea();
	scrollArea->setWidget(m_hierNavDisplay);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	// Create the label to show what users can do
	// in terms of the mouse position
	labelCaption = new QLabel("Inactive area");

	// Set the slider for fade factor
	label1 = new QLabel("Dynamic Masking");
	slider_fade_factor = new QSlider();
	slider_fade_factor->setMaximum(100);
	slider_fade_factor->setMinimum(-100);
	slider_fade_factor->setTickInterval(1);
	slider_fade_factor->setValue((int)(m_hier_info->fade_factor*100));
	slider_fade_factor->setOrientation(Qt::Horizontal);

	QHBoxLayout *sliderFadeLayout = new QHBoxLayout();

	labelFadeFactor = new QLabel(QString::number(m_hier_info->fade_factor, 'f', 2));
	sliderFadeLayout->addWidget(labelFadeFactor);
	sliderFadeLayout->addWidget(slider_fade_factor);
	connect(slider_fade_factor, SIGNAL(valueChanged(int)), this, SLOT(setFadeFactorLabel(int)) );
	connect(slider_fade_factor, SIGNAL(valueChanged(int)), this, SLOT(updateFadeFactor(int)) );

	// Set the label and slider for non-brushed cluster radius
	label2 = new QLabel("Non-brushed Cluster Radius");
	slider_nbradius = new QSlider();
	slider_nbradius->setMaximum(100);
	slider_nbradius->setMinimum(0);
	slider_nbradius->setTickInterval(1);
	slider_nbradius->setValue((int)(m_hier_info->cluster_radius*100));
	slider_nbradius->setOrientation(Qt::Horizontal);

	QHBoxLayout *sliderClusterRLayout = new QHBoxLayout();
	labelNBRadius = new QLabel(QString::number(m_hier_info->cluster_radius, 'f', 2));
	sliderClusterRLayout->addWidget(labelNBRadius);
	sliderClusterRLayout->addWidget(slider_nbradius);
	connect(slider_nbradius, SIGNAL(valueChanged(int)), this, SLOT(setNBRadiusLabel(int)) );
	connect(slider_nbradius, SIGNAL(valueChanged(int)), this, SLOT(updateClusterRadius(int)) );

	// Set the label and slider for brushed cluster radius
	label3 = new QLabel("Brushed Cluster Radius");
	slider_bradius = new QSlider();
	slider_bradius->setMaximum(100);
	slider_bradius->setMinimum(0);
	slider_bradius->setTickInterval(1);
	slider_bradius->setValue((int)(m_hier_info->brushed_radius*100));
	slider_bradius->setOrientation(Qt::Horizontal);

	QHBoxLayout *sliderBrushedRLayout = new QHBoxLayout();
	labelBRadius = new QLabel(QString::number(m_hier_info->brushed_radius, 'f', 2));
	sliderBrushedRLayout->addWidget(labelBRadius);
	sliderBrushedRLayout->addWidget(slider_bradius);
	connect(slider_bradius, SIGNAL(valueChanged(int)), this, SLOT(setBRadiusLabel(int)) );
	connect(slider_bradius, SIGNAL(valueChanged(int)), this, SLOT(updateBrushedRadius(int)) );

	// set the check box for showing  bands
	check_showBands = new QCheckBox("Show Bands", this);
	check_showBands->setChecked(m_hier_info->SHOW_BANDS);
	connect(check_showBands, SIGNAL(stateChanged(int)), this, SLOT(updateShowBands(int)) );

	// Set the label and slider for extent scaling factor
	label4 = new QLabel("Extent Scaling");
	slider_extent = new QSlider();
	slider_extent->setMaximum(100);
	slider_extent->setMinimum(0);
	slider_extent->setTickInterval(1);
	slider_extent->setValue(100-(int)(m_hier_info->extent_scaling_fac*100.0));
	slider_extent->setOrientation(Qt::Horizontal);

	QHBoxLayout *sliderExtentLayout = new QHBoxLayout();
	labelExtent = new QLabel(QString::number(slider_extent->value()));
	sliderExtentLayout->addWidget(labelExtent);
	sliderExtentLayout->addWidget(slider_extent);
	connect(slider_extent, SIGNAL(valueChanged(int)), this, SLOT(setExtentLabel(int)) );
	connect(slider_extent, SIGNAL(valueChanged(int)), this, SLOT(updateExtentScaling(int)) );

	// Set the label and input box for left handle position
	label5 = new QLabel("Left Handle Position(0-1)");
	line_lHandlePos = new QLineEdit(QString::number(m_hier_info->tree_brush[0]));
	connect(line_lHandlePos, SIGNAL(returnPressed()), this, SLOT(updateHandlePosition()));

	// Set the label and input box for left handle position
	label6 = new QLabel("Right Handle Position(0-1)");
	line_rHandlePos = new QLineEdit(QString::number(m_hier_info->tree_brush[1]));
	connect(line_rHandlePos, SIGNAL(returnPressed()), this, SLOT(updateHandlePosition()));

	// Create the lower left box layout
	lowerLGroupBox = new QGroupBox();

	lowerLLayout = new QVBoxLayout();
	lowerLLayout->addWidget(label1);
	lowerLLayout->addLayout(sliderFadeLayout);
	lowerLLayout->addWidget(label2);
	lowerLLayout->addLayout(sliderClusterRLayout);
	lowerLLayout->addWidget(label3);
	lowerLLayout->addLayout(sliderBrushedRLayout);

	lowerLGroupBox->setLayout(lowerLLayout);

	// Create the lower right box layout
	lowerRGroupBox = new QGroupBox();

	lowerRLayout = new QVBoxLayout();
	lowerRLayout->addWidget(check_showBands);
	lowerRLayout->addWidget(label4);
	lowerRLayout->addLayout(sliderExtentLayout);
	lowerRLayout->addWidget(label5);
	lowerRLayout->addWidget(line_lHandlePos);
	lowerRLayout->addWidget(label6);
	lowerRLayout->addWidget(line_rHandlePos);

	lowerRGroupBox->setLayout(lowerRLayout);

	// Combine two layout widgets at the bottom and combine them into one layout widget
	lowerGroupBox = new QGroupBox();

	lowerLayout = new QHBoxLayout();
	lowerLayout->addWidget(lowerLGroupBox);
	lowerLayout->addWidget(lowerRGroupBox);

	lowerGroupBox->setLayout(lowerLayout);

	// Create the main layout
	mainLayout = new QVBoxLayout();
	mainLayout->addWidget(scrollArea);
	mainLayout->addWidget(labelCaption);
	mainLayout->addWidget(lowerGroupBox);

	this->setLayout(mainLayout);
}

void SBBControl::setFadeFactorLabel(int fade_factor)
{
	labelFadeFactor->setText(QString::number(fade_factor/100.0, 'f', 2));
}

void SBBControl::setNBRadiusLabel(int nbradius)
{
	labelNBRadius->setText(QString::number(nbradius/100.0, 'f', 2));
}

void SBBControl::setBRadiusLabel(int bradius)
{
	labelBRadius->setText(QString::number(bradius/100.0, 'f', 2));
}

void SBBControl::setExtentLabel(int extent) {
	labelExtent->setText(QString::number(extent));
}

void SBBControl::setMainWnd(XmdvToolMainWnd* mainWnd) {
	m_mainWnd = mainWnd;
}

void SBBControl::setPipelineID(int pID) {
	m_pipelineID = pID;
}

void SBBControl::updateFadeFactor(int fade_factor) {
	m_hier_info->fade_factor = fade_factor/100.0;

	// fade_factor is used while generating view,
	// thus we do not need to re-assemble the pipeline
	updateHierDisplay();
}

void SBBControl::updateExtentScaling(int extent) {
	m_hier_info->extent_scaling_fac = (100-extent)/100.0;

	// extent_scaling_fac is used while generating view,
	// thus we do not need to re-assemble the pipeline
	updateHierDisplay();
}

void SBBControl::updateClusterRadius(int nbradius) {
	m_hier_info->cluster_radius = nbradius/100.0;

	// Update the cluster tree
	m_hierNavDisplay->updateGL();
	// Reassemble the pipeline
	ReassemblePipeline(DIRTY_CLUSTER_RADIUS);
	// Update the hierarchical view
	updateHierDisplay();
}

void SBBControl::updateBrushedRadius(int bradius) {
	m_hier_info->brushed_radius = bradius/100.0;

	// Update the cluster tree
	m_hierNavDisplay->updateGL();
	// Reassemble the pipeline
	ReassemblePipeline(DIRTY_BRUSHED_RADIUS);
	// Update the hierarchical view
	updateHierDisplay();
}

void SBBControl::updateHandlePosition() {

	//First check whether the input of handle position is valid
	bool isOk;
	double lpos = line_lHandlePos->text().toDouble(&isOk);
	if (!isOk || lpos<0.0 || lpos>1.0) {
		QMessageBox::warning(this->parentWidget(),
				"Invalid Input",
				"The left handle position must be a number between 0.0 and 1.0 !");
		line_lHandlePos->setText( QString::number(m_hier_info->tree_brush[0]) );
		return;
	}

	double rpos = line_rHandlePos->text().toDouble(&isOk);
	if (!isOk || rpos<0.0 || rpos>1.0) {
		QMessageBox::warning(this->parentWidget(),
				"Invalid Input",
				"The right handle position must be a number between 0.0 and 1.0 !");
		line_rHandlePos->setText( QString::number(m_hier_info->tree_brush[1]) );
		return;
	}

	if (lpos > rpos) {
		QMessageBox::warning(this->parentWidget(),
				"Invalid Input",
				"The left handle position must be smaller than right handle position !");
		line_lHandlePos->setText( QString::number(m_hier_info->tree_brush[0]) );
		line_rHandlePos->setText( QString::number(m_hier_info->tree_brush[1]) );
		return;
	}

	m_hier_info->tree_brush[0] = lpos;
	m_hier_info->tree_brush[1] = rpos;

	// Update the cluster tree
	m_hierNavDisplay->updateGL();
	// Reassemble the pipeline
	ReassemblePipeline(DIRTY_HANDLE_POSITION);
	// Update the hierarchical view
	updateHierDisplay();
}


void SBBControl::updateShowBands(int) {
	m_hier_info->SHOW_BANDS = check_showBands->isChecked();

	// SHOW_BANDS is used while generating view,
	// thus we do not need to re-assemble the pipeline
	updateHierDisplay();
}

void SBBControl::updateDisplay(int changeFlag) {
	// updpate the cluster tree
	m_hierNavDisplay->updateGL();
	// update widgets
	updateWidgets();

	// Reassemble the
	ReassemblePipeline(changeFlag);
	updateHierDisplay();
}

void SBBControl::ReassemblePipeline(int changeFlag) {
	// Get the pointer to the pipeline
	Pipeline* pl = m_pipelineManager->getPipeline(m_pipelineID);
	// Get the pointer to the SBBOperator
	SBBOperator* sbbQ = dynamic_cast<SBBOperator*>
		(pl->getMainTransformation()->getOperator(PL_SBB_OP_SBB));
	// mark a flag for the SBBOperator,
	// so the SBBOperator will modify the flag for clusters
	// in terms of the new parameters.
	sbbQ->markChangeFlag(changeFlag);
	// re-assemble the pipeline starting from SBB operator.
	m_pipelineManager->assemble(m_pipelineID, PL_SBB_OP_SBB);
	// Clear the flag for the SBB Operator
	sbbQ->clearChangeFlag();

}

void SBBControl::updateHierDisplay() {
	ViewManager* vm = m_mainWnd->getViewManager();
	vm->refreshDisplayByPID(m_pipelineID);
}

void SBBControl::showCaption(QString caption) {
	labelCaption->setText(caption);
}

void SBBControl::updateWidgets() {
	slider_fade_factor->setValue((int)(m_hier_info->fade_factor*100));
	slider_nbradius->setValue((int)(m_hier_info->cluster_radius*100));
	slider_bradius->setValue((int)(m_hier_info->brushed_radius*100));
	line_lHandlePos->setText(QString::number(m_hier_info->tree_brush[0]));
	line_rHandlePos->setText(QString::number(m_hier_info->tree_brush[1]));
}
