/*
 * ColorSelector.cpp
 *
 *  Created on: Jun 12, 2009
 *  Author: Jason Stasik
 */

#include "interface/color/ColorSelector.h"
#include "color/ColorMap.h"
#include "color/ColorManager.h"
#include "interface/color/ColorCustom.h"
#include "datatype/RGBt.h"
#include "main/XmdvTool.h"
#include "pipeline/PipelineManager.h"
#include "view/ViewManager.h"
#include "main/XmdvToolMainWnd.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QColorDialog>
#include <QPushButton>
#include <QColor>
#include <QButtonGroup>
#include <QColor>
#include <QSpinBox>
#include <QCheckBox>
#include <QPixmap>
#include <QCloseEvent>


ColorSelector::ColorSelector(ColorCustom *colorCustom, ColorMap* color, int idx) : QDialog(colorCustom) {
	setWindowTitle(tr("Color Selection"));

	m_index = idx;
	int index;
	m_color = color;
	// Make sure it is deleted when closed, not just hidden
	setAttribute(Qt::WA_DeleteOnClose);

	// A ButtonGroup is a container used to make sure only 1 radio button is selected at a time
	// A GroupBox is merely a container used to graphically link related radio buttons
	QButtonGroup *buttongroup = new QButtonGroup(this);

	ColorManager *cm = g_globals.colorManager;

	m_colorCustom = colorCustom;
	m_map = new QSignalMapper(this);

	m_numBins = m_color->getColorRamp()->getNumBins();
	m_color->getColorRamp()->getRampMode() == ColorRamp::BINS ? m_bins = true : m_bins = false;

	QVBoxLayout *layout = new QVBoxLayout();

	QGroupBox *single, *seq, *div, *qual, *link;

	m_selected = -1;
	m_singleColor = new ColorMap(*m_color);
	m_singleLabel = new QLabel();

	int colorType = -1;

	// For each type of ramp, check if that is an allowed type, and if so add it to the dialog
	//---------------------------------------------------------------
	if(m_color->getAllowedType() & ColorRamp::RAMP_SINGLE) {
		single = new QGroupBox("Single Color");
		single->setFixedSize(250, 60);
		QHBoxLayout *hbox = new QHBoxLayout();

		if(m_color->getColorRamp()->getRampType() != ColorRamp::RAMP_SINGLE)
			m_singleColor->setColorRamp(new ColorRamp());

		QRadioButton *button = new QRadioButton();
		buttongroup->addButton(button);
		connect(button, SIGNAL(clicked()), m_map, SLOT(map()));
		m_map->setMapping(button, ColorManager::TOTAL_RAMPS);

		if(m_color->getRampID() == ColorManager::TOTAL_RAMPS) {
			button->setChecked(true);
			setSelectedRadio(ColorManager::TOTAL_RAMPS);
		}

		QPushButton *pbutton = new QPushButton(tr("Change Color"));
		connect(pbutton, SIGNAL(clicked()), this, SLOT(getNewColor()));
		m_singleLabel->setFixedSize(120,15);

		hbox->addWidget(button);
		hbox->addWidget(m_singleLabel);
		hbox->addWidget(pbutton);
		single->setLayout(hbox);
		layout->addWidget(single);
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	if(m_color->getAllowedType() & ColorRamp::RAMP_SEQ) {
		seq = new QGroupBox("Sequential Ramps");

		QGridLayout *grid = new QGridLayout();
		grid->setColumnMinimumWidth(2, 20);

		seq_labels.resize(cm->getPredefinedTotal(ColorManager::SEQUENTIAL));

		index = 0;
		for(int i = cm->getPredefinedStart(ColorManager::SEQUENTIAL); i < cm->getPredefinedEnd(ColorManager::SEQUENTIAL); i++, index++) {
			QRadioButton *button = new QRadioButton();
			buttongroup->addButton(button);
			connect(button, SIGNAL(clicked()), m_map, SLOT(map()));
			m_map->setMapping(button, i);
			seq_labels[index] = new QLabel();
			seq_labels[index]->setFixedSize(120,15);

			if(m_color->getRampID() == i) {
				button->setChecked(true);
				setSelectedRadio(i);
			}

			QLabel *label = new QLabel(cm->getPredefinedColorRampName(i));

			if(index % 2 == 0) {
				grid->addWidget(label, index, 1);
				grid->addWidget(button, index+1, 0);
				grid->addWidget(seq_labels[index], index+1, 1);
			} else {
				grid->addWidget(label, index-1, 4);
				grid->addWidget(button, index, 3);
				grid->addWidget(seq_labels[index], index, 4);
			}
		}

		seq->setLayout(grid);
		layout->addWidget(seq);
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	if(m_color->getAllowedType() & ColorRamp::RAMP_DIV) {
		div = new QGroupBox("Diverging Ramps");

		QGridLayout *grid = new QGridLayout();
		grid->setColumnMinimumWidth(2, 20);

		div_labels.resize(cm->getPredefinedTotal(ColorManager::DIVERGING));

		index = 0;
		for(int i = cm->getPredefinedStart(ColorManager::DIVERGING); i < cm->getPredefinedEnd(ColorManager::DIVERGING); i++, index++) {
			QRadioButton *button = new QRadioButton();
			buttongroup->addButton(button);
			connect(button, SIGNAL(clicked()), m_map, SLOT(map()));
			m_map->setMapping(button, i);
			div_labels[index] = new QLabel();
			div_labels[index]->setFixedSize(120,15);

			if(m_color->getRampID() == i) {
				button->setChecked(true);
				setSelectedRadio(i);
			}

			QLabel *label = new QLabel(cm->getPredefinedColorRampName(i));

			if(index % 2 == 0) {
				grid->addWidget(label, index, 1);
				grid->addWidget(button, index+1, 0);
				grid->addWidget(div_labels[index], index+1, 1);
			} else {
				grid->addWidget(label, index-1, 4);
				grid->addWidget(button, index, 3);
				grid->addWidget(div_labels[index], index, 4);
			}
		}

		div->setLayout(grid);
		layout->addWidget(div);
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	if(m_color->getAllowedType() & ColorRamp::RAMP_QUAL) {
		qual = new QGroupBox("Qualitative Ramps");

		QGridLayout *grid = new QGridLayout();
		grid->setColumnMinimumWidth(2, 20);

		qual_labels.resize(cm->getPredefinedTotal(ColorManager::QUALITATIVE));

		index = 0;
		for(int i = cm->getPredefinedStart(ColorManager::QUALITATIVE); i < cm->getPredefinedEnd(ColorManager::QUALITATIVE); i++, index++) {
			QRadioButton *button = new QRadioButton();
			buttongroup->addButton(button);
			connect(button, SIGNAL(clicked()), m_map, SLOT(map()));
			m_map->setMapping(button, i);
			qual_labels[index] = new QLabel();
			qual_labels[index]->setFixedSize(120,15);

			if(m_color->getRampID() == i) {
				button->setChecked(true);
				setSelectedRadio(i);
			}

			QLabel *label = new QLabel(cm->getPredefinedColorRampName(i));

			if(index % 2 == 0) {
				grid->addWidget(label, index, 1);
				grid->addWidget(button, index+1, 0);
				grid->addWidget(qual_labels[index], index+1, 1);
			} else {
				grid->addWidget(label, index-1, 4);
				grid->addWidget(button, index, 3);
				grid->addWidget(qual_labels[index], index, 4);
			}
		}

		qual->setLayout(grid);
		layout->addWidget(qual);
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// It is either exactly equal to RAMP_LINK or not a link at all
	if(m_color->getAllowedType() == ColorRamp::RAMP_LINK) {
		link = new QGroupBox("Linked Colors Ramps");

		QGridLayout *grid = new QGridLayout();
		grid->setColumnMinimumWidth(2, 20);

		// If it's a link, check which type of link (it can only be one type)
		if(m_color->getLinkType() == ColorRamp::LINK_REL2) colorType = ColorManager::LINK_REL2;
		else if(m_color->getLinkType() == ColorRamp::LINK_CON2) colorType = ColorManager::LINK_CON2;
		else if(m_color->getLinkType() == ColorRamp::LINK_REL5) colorType = ColorManager::LINK_REL5;
		link_labels.resize(cm->getPredefinedTotal(colorType));

		index = 0;
		for(int i = cm->getPredefinedStart(colorType); i < cm->getPredefinedEnd(colorType); i++, index++) {
			QRadioButton *button = new QRadioButton();
			buttongroup->addButton(button);
			connect(button, SIGNAL(clicked()), m_map, SLOT(map()));
			m_map->setMapping(button, i);
			link_labels[index] = new QLabel();
			link_labels[index]->setFixedSize(120,15);

			if(m_color->getRampID() == i) {
				button->setChecked(true);
				setSelectedRadio(i);
			}

			QLabel *label = new QLabel(cm->getPredefinedColorRampName(i));

			if(index % 2 == 0) {
				grid->addWidget(label, index, 1);
				grid->addWidget(button, index+1, 0);
				grid->addWidget(link_labels[index], index+1, 1);
			} else {
				grid->addWidget(label, index-1, 4);
				grid->addWidget(button, index, 3);
				grid->addWidget(link_labels[index], index, 4);
			}
		}

		link->setLayout(grid);
		layout->addWidget(link);
	}
	//---------------------------------------------------------------

	// Only let users change bins if it isn't a linked ramp
	if(m_color->getAllowedType() != ColorRamp::RAMP_LINK) {
		QCheckBox *useBins = new QCheckBox("Switch Ramps to Bins");
		if(m_bins) useBins->setChecked(true);
		connect(useBins, SIGNAL(clicked(bool)), this, SLOT(setUseBins(bool)));
		QLabel *spinLabel = new QLabel("Number of Bins:");
		QSpinBox *numBins = new QSpinBox();
		numBins->setRange(2,30);
		numBins->setValue(m_numBins);
		connect(numBins, SIGNAL(valueChanged(int)), this, SLOT(setNumBins(int)));

		QHBoxLayout *binsLayout = new QHBoxLayout();
		binsLayout->addWidget(useBins);
		QHBoxLayout *innerBinsLayout = new QHBoxLayout();
		innerBinsLayout->addWidget(spinLabel);
		innerBinsLayout->addWidget(numBins);
		binsLayout->addLayout(innerBinsLayout);
		layout->addLayout(binsLayout);
	}

	QPushButton *ok = new QPushButton("OK");
	connect(ok, SIGNAL(clicked()), this, SLOT(apply()));
	QPushButton *cancel = new QPushButton("Cancel");
	connect(cancel, SIGNAL(clicked()), this, SLOT(cancel()));

	QHBoxLayout *buttons = new QHBoxLayout();
	buttons->addWidget(ok);
	buttons->addWidget(cancel);
	layout->addLayout(buttons);

	setLayout(layout);

	drawColorBars();

	connect(m_map, SIGNAL(mapped(int)), this, SLOT(setSelectedRadio(int)));
}

ColorSelector::~ColorSelector() {
	SAFE_DELETE(m_singleColor);
	SAFE_DELETE(m_singleLabel);
}

void ColorSelector::drawColorBars() {
	ColorManager *cm = g_globals.colorManager;
	int index, i;

	// Same as above, check the allowed type and draw the corresponding color ramps
	if(m_color->getAllowedType() & ColorRamp::RAMP_SINGLE) {
		QPixmap p = m_singleColor->drawColorBar2(120, 20);
		m_singleLabel->setPixmap(p);
	}

	if(m_color->getAllowedType() & ColorRamp::RAMP_SEQ) {
		index = 0;
		for(i = cm->getPredefinedStart(ColorManager::SEQUENTIAL); i < cm->getPredefinedEnd(ColorManager::SEQUENTIAL); i++, index++) {
			ColorMap *c = new ColorMap(cm->getPredefinedColorRamp(i));
			if(m_bins) {
				c->setNumBins(m_numBins);
				c->swapRampMode();
			}
			QPixmap p = c->drawColorBar2(120, 20);
			seq_labels[index]->setPixmap(p);
			delete c;
		}
	}

	if(m_color->getAllowedType() & ColorRamp::RAMP_DIV) {
		index = 0;
		for(i = cm->getPredefinedStart(ColorManager::DIVERGING); i < cm->getPredefinedEnd(ColorManager::DIVERGING); i++, index++) {
			ColorMap *c = new ColorMap(cm->getPredefinedColorRamp(i));
			if(m_bins) {
				c->setNumBins(m_numBins);
				c->swapRampMode();
			}
			QPixmap p = c->drawColorBar2(120, 20);
			div_labels[index]->setPixmap(p);
			delete c;
		}
	}

	if(m_color->getAllowedType() & ColorRamp::RAMP_QUAL) {
		index = 0;
		for(i = cm->getPredefinedStart(ColorManager::QUALITATIVE); i < cm->getPredefinedEnd(ColorManager::QUALITATIVE); i++, index++) {
			ColorMap *c = new ColorMap(cm->getPredefinedColorRamp(i));
			c->setNumBins(m_numBins);
			QPixmap p = c->drawColorBar2(120, 20);
			qual_labels[index]->setPixmap(p);
			delete c;
		}
	}

	if(m_color->getAllowedType() == ColorRamp::RAMP_LINK) {
		int colorType = -1;
		if(m_color->getLinkType() == ColorRamp::LINK_REL2) colorType = ColorManager::LINK_REL2;
		else if(m_color->getLinkType() == ColorRamp::LINK_CON2) colorType = ColorManager::LINK_CON2;
		else if(m_color->getLinkType() == ColorRamp::LINK_REL5) colorType = ColorManager::LINK_REL5;
		index = 0;
		for(i = cm->getPredefinedStart(colorType); i < cm->getPredefinedEnd(colorType); i++, index++) {
			ColorMap *c = new ColorMap(cm->getPredefinedColorRamp(i));
			QPixmap p = c->drawColorBar2(120, 20);
			link_labels[index]->setPixmap(p);
			delete c;
		}
	}
}

void ColorSelector::setSelectedRadio(int button) {
	m_selected = button;
}

void ColorSelector::getNewColor() {
	RGBt rgb(m_singleColor->getColor());
	QColor oldColor((int)(rgb.R*255), (int)(rgb.G*255), (int)(rgb.B*255));
	QColor newColor = QColorDialog::getColor(oldColor, this);

	if(!newColor.isValid()) return;

	RGBt temp(newColor.red()/255., newColor.green()/255., newColor.blue()/255.);
	m_singleColor->setColorRamp(new ColorRamp(temp.getColor()));

	QPixmap p = m_singleColor->drawColorBar2(120, 20);
	m_singleLabel->setPixmap(p);
}

void ColorSelector::apply() {
	if(m_selected == -1) {
		close();
		return;
	} else if (m_selected == ColorManager::TOTAL_RAMPS)
		m_color->setColorRamp(new ColorRamp(*(m_singleColor->getColorRamp())));
	else {
		ColorRamp *c = g_globals.colorManager->getPredefinedColorRamp(m_selected);
		c->setNumBins(m_numBins);
		if(m_bins)
			c->setRampMode(ColorRamp::BINS);
		m_color->setColorRamp(c);
	}
	m_colorCustom->drawColorMaps();
	m_colorCustom->swapOpenSelector(m_index);
	close();
	XmdvToolMainWnd* mainWnd = m_colorCustom->getMainWnd();
	// Refresh all displays, not just the active one
	// We don't know if a display is visible or not
	mainWnd->getViewManager()->refreshAllDisplays();
	m_colorCustom->activateWindow();
}

void ColorSelector::cancel() {
	m_colorCustom->swapOpenSelector(m_index);
	close();
}

void ColorSelector::setUseBins(bool checked) {
	m_bins = checked;
	drawColorBars();
}

void ColorSelector::setNumBins(int bins) {
	m_numBins = bins;
	drawColorBars();
}


void ColorSelector::closeEvent(QCloseEvent *event)
{
	m_colorCustom->setOpenSelector(m_index, false);
	event->accept();
}
