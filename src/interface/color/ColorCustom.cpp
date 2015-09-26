/*
 * ColorCustom.cpp
 *
 *  Created on: Jun 9, 2009
 *  Author: Jason Stasik
 */

#include "interface/color/ColorCustom.h"
#include "main/XmdvToolMainWnd.h"
#include "color/ColorManager.h"
#include "color/ColorMap.h"
#include "main/XmdvTool.h"
#include "interface/color/ColorSelector.h"
#include "pipeline/PipelineManager.h"
#include "view/ViewManager.h"
#include "view/auxiliary/Image.h"

#include <QGridLayout>
#include <QColor>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>

ColorCustom::ColorCustom(XmdvToolMainWnd *mainWnd) : QDialog(mainWnd) {
	m_mainWnd = mainWnd;
	setWindowTitle(tr("Color Management Controls"));

	ColorManager *cm = g_globals.colorManager;

	m_map = new QSignalMapper(this);

	QVBoxLayout *mainLayout = new QVBoxLayout();

	QGridLayout *layout = new QGridLayout();
	layout->setColumnMinimumWidth(2, 20);
	layout->setVerticalSpacing(15);

	pbutton_colorSelect.resize(ColorManager::TOTAL_COLORS - cm->m_uneditableColors);
	label_colorBars.resize(ColorManager::TOTAL_COLORS - cm->m_uneditableColors);
	m_openSelectors.assign(ColorManager::TOTAL_COLORS - cm->m_uneditableColors, false);

	for(int i = 0; i < ColorManager::TOTAL_COLORS - cm->m_uneditableColors; i++) {
		pbutton_colorSelect[i] = new QPushButton(cm->getColorName(i));
		pbutton_colorSelect[i]->setFixedSize(100, 20);
		connect(pbutton_colorSelect[i], SIGNAL(clicked()), m_map, SLOT(map()));
		m_map->setMapping(pbutton_colorSelect[i], i);

		label_colorBars[i] = new QLabel();
		label_colorBars[i]->setFixedSize(120, 15);

		if(i % 2 == 0) {
			layout->addWidget(pbutton_colorSelect[i], i, 0);
			layout->addWidget(label_colorBars[i], i, 1);
		} else {
			layout->addWidget(pbutton_colorSelect[i], i-1, 3);
			layout->addWidget(label_colorBars[i], i-1, 4);
		}
	}

	connect(m_map, SIGNAL(mapped(int)), this, SLOT(openColorSelector(int)));

	selectTheme = new QComboBox();
	selectTheme->setFixedSize(200, 20);
	selectTheme->addItem("Select a theme:");
	for(int i = 0; i < ColorManager::TOTAL_THEMES; i++)
		selectTheme->addItem(cm->getThemeName(i));

	connect(selectTheme, SIGNAL(currentIndexChanged(int)), this, SLOT(theme(int)));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(selectTheme);
	mainLayout->addLayout(layout);
	mainLayout->addLayout(hbox);

	setLayout(mainLayout);

	drawColorMaps();
}

ColorCustom::~ColorCustom() {
}

void ColorCustom::drawColorMaps() {
	ColorManager *cm = g_globals.colorManager;

	for(int i = 0; i < ColorManager::TOTAL_COLORS - cm->m_uneditableColors; i++) {
		ColorMap *c = cm->getColorMap(i);
		QPixmap p = c->drawColorBar2(120, 20);
		label_colorBars[i]->setPixmap(p);
	}
}

void ColorCustom::openColorSelector(int index) {
	ColorMap *c = g_globals.colorManager->getColorMap(index);

	if(c->getAllowedType() == ColorRamp::RAMP_SINGLE)
		getSingleColor(c);
	else {
		if(!m_openSelectors[index]) {
			ColorSelector *colorSelector = new ColorSelector(this, c, index);
			colorSelector->show();
			colorSelector->activateWindow();
			swapOpenSelector(index);
		}
	}
}

void ColorCustom::swapOpenSelector(int index) {
	/*
	 * m_openSelectors[index] cannot directly used
	 * in some bit manipulation because of the optimization
	 * for vector<bool>.  The following code is a safe replacement
	 * for m_openSelectors[index] ^= 1;
	 *
	 */
	if (m_openSelectors[index]==true) {
		m_openSelectors[index] = false;
	} else {
		m_openSelectors[index] = true;
	}
}

void ColorCustom::setOpenSelector(int index, bool value) {
	m_openSelectors[index] = value;
}

void ColorCustom::getSingleColor(ColorMap *c) {
	RGBt rgb(c->getColor());
	QColor oldColor((int)(rgb.R*255), (int)(rgb.G*255), (int)(rgb.B*255));
	QColor newColor = QColorDialog::getColor(oldColor, this);
	if(!newColor.isValid()) return;

	RGBt temp(newColor.red()/255., newColor.green()/255., newColor.blue()/255.);
	c->setColorRamp(new ColorRamp(temp.getColor()));

	m_mainWnd->getViewManager()->refreshAllDisplays();

	drawColorMaps();
}

void ColorCustom::theme(int theme) {
	if(theme == 0) return;
	g_globals.colorManager->theme(theme-1);
	selectTheme->setCurrentIndex(0);
	m_mainWnd->getViewManager()->refreshAllDisplays();
	drawColorMaps();
}
