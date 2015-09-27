/*
 * XmdvToolMainWnd.cpp
 *
 *  Created on: Apr. 1, 2008
 *      Author: Zaixian Xie
 */

#include "main/XmdvToolMainWnd.h"
#include "main/XmdvTool.h"
#include "main/HandleFiles.h"
#include "main/HandleView.h"
#include "main/HandleBrush.h"
#include "main/HandleTools.h"
#include "main/HandleZoom.h"
#include "interface/viewlist/HandleViewList.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Pipeline.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/auxiliary/ZoomLineEdit.h"
#include "view/GLDisplay.h"

#include "main/HandleZoom.h"
#include "help/Assistant.h"
#include "help/FindFileDialog.h"

#include <QGLWidget>
#include <QtGui>
#include <QtOpenGL>
#include <QDir>
#include <QString>

XmdvToolMainWnd::XmdvToolMainWnd(QWidget *parent)
: QMainWindow(parent)
{
	//To display help in QT Assistant
	assistant = new Assistant;

	m_pipelineManager = new PipelineManager(this);

	m_viewManager = new ViewManager(this);

	if (this->objectName().isEmpty())
		this->setObjectName(QString::fromUtf8("XmdvToolMainWnd"));
	this->resize(1000, 600);
	this->setWindowIcon(ICON_PA);

	m_mdiArea = new QMdiArea(this);
	this->setCentralWidget(m_mdiArea);
	m_viewManager->setMdiArea(m_mdiArea);
	connect(m_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
			m_viewManager, SLOT(mdiSubWindowActivated(QMdiSubWindow*)) );

	m_handleFiles = new HandleFiles(this);
	m_handleView = new HandleView(this);
	m_handleBrush = new HandleBrush(this);
	m_handleTools = new HandleTools(this);
	m_handleZoom = new HandleZoom(this);

	initFileActions();
	initViewActions();
	initBrushActions();
	initToolsActions();
	initWindowActions();
	initVisActions();
	initZoomActions();
	initHelpActions();

	initMenuBar();
	initFileMenu();
	initViewMenu();
	initBrushMenu();
	initToolsMenu();
	initWindowMenu();
	initHelpMenu();

    initToolBarTop();
	initToolBarRight();
	initStatusBar();

	this->setMouseTracking(true);
}

XmdvToolMainWnd::~XmdvToolMainWnd()
{
	SAFE_DELETE(m_pipelineManager);
	SAFE_DELETE(m_viewManager);
	SAFE_DELETE(m_handleFiles);
	SAFE_DELETE(m_handleView);
	SAFE_DELETE(m_handleTools);
}

PipelineManager* XmdvToolMainWnd::getPipelineManager() {
	return this->m_pipelineManager;
}

ViewManager* XmdvToolMainWnd::getViewManager() {
	return this->m_viewManager;
}

HandleTools *XmdvToolMainWnd::getHandleTools(){
	return this->m_handleTools;
}

void XmdvToolMainWnd::initFileActions() {
	action_Open = new QAction(this);
	action_Open->setObjectName(QString::fromUtf8("action_Open"));
	action_Open->setText(tr("Open"));
	action_Open->setIcon(ICON_OPEN);
	action_Open->setShortcut(tr("Ctrl+F"));
	action_Open->setStatusTip(tr("Open file"));
	connect(action_Open, SIGNAL(triggered()), m_handleFiles, SLOT(openFile()));

	action_SaveImage = new QAction(this);
	action_SaveImage->setText(tr("Save Image"));
	action_SaveImage->setObjectName(QString::fromUtf8("action_SaveImage"));
	action_SaveImage->setStatusTip(tr("Save image as file"));
	connect(action_SaveImage, SIGNAL(triggered()), m_handleFiles, SLOT(saveImage()));

	action_Exit = new QAction(this);
	action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
	action_Exit->setText(tr("Exit"));
	action_Exit->setShortcut(tr("Ctrl+Q"));
	action_Exit->setStatusTip(tr("Exit XmdvTool"));
	connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));
}

void XmdvToolMainWnd::initViewActions() {

	action_Font = new QAction(this);
	action_Font->setText(tr("Font"));
	action_Font->setObjectName(QString::fromUtf8("action_Font"));
	connect(action_Font, SIGNAL(triggered()), m_handleView, SLOT(changeFont()));
}

void XmdvToolMainWnd::initBrushActions() {
	action_SBB = new QAction(this);
	action_SBB->setObjectName(QString::fromUtf8("action_SBB"));
	action_SBB->setText(tr("Structure-Based Brush for Hier Displays"));
	action_SBB->setIcon(ICON_SBB);
	action_SBB->setStatusTip(tr("Structure-Based Brush"));
	connect(action_SBB, SIGNAL(triggered()), m_handleBrush, SLOT(openSBBDialog()));
}


void XmdvToolMainWnd::initToolsActions() {
	action_DimManual = new QAction(this);
	action_DimManual->setObjectName(QString::fromUtf8("action_DimManual"));
	action_DimManual->setText(tr("Manual Dimension On/Off/Reorder"));
	action_DimManual->setStatusTip(tr("Manual Dimension On/Off/Reorder"));
	connect(action_DimManual, SIGNAL(triggered()), m_handleTools, SLOT(showDimManualDlg()));

	action_DimAuto = new QAction(this);
	action_DimAuto->setObjectName(QString::fromUtf8("action_DimAuto"));
	action_DimAuto->setText(tr("Automated Dimension Reorder"));
	action_DimAuto->setStatusTip(tr("Automated Dimension Reorder"));
	connect(action_DimAuto, SIGNAL(triggered()), m_handleTools, SLOT(showDimAutoDlg()));

	action_ColorCustom = new QAction(this);
	action_ColorCustom->setObjectName(QString::fromUtf8("action_ColorCustom"));
	action_ColorCustom->setText(tr("Color Customization"));
	action_ColorCustom->setStatusTip(tr("Color Customization"));
	connect(action_ColorCustom, SIGNAL(triggered()), m_handleTools, SLOT(showColorCustom()));

	action_ColorStrategy = new QAction(this);
	action_ColorStrategy->setObjectName(QString::fromUtf8("action_ColorStrategy"));
	action_ColorStrategy->setText(tr("Color Strategy"));
	action_ColorStrategy->setStatusTip(tr("Color Strategy"));
	connect(action_ColorStrategy, SIGNAL(triggered()), m_handleTools, SLOT(showColorStrategy()));

	action_ScatterplotCustom = new QAction(this);
	action_ScatterplotCustom->setObjectName(QString::fromUtf8("action_ScatterplotCustom"));
	action_ScatterplotCustom->setText(tr("Scatterplot Customization"));
	action_ScatterplotCustom->setStatusTip(tr("Scatterplot Customization for Flat Display"));
	connect(action_ScatterplotCustom, SIGNAL(triggered()), m_handleTools, SLOT(ShowScatterplotCustom()));

	action_GlyphCustom = new QAction(this);
	action_GlyphCustom->setObjectName(QString::fromUtf8("action_GlyphCustom"));
	action_GlyphCustom->setText(tr("Glyph Customization"));
	action_GlyphCustom->setStatusTip(tr("Glyph Customization for Flat Display"));
	connect(action_GlyphCustom, SIGNAL(triggered()), m_handleTools, SLOT(showGlyphCustom()));

	action_DimstackCustomization = new QAction(this);
	action_DimstackCustomization->setObjectName(QString::fromUtf8("action_DimstackCustomization"));
	action_DimstackCustomization->setText(tr("Dimension Stacking Customization"));
	action_DimstackCustomization->setStatusTip(tr("Dimensional Stacking Customization for Flat Display"));
	connect(action_DimstackCustomization, SIGNAL(triggered()), m_handleTools, SLOT(openDimstackCustomization()));

	action_PixelCustomization = new QAction(this);
	action_PixelCustomization->setObjectName(QString::fromUtf8("action_PixelCustomization"));
	action_PixelCustomization->setText(tr("Pixel-Oriented Customization"));
	action_PixelCustomization->setStatusTip(tr("Pixel-Oriented Customization for Flat Display"));
	connect(action_PixelCustomization, SIGNAL(triggered()), m_handleTools, SLOT(openPixelCustomization()));

	action_DimReductModeOn = new QAction(this);
	action_DimReductModeOn->setObjectName(QString::fromUtf8("action_DimReductMode"));
	action_DimReductModeOn->setText(tr("Turn on Dimension Reduction"));
	action_DimReductModeOn->setStatusTip(tr("Turn on Dimension Reduction Mode"));
	connect(action_DimReductModeOn, SIGNAL(triggered()), m_handleTools, SLOT(turnOnDimReductMode()));

	action_DimReductModeOff = new QAction(this);
	action_DimReductModeOff->setObjectName(QString::fromUtf8("action_DimReductModeOff"));
	action_DimReductModeOff->setText(tr("Turn off Dimension Reduction"));
	action_DimReductModeOff->setStatusTip(tr("Turn off Dimension Reduction Mode"));
	connect(action_DimReductModeOff, SIGNAL(triggered()), m_handleTools, SLOT(turnOffDimReductMode()));

	action_DimReductDlg = new QAction(this);
	action_DimReductDlg->setObjectName(QString::fromUtf8("action_DimReductDlg"));
	action_DimReductDlg->setText(tr("Dimension Reduction Dialog"));
	action_DimReductDlg->setStatusTip(tr("Open Dimension Reduction Dialog"));
	connect(action_DimReductDlg, SIGNAL(triggered()), m_handleTools, SLOT(openDimReductDlg()));
}

void XmdvToolMainWnd::initZoomActions() {
	comboBox_ZoomDirection = new QComboBox();
	comboBox_ZoomDirection->addItem("X + Y", 0);
	comboBox_ZoomDirection->addItem("X", 1);
	comboBox_ZoomDirection->addItem("Y", 2);

	connect(comboBox_ZoomDirection, SIGNAL(currentIndexChanged(int)), m_handleZoom, SLOT(setZoomDirection(int)));

	label_ZoomX = new QLabel(QString(" Zoom X: "));
	label_ZoomY = new QLabel(QString(" Zoom Y: "));;
	label_ZoomDirection = new QLabel(QString(" Zoom Direction: "));

	lineEdit_ZoomX = new ZoomLineEdit(this, QString("100"));
	lineEdit_ZoomY = new ZoomLineEdit(this, QString("100"));

	connect(lineEdit_ZoomX, SIGNAL(returnPressed()), m_handleZoom, SLOT(setZoomFactors()));
	connect(lineEdit_ZoomY, SIGNAL(returnPressed()), m_handleZoom, SLOT(setZoomFactors()));

	action_ZoomIn = new QAction(this);
	action_ZoomIn->setObjectName(QString::fromUtf8("action_ZoomIn"));
	action_ZoomIn->setText(tr("Zoom In"));
	action_ZoomIn->setIcon(ICON_ZOOMIN);
	action_ZoomIn->setShortcut(tr("Ctrl++"));
	action_ZoomIn->setStatusTip(tr("Zoom In"));
	connect(action_ZoomIn, SIGNAL(triggered()), m_handleZoom, SLOT(zoomIn()));

	action_ZoomOut = new QAction(this);
	action_ZoomOut->setObjectName(QString::fromUtf8("action_ZoomOut"));
	action_ZoomOut->setText(tr("Zoom Out"));
	action_ZoomOut->setIcon(ICON_ZOOMOUT);
	action_ZoomOut->setShortcut(tr("Ctrl+-"));
	action_ZoomOut->setStatusTip(tr("Zoom Out"));
	connect(action_ZoomOut, SIGNAL(triggered()), m_handleZoom, SLOT(zoomOut()));

	action_ZoomReset = new QAction(this);
	action_ZoomReset->setObjectName(QString::fromUtf8("action_ZoomReset"));
	action_ZoomReset->setText(tr("Fit Window Size"));
	action_ZoomReset->setIcon(ICON_RESETSIZE);
	action_ZoomReset->setShortcut(tr("Ctrl+r"));
	action_ZoomReset->setStatusTip(tr("Fit Window Size"));
	connect(action_ZoomReset, SIGNAL(triggered()), m_handleZoom, SLOT(zoomReset()));
}


void XmdvToolMainWnd::initWindowActions() {

	action_Close = new QAction(tr("Cl&ose"), this);
	action_Close->setShortcut(tr("Ctrl+F4"));
	action_Close->setStatusTip(tr("Close the active window"));
	connect(action_Close, SIGNAL(triggered()), m_mdiArea, SLOT(closeActiveSubWindow()));

	action_CloseAll = new QAction(tr("Close &All"), this);
	action_CloseAll->setStatusTip(tr("Close all the windows"));
	connect(action_CloseAll, SIGNAL(triggered()), m_mdiArea, SLOT(closeAllSubWindows()));

	action_Tile = new QAction(tr("&Tile"), this);
	action_Tile->setStatusTip(tr("Tile the windows"));
	connect(action_Tile, SIGNAL(triggered()), m_mdiArea, SLOT(tileSubWindows()));

	action_Cascade = new QAction(tr("&Cascade"), this);
	action_Cascade->setStatusTip(tr("Cascade the windows"));
	connect(action_Cascade, SIGNAL(triggered()), m_mdiArea, SLOT(cascadeSubWindows()));

	action_Next = new QAction(tr("Ne&xt"), this);
	action_Next->setShortcut(tr("Ctrl+F6"));
	action_Next->setStatusTip(tr("Move the focus to the next window"));
	connect(action_Next, SIGNAL(triggered()), m_mdiArea, SLOT(activateNextSubWindow()));

	action_Previous = new QAction(tr("Pre&vious"), this);
	action_Previous->setShortcut(tr("Ctrl+Shift+F6"));
	action_Previous->setStatusTip(tr("Move the focus to the previous window"));
	connect(action_Previous, SIGNAL(triggered()), m_mdiArea, SLOT(activatePreviousSubWindow()));

	action_Separator = new QAction(this);
	action_Separator->setSeparator(true);

}



//To Display Help Menu

void XmdvToolMainWnd::initHelpMenu(){
	menu_Help = new QMenu(tr("&Help"),this);
	menubar->addMenu(menu_Help);

	menu_Help->addAction(action_Assistant);
	menu_Help->addAction(action_About);
	menu_Help->addAction(action_AboutQT);
}

void XmdvToolMainWnd::initHelpActions(){
		action_Assistant = new QAction(tr("Help Contents"), this);
		action_Assistant->setShortcut(QKeySequence::HelpContents);
	    connect(action_Assistant, SIGNAL(triggered()), this, SLOT(showHelp()));

	     action_About = new QAction(tr("&About"), this);
	     connect(action_About, SIGNAL(triggered()), this, SLOT(about()));

	     action_AboutQT = new QAction(tr("About &Qt"), this);
	     connect(action_AboutQT, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void XmdvToolMainWnd::about()
{
     QMessageBox::about(this, tr("About XmvTool"),
                        tr("XmdvTool is a public-domain software package for the interactive visual exploration of\n"
                           "multivariate data sets.  For more information, please visit:\n"
                           "http://davis.wpi.edu/~xmdv"));
}

void XmdvToolMainWnd::showHelp()
 {
     assistant->showHelp("help/index.html");
 }

void XmdvToolMainWnd::closeEvent(QCloseEvent *)
{
    delete assistant;
}

void XmdvToolMainWnd::initVisActions() {

	// Initialize the action for flat display
	action_Pal = new QAction(this);
	action_Pal->setObjectName(QString::fromUtf8("action_pal"));
	action_Pal->setIcon(ICON_PA);
	action_Pal->setToolTip(tr("Parallel coordinates"));
	action_Pal->setStatusTip(tr("Parallel coordinates"));
	action_Pal->setCheckable(true);
	connect(action_Pal, SIGNAL(triggered()), m_viewManager, SLOT(changeToParallel()));

	action_Scatter = new QAction(this);
	action_Scatter->setObjectName(QString::fromUtf8("action_Scatter"));
	action_Scatter->setIcon(ICON_SC);
	action_Scatter->setToolTip(tr("Scatterplot matrices"));
	action_Scatter->setStatusTip(tr("Scatterplot matrices"));
	action_Scatter->setCheckable(true);
	connect(action_Scatter, SIGNAL(triggered()), m_viewManager, SLOT(changeToScatter()));

	action_Glyph = new QAction(this);
	action_Glyph->setObjectName(QString::fromUtf8("action_Glyph"));
	action_Glyph->setIcon(ICON_ST);
	action_Glyph->setToolTip(tr("Star glyphs"));
	action_Glyph->setStatusTip(tr("Star glyphs"));
	action_Glyph->setCheckable(true);
	connect(action_Glyph, SIGNAL(triggered()), m_viewManager, SLOT(changeToGlyph()));

	action_Stacking = new QAction(this);
	action_Stacking->setObjectName(QString::fromUtf8("action_Stacking"));
	action_Stacking->setIcon(ICON_DI);
	action_Stacking->setToolTip(tr("Dimension Stacking"));
	action_Stacking->setStatusTip(tr("Dimension Stacking"));
	action_Stacking->setCheckable(true);
	connect(action_Stacking, SIGNAL(triggered()), m_viewManager, SLOT(changeToDimstack()));

	action_Pixel = new QAction(this);
	action_Pixel->setObjectName(QString::fromUtf8("action_Pixel"));
	action_Pixel->setIcon(ICON_PI);
	action_Pixel->setToolTip(tr("Pixel-oriented display"));
	action_Pixel->setStatusTip(tr("Pixel-oriented display"));
	action_Pixel->setCheckable(true);
	connect(action_Pixel, SIGNAL(triggered()), m_viewManager, SLOT(changeToPixel()));

	// Initialize the action for hierarchical display
    action_HierPal = new QAction(this);
    action_HierPal->setObjectName(QString::fromUtf8("action_HierPal"));
    action_HierPal->setIcon(ICON_HPA);
    action_HierPal->setToolTip(tr("Hierarchical Parallel coordinates"));
    action_HierPal->setStatusTip(tr("Hierarchical Parallel coordinates"));
    action_HierPal->setCheckable(true);
	connect(action_HierPal, SIGNAL(triggered()), m_viewManager, SLOT(changeToHierParallel()));

	action_HierScatter = new QAction(this);
	action_HierScatter->setObjectName(QString::fromUtf8("action_HierScatter"));
	action_HierScatter->setIcon(ICON_HSC);
	action_HierScatter->setToolTip(tr("Hierarchical Scatterplot matrices"));
	action_HierScatter->setStatusTip(tr("Hierarchical Scatterplot matrices"));
	action_HierScatter->setCheckable(true);
	connect(action_HierScatter, SIGNAL(triggered()), m_viewManager, SLOT(changeToHierScatter()));


	action_HierGlyph = new QAction(this);
	action_HierGlyph->setObjectName(QString::fromUtf8("action_HierGlyph"));
	action_HierGlyph->setIcon(ICON_HST);
	action_HierGlyph->setToolTip(tr("Hierarchical Star glyphs"));
	action_HierGlyph->setStatusTip(tr("Hierarchical Star glyphs"));
	action_HierGlyph->setCheckable(true);
	connect(action_HierGlyph, SIGNAL(triggered()), m_viewManager, SLOT(changeToHierGlyph()));


	action_HierStacking = new QAction(this);
	action_HierStacking->setObjectName(QString::fromUtf8("action_HierStacking"));
	action_HierStacking->setIcon(ICON_HDI);
	action_HierStacking->setToolTip(tr("Hierarchical Dimension Stacking"));
	action_HierStacking->setStatusTip(tr("Hierarchical Dimension Stacking"));
	action_HierStacking->setCheckable(true);
	connect(action_HierStacking, SIGNAL(triggered()), m_viewManager, SLOT(changeToHierStacking()));

	action_HierPixel = new QAction(this);
	action_HierPixel->setObjectName(QString::fromUtf8("action_HierPixel"));
	action_HierPixel->setIcon(ICON_HPI);
	action_HierPixel->setToolTip(tr("Hierarchical Pixel-oriented display"));
	action_HierPixel->setStatusTip(tr("Hierarchical Pixel-oriented display"));
	action_HierPixel->setCheckable(true);
	connect(action_HierPixel, SIGNAL(triggered()), m_viewManager, SLOT(changeToHierPixel()));

	visActionGroup = new QActionGroup(this);
	visActionGroup->addAction(action_Pal);
	visActionGroup->addAction(action_Scatter);
	visActionGroup->addAction(action_Glyph);
	visActionGroup->addAction(action_Stacking);
	visActionGroup->addAction(action_Pixel);
	visActionGroup->addAction(action_HierPal);
	visActionGroup->addAction(action_HierScatter);
	visActionGroup->addAction(action_HierGlyph);
	visActionGroup->addAction(action_HierStacking);
	visActionGroup->addAction(action_HierPixel);
	action_Pal->setChecked(true);
}

void XmdvToolMainWnd::initFileMenu() {
	menu_File = new QMenu(tr("&File"), this);
	menubar->addMenu(menu_File);

	menu_File->setObjectName(QString::fromUtf8("menu_File"));
	menu_File->addAction(action_Open);
	menu_File->addAction(action_SaveImage);
	menu_File->addSeparator();
	menu_File->addAction(action_Exit);
}

void XmdvToolMainWnd::initViewMenu() {
	menu_View = new QMenu(tr("&View"), this);
	menubar->addMenu(menu_View);

	menu_View->setObjectName(QString::fromUtf8("menu_View"));
	menu_View->addAction( m_handleView->viewTreeToggleAction() );
	menu_View->addSeparator();
	menu_View->addAction(action_Font);
}

void XmdvToolMainWnd::initBrushMenu() {
	menu_Brush = new QMenu(tr("&Brushes"), this);
	menubar->addMenu(menu_Brush);

	menu_Brush->setObjectName(QString::fromUtf8("menu_Brush"));
	menu_Brush->addAction(action_SBB);
}


void XmdvToolMainWnd::initToolsMenu() {
	menu_Tools = new QMenu(tr("&Tools"), this);
	menubar->addMenu(menu_Tools);

	menu_Tools->setObjectName(QString::fromUtf8("menu_Tools"));

	// Add the actions for dimension on/off/reorder
	menu_Tools->addAction(action_DimManual);
	menu_Tools->addAction(action_DimAuto);
	menu_Tools->addSeparator();
	// Add the actions for color strategy
	menu_Tools->addAction(action_ColorCustom);
	menu_Tools->addAction(action_ColorStrategy);
	menu_Tools->addSeparator();
	// Add the actions for color strategy
	menu_Tools->addAction(action_ScatterplotCustom);
	menu_Tools->addAction(action_GlyphCustom);
	menu_Tools->addAction(action_DimstackCustomization);
	menu_Tools->addAction(action_PixelCustomization);
	// Add the action for dimension reduction
	menu_Tools->addSeparator();
	menu_Tools->addAction(action_DimReductModeOn);
	menu_Tools->addAction(action_DimReductModeOff);
	menu_Tools->addAction(action_DimReductDlg);
}


void XmdvToolMainWnd::initWindowMenu() {
	menu_Window = new QMenu(tr("&Window"), this);
	menubar->addMenu(menu_Window);

	menu_Window->addAction(action_Close);
	menu_Window->addAction(action_CloseAll);
	menu_Window->addSeparator();
	menu_Window->addAction(action_Tile);
	menu_Window->addAction(action_Cascade);
	menu_Window->addSeparator();
	menu_Window->addAction(action_Next);
	menu_Window->addAction(action_Previous);
	menu_Window->addAction(action_Separator);
}

void XmdvToolMainWnd::initMenuBar() {
	menubar = new QMenuBar(this);
	menubar->setObjectName(QString::fromUtf8("menubar"));
	menubar->setGeometry(QRect(0, 0, 800, 19));

	this->setMenuBar(menubar);
}


void XmdvToolMainWnd::initToolBarTop() {
	//set attributes for toolBarTop
	toolBarTop = new QToolBar(this);
	toolBarTop->setObjectName(QString::fromUtf8("toolBarTop"));
	toolBarTop->setMovable(false);
	toolBarTop->setFloatable(false);

	//add actions for toolBarTop
	toolBarTop->addAction(action_Open);
	toolBarTop->addAction(action_SBB);

	toolBarTop->addWidget(label_ZoomX);
	toolBarTop->addWidget(lineEdit_ZoomX);

	toolBarTop->addWidget(label_ZoomY);
	toolBarTop->addWidget(lineEdit_ZoomY);

	toolBarTop->addWidget(label_ZoomDirection);
	toolBarTop->addWidget(comboBox_ZoomDirection);

	//add toolbar to the main window
	this->addToolBar(Qt::TopToolBarArea, toolBarTop);

	//add zoom actions for toolBarTop
	toolBarTop->addAction(action_ZoomOut);
	toolBarTop->addAction(action_ZoomReset);
	toolBarTop->addAction(action_ZoomIn);
}

void XmdvToolMainWnd::initToolBarRight() {
	//set attributes for toolBarRight
	toolBarRight = new QToolBar(this);
	toolBarRight->setObjectName(QString::fromUtf8("toolBarRight"));
	toolBarRight->setMovable(false);
	toolBarRight->setFloatable(false);

	//add actions for toolBarRight
	toolBarRight->addAction(action_Pal);
	toolBarRight->addAction(action_Scatter);
	toolBarRight->addAction(action_Glyph);
	toolBarRight->addAction(action_Stacking);
	toolBarRight->addAction(action_Pixel);
	toolBarRight->addSeparator();
	toolBarRight->addAction(action_HierPal);
	toolBarRight->addAction(action_HierScatter);
	toolBarRight->addAction(action_HierGlyph);
	toolBarRight->addAction(action_HierStacking);
	toolBarRight->addAction(action_HierPixel);

	//add toolbar to the main window
	this->addToolBar(Qt::RightToolBarArea, toolBarRight);
}

void XmdvToolMainWnd::initStatusBar() {
	statusBar = new QStatusBar(this);
	statusBar->setObjectName(QString::fromUtf8("statusBar"));
	this->setStatusBar(statusBar);
	statusBar->showMessage(tr("Ready"));

}

void XmdvToolMainWnd::updateMenuToolbarDock() {
	ViewManager* vm = getViewManager();
	ViewWindow* win = vm->getActiveViewWindow();

	// If an active view window exist, we do update menu and tool bar
	if (win!=0) {
		// get the visualization type for the current view
		XmdvTool::VISTYPE visType =  win->getVisType();
		int pID = win->getPipelineID();
		updateMenuToolbar(pID, visType);
	}else{
		// if no active view window exist, disable some of the menu and tool bar buttons
		disableMenuToolbar();
	}

	m_handleView->updateDockWidgets();
}

// Enable or disable those menu items which are only available for
// a part of visualization types
void XmdvToolMainWnd::updateMenuToolbar(int pID, XmdvTool::VISTYPE visType) {
	// Enable or disable menu and tool bar items in terms of the current pipeline type.
	XmdvTool::PLTYPE plType = m_pipelineManager->getPipeline(pID)->getPipelineType();

	// enable the tool bar and menu buttons
	action_Pal->setEnabled(true);
	action_Scatter->setEnabled(true);
	action_Glyph->setEnabled(true);
	action_Stacking->setEnabled(true);
	action_Pixel->setEnabled(true);
	action_HierPal->setEnabled(true);
	action_HierScatter->setEnabled(true);
	action_HierGlyph->setEnabled(true);
	action_HierStacking->setEnabled(true);
	action_HierPixel->setEnabled(true);

	action_ColorCustom->setEnabled(true);
	action_ColorStrategy->setEnabled(true);

	switch (plType) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		// Disable structure-based brush
		action_SBB->setEnabled(false);
		// Allow to turn on dimension reduction
		action_DimReductModeOn->setEnabled(true);
		action_DimReductModeOff->setEnabled(false);
		action_DimReductDlg->setEnabled(false);
		// Enable the dimension on/off/reordering
	    action_DimManual->setEnabled(true);
	    action_DimAuto->setEnabled(true);
		break;
	case XmdvTool::PLTYPE_DIMR :
		// Disable structure-based brush
		action_SBB->setEnabled(false);
		action_DimReductModeOn->setEnabled(false);
		action_DimReductModeOff->setEnabled(true);
		action_DimReductDlg->setEnabled(true);
		// Disable the dimension on/off/reordering
	    action_DimManual->setEnabled(false);
	    action_DimAuto->setEnabled(false);
		break;
	case XmdvTool::PLTYPE_SBB :
		// Enable structure-based brush
		action_SBB->setEnabled(true);
		// Disable dimension reduction mode
		action_DimReductModeOn->setEnabled(false);
		action_DimReductModeOff->setEnabled(false);
		action_DimReductDlg->setEnabled(false);
		// Enable the dimension on/off/reordering
	    action_DimManual->setEnabled(true);
	    action_DimAuto->setEnabled(true);
		break;
	}

	action_ScatterplotCustom->setEnabled(false);
	action_DimstackCustomization->setEnabled(false);
	action_PixelCustomization->setEnabled(false);
	action_GlyphCustom->setEnabled(false);

	switch (visType) {
	case XmdvTool::VISTYPE_PA : //= 0, Parallel coordinates
		if (plType==XmdvTool::PLTYPE_FLATNORMAL ||
				plType==XmdvTool::PLTYPE_DIMR ) {
			action_Pal->setChecked(true);
		} else {
			action_HierPal->setChecked(true);
		}
		break;
	case XmdvTool::VISTYPE_SC : //= 1, Scatterplot matrices
		if (plType==XmdvTool::PLTYPE_FLATNORMAL ||
				plType==XmdvTool::PLTYPE_DIMR ) {
			action_Scatter->setChecked(true);
		} else {
			action_HierScatter->setChecked(true);
		}
		action_ScatterplotCustom->setEnabled(true);
		break;
	case XmdvTool::VISTYPE_GL : //= 2, Glyphs
		if (plType==XmdvTool::PLTYPE_FLATNORMAL ||
				plType==XmdvTool::PLTYPE_DIMR ) {
			action_Glyph->setChecked(true);
		} else {
			action_HierGlyph->setChecked(true);
		}
		action_GlyphCustom->setEnabled(true);
		break;
	case XmdvTool::VISTYPE_DI : //= 3, Dimension Stacking
		if (plType==XmdvTool::PLTYPE_FLATNORMAL ||
				plType==XmdvTool::PLTYPE_DIMR ) {
			action_Stacking->setChecked(true);
		} else {
			action_HierStacking->setChecked(true);
		}
		action_DimstackCustomization->setEnabled(true);;
		break;
	case XmdvTool::VISTYPE_PI : //= 4, Pixel-oriented display
		if (plType==XmdvTool::PLTYPE_FLATNORMAL ||
				plType==XmdvTool::PLTYPE_DIMR ) {
			action_Pixel->setChecked(true);
		} else {
			action_HierPixel->setChecked(true);
		}
		if ( plType==XmdvTool::PLTYPE_FLATNORMAL ) {
			action_PixelCustomization->setEnabled(true);
		} else {
			action_PixelCustomization->setEnabled(false);
		}
		break;
	};

	updateZoomFactors();
}


// this function is invoked when all data sets is closed and no active window exists
// some of the buttons in the menu and tool bar are disabled
void XmdvToolMainWnd::disableMenuToolbar(){
	action_SBB->setEnabled(false);
	action_DimReductModeOn->setEnabled(false);
	action_DimReductModeOff->setEnabled(false);
	action_DimReductDlg->setEnabled(false);
	action_DimManual->setEnabled(false);
	action_DimAuto->setEnabled(false);

	action_Pal->setEnabled(false);
	action_Scatter->setEnabled(false);
	action_Glyph->setEnabled(false);
	action_Stacking->setEnabled(false);
	action_Pixel->setEnabled(false);
	action_HierPal->setEnabled(false);
	action_HierScatter->setEnabled(false);
	action_HierGlyph->setEnabled(false);
	action_HierStacking->setEnabled(false);
	action_HierPixel->setEnabled(false);

	action_ColorCustom->setEnabled(false);
	action_ColorStrategy->setEnabled(false);

	action_ScatterplotCustom->setEnabled(false);
	action_DimstackCustomization->setEnabled(false);
	action_PixelCustomization->setEnabled(false);
	action_GlyphCustom->setEnabled(false);

}

ZoomLineEdit* XmdvToolMainWnd::getLineEdit_ZoomX(){
	return lineEdit_ZoomX;
}
ZoomLineEdit* XmdvToolMainWnd::getLineEdit_ZoomY(){
	return lineEdit_ZoomY;
}

QComboBox* XmdvToolMainWnd::getComboBox_ZoomDirection(){
	return comboBox_ZoomDirection;
}

void XmdvToolMainWnd::updateZoomFactors(){
	this->m_handleZoom->updateZoomBarInfoFromGLDisplay();

}

GLDisplay* XmdvToolMainWnd::getGLDisplay(){
	if (getViewManager() == 0) {
		return 0;
	}
	if (getViewManager()->getActiveViewWindow() == 0) {
		return 0 ;
	}
	if (getViewManager()->getActiveViewWindow()->getGLDisplay() == 0) {
		return 0 ;
	}

	return getViewManager()->getActiveViewWindow()->getGLDisplay();
}

void XmdvToolMainWnd::openInitialDatasets() {
	// Get the current folder
	QDir currentDir = QDir::current();
	currentDir.cd(QString("data"));

#ifdef WIN32
	// Under win32, we only need to open one dataset
	// open the default dataset 1
	QString defaultFileName1(g_globals.default_file_name1.c_str());
	if (currentDir.exists (defaultFileName1) ) {
		QString filePath(
				currentDir.absoluteFilePath(defaultFileName1) );
		getPipelineManager()->openFile( filePath, true, (XmdvTool::VISTYPE)XmdvTool::DEFAULT_VISTYPE );
	}

#else
	// Under unix, we need to open two datasets and use star glyphs as
	// the first view, because a strange bug does not allow us to display text
	// for the first QGLWidget

	// open the default dataset 1
	QString defaultFileName1(g_globals.default_file_name1.c_str());
	if (currentDir.exists (defaultFileName1) ) {
		QString filePath(
				currentDir.absoluteFilePath(defaultFileName1) );
		getPipelineManager()->openFile( filePath, true, XmdvTool::VISTYPE_GL );
	}
	// open the default dataset 2
	QString defaultFileName2(g_globals.default_file_name2.c_str());
	if (currentDir.exists (defaultFileName2) ) {
		QString filePath(
				currentDir.absoluteFilePath(defaultFileName2) );
		getPipelineManager()->openFile( filePath, true, XmdvTool::VISTYPE_GL );
	}
#endif
}
