/*
 * HandleView.cpp
 *
 *  Created on: Mar 1, 2010
 *      Author: Zaixian Xie
 */

#include "main/HandleView.h"

#include <QObject>
#include <QAction>
#include <QFontDialog>

#include "main/XmdvToolMainWnd.h"
#include "view/ViewManager.h"
#include "interface/viewlist/HandleViewList.h"
#include "interface/viewlist/ViewListDock.h"

HandleView::HandleView(XmdvToolMainWnd* mainWnd){
	this->m_mainWnd = mainWnd;
	m_handleViewList = new HandleViewList(mainWnd);
	m_handleViewList->initDock();
}

HandleView::~HandleView() {
}

void HandleView::updateDockWidgets() {
	m_handleViewList->updateWidgets();
}

QAction* HandleView::viewTreeToggleAction() {
	return m_handleViewList->m_dock->toggleViewAction();
}

void HandleView::changeFont() {
    bool ok;
    // Open the font dialog
    QFont newTextFont = QFontDialog::getFont(&ok, g_globals.textFont, m_mainWnd);
    if (ok) {
    	// If users clicked "Ok", change the system font
    	g_globals.textFont = newTextFont;
    	m_mainWnd->getViewManager()->refreshAllDisplays();
    }

}
