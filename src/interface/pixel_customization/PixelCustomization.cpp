/*
 * PixelCustomization.cpp
 *
 *  Created on: May 18, 2009
 *      Author: jstasik
 */

#include "interface/pixel_customization/PixelCustomization.h"

#include <typeinfo>
#include <assert.h>

#include <QtGui/QGridLayout>
#include <QtGui/QButtonGroup>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include "main/XmdvToolMainWnd.h"
#include "data/multidim/OkcData.h"
#include "pipeline/Transformation.h"
#include "pipeline/multidim/OkcPipeline.h"
#include "interface/pixel_customization/PixelCustomizationSettings.h"
#include "view/multidim/VisPixel.h"
#include "view/ViewDisplay.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/auxiliary/Subwindow.h"


PixelCustomization::PixelCustomization(XmdvToolMainWnd* mainWnd, QMdiSubWindow *window,
		PixelCustomizationSettings *setting)
: QDialog(mainWnd)
{
	brushOpen = false;
	this->mainWnd = mainWnd;

	// Get the pointer to the VisPixel
	ViewDisplay *vd = mainWnd->getViewManager()->
	getActiveViewWindow()->getViewDisplay();
	assert(typeid(*vd) == typeid(VisPixel));
	visPixel = dynamic_cast<VisPixel *>(vd);

	// Get the pointer to the OkcPipeline
	m_okcPipeline = dynamic_cast<OkcPipeline*>
	( mainWnd->getViewManager()->
			getActiveViewWindow()->getPipeline() );

	// Get the dimension size
	OkcData* okcdata = visPixel->getDisplayDataset();
	m_dim_size = okcdata->getDimSize();
	okcdata->getDimNameArr(m_dim_names);
	m_orig_dim_size = okcdata->getOrigDimSize();
	okcdata->getOrigDimNameArr(m_orig_dim_names);

	settings = setting;

	// initialize settings
	if(!settings->isSet) {
		setDefaultValues();
	}

	setWindowTitle(tr("Pixel-Oriented Customization dialog"));

	QGridLayout *grid = new QGridLayout();

	QLabel *selectLabel = new QLabel(tr("Selected dim:"));
	QLabel *sortLabel = new QLabel(tr("Sort by:"));
	QLabel *layoutLabel = new QLabel(tr("Pixel layout:"));
	QLabel *orderLabel = new QLabel(tr("Order:"));
	QLabel *scaleLabel = new QLabel(tr("Scale:"));
	QLabel *placementLabel = new QLabel(tr("Placement type:"));

	// Create sorted/selected combo boxes
	combo_sortedDim = new QComboBox();
	combo_selectedDim = new QComboBox();
	combo_sortedDim->addItem("None", 0);
	int i;
	for (i = 0; i < m_dim_size; i++) {
		combo_selectedDim->addItem(m_dim_names[i], i);
	}
	for (i = 0; i < m_orig_dim_size; i++) {
		combo_sortedDim->addItem(m_orig_dim_names[i], i);
	}
	combo_sortedDim->setCurrentIndex(settings->settings_sortedDim+1);
	connect(combo_sortedDim, SIGNAL(currentIndexChanged(int)), this, SLOT(setSortedDim(int)));
	combo_selectedDim->setCurrentIndex(settings->settings_selectedDim);
	connect(combo_selectedDim, SIGNAL(currentIndexChanged(int)), this, SLOT(setSelectedDim(int)));

	combo_pixelLayout = new QComboBox();
	combo_pixelLayout->addItem("Spiral", 0);
	combo_pixelLayout->addItem("Horizontal", 1);
	combo_pixelLayout->addItem("Vertical", 2);
	connect(combo_pixelLayout, SIGNAL(currentIndexChanged(int)), this, SLOT(setPixelLayout(int)));

	// Create order radio buttons
	QButtonGroup *orderGroup = new QButtonGroup();
	radio_ascending = new QRadioButton(tr("&ascending"));
	radio_descending = new QRadioButton(tr("&descending"));
	orderGroup->addButton(radio_ascending);
	orderGroup->addButton(radio_descending);
	if(settings->settings_asc) radio_ascending->setChecked(true);
	else radio_descending->setChecked(true);

	QHBoxLayout *orderLayout = new QHBoxLayout();
	orderLayout->addWidget(radio_ascending);
	orderLayout->addWidget(radio_descending);
	connect(orderGroup, SIGNAL(buttonClicked(int)), this, SLOT(setOrder(int)));

	// Create scale slider
	slider_scale = new QSlider();
	slider_scale->setMaximum(50);
	slider_scale->setMinimum(0);
	slider_scale->setTickInterval(1);
	slider_scale->setValue(settings->settings_distortion[settings->settings_selectedDim]);
	slider_scale->setOrientation(Qt::Horizontal);
	connect(slider_scale, SIGNAL(valueChanged(int)), this, SLOT(setScale(int)));

	QVBoxLayout *scaleLayout = new QVBoxLayout();
	QHBoxLayout *scaleSliderLayout = new QHBoxLayout();
	QLabel *scaleValueLabel = new QLabel(QString::number(settings->settings_distortion[settings->settings_selectedDim]));
	scaleSliderLayout->addWidget(scaleValueLabel);
	scaleSliderLayout->addWidget(slider_scale);
	check_scaleAll = new QCheckBox("Scale All:");
	check_scaleAll->setChecked(settings->settings_scaleAll);
	scaleLayout->addWidget(check_scaleAll);
	scaleLayout->addLayout(scaleSliderLayout);
	connect(check_scaleAll, SIGNAL(clicked(bool)), this, SLOT(setScaleAll(bool)));
	connect(slider_scale, SIGNAL(valueChanged(int)), scaleValueLabel, SLOT(setNum(int)));

	// Create placement radio buttons
	QButtonGroup *placementGroup = new QButtonGroup();
	radio_normal = new QRadioButton(tr("&normal"));
	radio_mds = new QRadioButton(tr("&mds"));
	if(settings->settings_placement == PixelCustomizationSettings::RA_NOR)
		radio_normal->setChecked(true);
	else if(settings->settings_placement == PixelCustomizationSettings::RA_MDS)
		radio_mds->setChecked(true);

	placementGroup->addButton(radio_normal);
	placementGroup->addButton(radio_mds);

	connect(placementGroup, SIGNAL(buttonClicked(int)), this, SLOT(setPlacement(int)));

	QHBoxLayout *placementLayout = new QHBoxLayout();
	placementLayout->addWidget(radio_normal);
	placementLayout->addWidget(radio_mds);

	// Create sampling slider
	check_enableSampling= new QCheckBox("Sampling:");
	check_enableSampling->setChecked(settings->settings_enableSampling);
	connect(check_enableSampling, SIGNAL(clicked(bool)), this, SLOT(enableSampling(bool)));

	slider_sampling = new QSlider();
	slider_sampling->setMaximum(100);
	slider_sampling->setMinimum(15);
	slider_sampling->setTickInterval(1);
	slider_sampling->setValue(settings->settings_sampling);
	slider_sampling->setOrientation(Qt::Horizontal);

	QHBoxLayout *samplingLayout = new QHBoxLayout();
	QLabel *samplingValueLabel = new QLabel(QString::number(settings->settings_sampling));
	samplingLayout->addWidget(samplingValueLabel);
	samplingLayout->addWidget(slider_sampling);

	connect(slider_sampling, SIGNAL(valueChanged(int)), this, SLOT(setSampling(int)));
	connect(slider_sampling, SIGNAL(valueChanged(int)), samplingValueLabel, SLOT(setNum(int)));

	// Add widgets to grid layout
	grid->addWidget(selectLabel, 0, 0, Qt::AlignRight);
	grid->addWidget(sortLabel, 1, 0, Qt::AlignRight);
	grid->addWidget(layoutLabel, 2, 0, Qt::AlignRight);
	grid->addWidget(orderLabel, 3, 0, Qt::AlignRight);
	grid->addWidget(scaleLabel, 4, 0, Qt::AlignRight);
	grid->addWidget(placementLabel, 5, 0, Qt::AlignRight);
	grid->addWidget(check_enableSampling, 6, 0, Qt::AlignRight);

	grid->addWidget(combo_selectedDim, 0, 1);
	grid->addWidget(combo_sortedDim, 1, 1);
	grid->addWidget(combo_pixelLayout, 2, 1);
	grid->addLayout(orderLayout, 3, 1);
	grid->addLayout(scaleLayout, 4, 1);
	grid->addLayout(placementLayout, 5, 1);
	grid->addLayout(samplingLayout, 6, 1);

	this->setLayout(grid);
}

void PixelCustomization::setDefaultValues() {
	settings->settings_selectedDim = 0;
	settings->settings_sortedDim = -1;
	settings->settings_asc = true;
	settings->settings_scaleAll = true;
	settings->settings_sampling = 50;
	settings->settings_enableSampling = false;
	settings->settings_placement = PixelCustomizationSettings::RA_NOR;
	settings->settings_distortion.resize(visPixel->m_dim_size + 1);
	settings->settings_displayType = Subwindow::PDT_SPIRAL;
	for(int i = 0; i < visPixel->m_dim_size+1; i++) {
		settings->settings_distortion[i] = 0;
	}
	settings->isSet = true;
}

void PixelCustomization::disableGUI() {
	slider_sampling->setEnabled(false);
	check_enableSampling->setEnabled(false);
}

PixelCustomization::~PixelCustomization() {
}

void PixelCustomization::setSortedDim(int dim) {
	settings->settings_sortedDim = dim-1;
	visPixel->setSortedDim(dim-1);
	visPixel->refresh(true, false);
}

void PixelCustomization::setSelectedDim(int dim) {
	settings->settings_selectedDim = dim;
	visPixel->setSelectedDim(dim);
	visPixel->setPlacementHasModified();
	slider_scale->setValue(settings->settings_distortion[settings->settings_selectedDim]);
	visPixel->refresh(false, false);
}

void PixelCustomization::setOrder(int) {
	visPixel->setSortDimOrder(radio_ascending->isChecked());
	settings->settings_asc = radio_ascending->isChecked();
	visPixel->refresh(true, false);
}

void PixelCustomization::setScale(int scale) {
	int dim = combo_selectedDim->currentIndex();
	if(settings->settings_scaleAll) {
		for(int i = 0; i < visPixel->m_dim_size; i++) {
			settings->settings_distortion[i] = scale;
			visPixel->setDimDistortion(i, scale/100.0f);
		}
	} else {
		settings->settings_distortion[dim] = scale;
		visPixel->setDimDistortion(combo_selectedDim->currentIndex(), scale/100.0f);
	}
	visPixel->setDistortionHasModified();
	visPixel->refresh(false, false);
}

void PixelCustomization::setScaleAll(bool scale) {
	settings->settings_scaleAll = scale;
	if(scale)
		setScale(slider_scale->value());
}

void PixelCustomization::setPlacement(int) {
	if(radio_normal->isChecked()) {
		visPixel->setPlacementType(VisPixel::PPT_NORMAL);
		settings->settings_placement = PixelCustomizationSettings::RA_NOR;
	} else if(radio_mds->isChecked()) {
		visPixel->setPlacementType(VisPixel::PPT_MDS);
		settings->settings_placement = PixelCustomizationSettings::RA_MDS;
	}
	visPixel->setPlacementHasModified();
	visPixel->refresh(false, false);
}

void PixelCustomization::setSampling(int rate) {
	if(settings->settings_enableSampling) {
		settings->settings_sampling = rate;
		visPixel->setSamplingFactor(rate/100.0f);
	} else
		visPixel->setSamplingFactor(1.0f);
	visPixel->refresh(false, false);
}

void PixelCustomization::enableSampling(bool enable) {
	settings->settings_enableSampling = enable;
	setSampling(settings->settings_sampling);
}

void PixelCustomization::setPixelLayout(int layout) {
	if(layout == 0) settings->settings_displayType = Subwindow::PDT_SPIRAL;
	else if(layout == 1) settings->settings_displayType = Subwindow::PDT_HORIZONTAL;
	else if(layout == 2) settings->settings_displayType = Subwindow::PDT_VERTICAL;
	visPixel->setDisplay(settings->settings_displayType);
	visPixel->refresh(false, false);
}

void PixelCustomization::applyChanges(ViewDisplay *view) {
	visPixel = dynamic_cast<VisPixel *>(view);
	visPixel->setDisplay(settings->settings_displayType);
	visPixel->setSortedDim(settings->settings_sortedDim);
	visPixel->setSelectedDim(settings->settings_selectedDim);
	settings->settings_asc ? visPixel->setSortDimOrder(true) :
		visPixel->setSortDimOrder(false);
	if (radio_normal->isChecked()) {
		visPixel->setPlacementType(VisPixel::PPT_NORMAL);
	} else if(radio_mds->isChecked()) {
		visPixel->setPlacementType(VisPixel::PPT_MDS);
	}

	int i;
	for (i = 0; i < visPixel->m_dim_size; i++){
		visPixel->setDimDistortion(i, settings->settings_distortion[i]/100.0f);
	}
	if(check_enableSampling->isChecked())
		visPixel->setSamplingFactor(slider_sampling->value()/100.0f);
	else
		visPixel->setSamplingFactor(1.0f);
	visPixel->setDialog(this);
	visPixel->refresh(false, true);
}

PixelCustomizationSettings* PixelCustomization::getSettings() {
	return settings;
}

void PixelCustomization::update() {
	combo_sortedDim->setCurrentIndex(settings->settings_sortedDim+1);
	combo_selectedDim->setCurrentIndex(settings->settings_selectedDim);
	settings->settings_asc ? radio_ascending->setChecked(true) :
		radio_descending->setChecked(true);
	slider_scale->setValue(settings->settings_distortion[settings->settings_selectedDim]);
}

void PixelCustomization::updateMDS(double **mds) {

	settings->settings_mdsPos = new double * [2];
	settings->settings_mdsPos[0] = new double [visPixel->m_dim_size+1];
	settings->settings_mdsPos[1] = new double [visPixel->m_dim_size+1];
	for(int i = 0; i < visPixel->m_dim_size+1; i++) {
		settings->settings_mdsPos[0][i] = mds[0][i];
		settings->settings_mdsPos[1][i] = mds[1][i];
	}
}

VisPixel* PixelCustomization::getVisPixel() {
	return visPixel;
}
