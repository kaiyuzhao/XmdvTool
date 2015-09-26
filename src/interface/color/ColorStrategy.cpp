/*
 * ColorStrategy.cpp
 *
 *  Created on: Jul 8, 2009
 *      Author: jstasik
 */

#include "interface/color/ColorStrategy.h"

#include <typeinfo>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "data/multidim/OkcData.h"
#include "data/Data.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "main/XmdvToolMainWnd.h"
#include "color/ColorManager.h"
#include "pipeline/Transformation.h"
#include "operator/rowindex/SortRowsOperator.h"
#include "view/ViewManager.h"


ColorStrategy::ColorStrategy(XmdvToolMainWnd* mainwnd, int pid) : QDialog(mainwnd) {
	m_valueDim = 0;
	m_tempValue = -1;
	m_selectedRadio = 0;
	mainWnd = mainwnd;
	m_pid = pid;

	Data *data = mainWnd->getPipelineManager()->getPipeline(m_pid)->getOrigDataset();
	if(typeid(*data) == typeid(OkcData)) {
		m_dim_names.resize(((OkcData*)data)->getOrigDimSize());
		((OkcData*)data)->getDimNameArr(m_dim_names);
	} else {
		close();
		return;
	}

	setWindowTitle(tr("Color Strategy Selection"));

	QVBoxLayout *mainLayout = new QVBoxLayout();

	//----------------------------------------------------------
	QGroupBox *radioGroup = new QGroupBox(tr("Color Strategy"));
	QHBoxLayout *radioLayout = new QHBoxLayout();

	radio_none = new QRadioButton(tr("None"));
	connect(radio_none, SIGNAL(clicked()), this, SLOT(switchRadio()));
	radio_order = new QRadioButton(tr("Order"));
	radio_order->setChecked(true);
	connect(radio_order, SIGNAL(clicked()), this, SLOT(switchRadio()));
	radio_value = new QRadioButton(tr("Value"));
	connect(radio_value, SIGNAL(clicked()), this, SLOT(switchRadio()));

	radioLayout->addWidget(radio_none);
	radioLayout->addWidget(radio_order);
	radioLayout->addWidget(radio_value);

	radioGroup->setLayout(radioLayout);
	mainLayout->addWidget(radioGroup);
	//----------------------------------------------------------

	//----------------------------------------------------------
	QGroupBox *sortGroup = new QGroupBox(tr("Sorting Dimension"));
	QHBoxLayout *sortLayout = new QHBoxLayout();

	check_asc = new QCheckBox(tr("Increase"));
	check_asc->setChecked(true);
	combo_sort = new QComboBox();
	combo_sort->addItem("Don't Change Order", 0);
	combo_sort->addItem("Turn Off Sorting", 1);

	sortLayout->addWidget(check_asc);
	sortLayout->addWidget(combo_sort);

	sortGroup->setLayout(sortLayout);
	mainLayout->addWidget(sortGroup);
	//----------------------------------------------------------

	//----------------------------------------------------------
	group_value = new QGroupBox(tr("Value Dimension"));
	QHBoxLayout *valueLayout = new QHBoxLayout();

	line_value = new QLineEdit();
	line_value->setReadOnly(true);

	list_dims = new QListWidget();
	connect(list_dims, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectDim(QListWidgetItem*)));
	listitems_dims.resize(m_dim_names.size());

	valueLayout->addWidget(line_value);

	group_value->setLayout(valueLayout);
	mainLayout->addWidget(group_value);
	mainLayout->addWidget(list_dims);
	//----------------------------------------------------------

	for(int i = 0; i < (int)m_dim_names.size(); i++) {
		combo_sort->addItem(m_dim_names[i], i);
		listitems_dims[i] = new QListWidgetItem(m_dim_names[i], list_dims);
	}

	QHBoxLayout *buttons = new QHBoxLayout();
	QPushButton *apply = new QPushButton(tr("Apply"));
	connect(apply, SIGNAL(clicked()), this, SLOT(apply()));
	QPushButton *cancel = new QPushButton(tr("Cancel"));
	connect(cancel, SIGNAL(clicked()), this, SLOT(cancel()));
	buttons->addWidget(apply);
	buttons->addWidget(cancel);

	mainLayout->addLayout(buttons);

	setLayout(mainLayout);

	switchRadio();
}

ColorStrategy::~ColorStrategy() {
	for(int i = 0; i < (int)listitems_dims.size(); i++)
		SAFE_DELETE(listitems_dims[i]);
	listitems_dims.clear();
}

void ColorStrategy::switchRadio() {
	if(radio_none->isChecked()) m_selectedRadio = 0;
	else if(radio_order->isChecked()) m_selectedRadio = 1;
	else if(radio_value->isChecked()) m_selectedRadio = 2;

	switch(m_selectedRadio) {
	case 0:
		group_value->setEnabled(false);
		list_dims->setEnabled(false);
		break;
	case 1:
		group_value->setEnabled(false);
		list_dims->setEnabled(false);
		break;
	case 2:
		group_value->setEnabled(true);
		list_dims->setEnabled(true);
		break;
	}
}

void ColorStrategy::selectDim(QListWidgetItem* item) {
	int dim = list_dims->row(item);
	m_tempValue = dim;
	line_value->setText(item->text());
}

void ColorStrategy::reset() {
	m_tempValue = -1;
	combo_sort->setCurrentIndex(0);
	line_value->setText("");
	check_asc->setChecked(true);
}

void ColorStrategy::cancel() {
	reset();
	close();
}

void ColorStrategy::apply() {
	ColorManager *cm = g_globals.colorManager;

	PipelineManager* pm = mainWnd->getPipelineManager();
	ViewManager * vm = mainWnd->getViewManager();
	Transformation* mainT = pm->getPipeline(m_pid)->getMainTransformation();
	SortRowsOperator* sortO = dynamic_cast<SortRowsOperator*>(mainT->getOperator(PL_FN_OP_SORT));

	switch(m_selectedRadio) {
	case 0:
		cm->setStrategy(ColorManager::NONE);
		break;
	case 1:
		cm->setStrategy(ColorManager::ORDER);
		break;
	case 2:
		if(m_tempValue > -1) {
			cm->setStrategy(ColorManager::VALUE);
			m_valueDim = m_tempValue;
		}
		break;
	}

	int sortDim = combo_sort->currentIndex();
	if(sortDim > 1) {
		mainT->enableOperator(PL_FN_OP_SORT, true);
		sortO->setSortWay (XmdvTool::OKCDIM_ORIGINAL);
		sortO->setSortDimNo (sortDim-2);
		sortO->setSortOrder (check_asc->isChecked() ? XmdvTool::SORTORDER_INCREASE : XmdvTool::SORTORDER_DECREASE);
		pm->assemble(m_pid, sortO);
	} else if(sortDim == 1) {
		mainT->enableOperator(PL_FN_OP_SORT, false);
	} else {
		pm->assemble(m_pid, -1);
	}

	vm->refreshDisplayByPID(m_pid);

	reset();
}
