/*
 * XmdvToolMainWnd.h
 *
 *  Created on: Apr. 1, 2008
 *      Author: Zaixian Xie
 */

/*
 * class XmdvToolMainWnd is the main Window of
 * XmdvTool.  It is a derived class of QMainWindow.
 * Its constructor function will create some important
 * objects, such as
 * 	PipelineManager* pipelineManager;
 *  ViewManager* viewManager;
 *
 */
#ifndef XMDVTOOLMAINWND_H
#define XMDVTOOLMAINWND_H

#include <QtCore/QVariant>
#include <QtGui/QScrollArea>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QMdiArea>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QComboBox>

#include "main/XmdvTool.h"
#include "main/HandleFiles.h"
#include "help/Assistant.h"
#include "help/FindFileDialog.h"
#include "help/TextEdit.h"


class GLDisplay;
class HandleFiles;
class HandleView;
class HandleBrush;
class HandleTools;
class PipelineManager;
class ViewManager;
class ZoomLineEdit;
class HandleZoom;

class XmdvToolMainWnd : public QMainWindow
{
    Q_OBJECT

    // Make ViewManager and HandleView as the friend class of this class,
    // so it can update menu and toolbar in terms of user's action
    friend class ViewManager;
    friend class HandleView;

public:
	XmdvToolMainWnd(QWidget *parent);
    ~XmdvToolMainWnd();

private:
	//Managers to manage visualization pipeline and final visualization
	PipelineManager* m_pipelineManager;
	ViewManager* m_viewManager;

	HandleFiles *m_handleFiles;
	HandleView *m_handleView;
	HandleBrush *m_handleBrush;
	HandleTools *m_handleTools;
	HandleZoom *m_handleZoom;

	void initFileActions();
	void initViewActions();
	void initBrushActions();
	void initToolsActions();
	void initWindowActions();
	void initVisActions();
	void initHelpActions();

	void initToolBarTop();
	void initToolBarRight();
	void initStatusBar();

	void initMenuBar();
	void initFileMenu();
	void initViewMenu();
	void initBrushMenu();
	void initToolsMenu();
	void initWindowMenu();
	void initZoomActions();
	void initHelpMenu();

private slots:
    void about();
    void showHelp();
    //void aboutQt();

protected:
    void closeEvent(QCloseEvent *event);

	// set menu and tool bar items enabled or disabled depending on the current view
	// pID the pipelind ID corresponding to the current view
	// visType is the current visualization type
	void updateMenuToolbar(int pID, XmdvTool::VISTYPE visType);

	// this function is invoked when all data sets is closed and no active window exists
	// some of the buttons in the menu and tool bar are disabled
	void disableMenuToolbar();

private:
	//to display help
	Assistant *assistant;
	TextEdit *textViewer;

	// The actions in menu "File"
    QAction *action_Open;
    QAction *action_SaveImage;
    QAction *action_Exit;

    // The action under the meny "View"
    QAction *action_Font;

    // the action about MDI operation
    QAction *action_Close;
    QAction *action_CloseAll;
    QAction *action_Tile;
    QAction *action_Cascade;
    QAction *action_Next;
    QAction *action_Previous;
    QAction *action_Separator;

    // the help menu actions
    QAction *action_Assistant;
    QAction *action_About;
    QAction *action_AboutQT;

    //Brush action
    QAction *action_SBB;

    //tool actions
    //
    // The action to do manual dimension on/off/reorder
    QAction *action_DimManual;
    // The action to do automatic dimension on/off/reorder
    QAction *action_DimAuto;
    QAction *action_DimstackCustomization;
    QAction *action_PixelCustomization;
    QAction *action_ScatterplotCustom;
    QAction *action_GlyphCustom;
    QAction *action_ColorCustom;
    QAction *action_ColorStrategy;
    // Turn on dimension reduction mode
    QAction *action_DimReductModeOn;
    // Turn off dimension reduction mode
    QAction *action_DimReductModeOff;
    // Open the dimension reduction dialog
    QAction *action_DimReductDlg;

    // the actions to set different visualizations
    QActionGroup *visActionGroup;
    QAction *action_Pal;
    QAction *action_Scatter;
    QAction *action_Glyph;
    QAction *action_Stacking;
    QAction *action_Pixel;
    QAction *action_HierPal;
    QAction *action_HierScatter;
    QAction *action_HierGlyph;
    QAction *action_HierStacking;
    QAction *action_HierPixel;

    QMdiArea* m_mdiArea;

    QMenu *menu_File;
    QMenu *menu_View;
    QMenu *menu_Brush;
    QMenu *menu_Tools;
    QMenu *menu_Window;
    QMenu *menu_Help;

    QMenuBar *menubar;
    QToolBar *toolBarTop;
    QToolBar *toolBarRight;

    QComboBox *comboBox_ZoomDirection;
    QLabel*	label_ZoomX;
    QLabel*	label_ZoomY;
    QLabel*	label_ZoomDirection;
    ZoomLineEdit* lineEdit_ZoomX;
    ZoomLineEdit* lineEdit_ZoomY;

    // the actions to zoom in/out
    QAction *action_ZoomIn;
    QAction *action_ZoomOut;
    QAction *action_ZoomReset;


public:
    QStatusBar *statusBar;

public:
	PipelineManager* getPipelineManager();
	ViewManager* getViewManager();

	HandleTools *getHandleTools();

	// Do the same thing as the above function, but it automatically treats the
	// active view window as the current view
	void updateMenuToolbarDock();

	// open the initial datasets when XmdvTool is opened first time
	void openInitialDatasets();

	ZoomLineEdit* getLineEdit_ZoomX();
	ZoomLineEdit* getLineEdit_ZoomY();
	QComboBox* getComboBox_ZoomDirection();

	// set zoom factors when changing view window
	// read from GLDisplay
	// this is invoked by updateMenuToolbar(int pID, XmdvTool::VISTYPE visType);;
	void updateZoomFactors();

	//get GLDisplay of the current active view window
	GLDisplay* getGLDisplay();

};

#endif // XMDVTOOLMAINWND_H
