/*
 * HandleBrush.cpp
 *
 *  Created on: Aug 11, 2009
 *      Author: Zaixian Xie
 */

#include "HandleBrush.h"

#include "interface/sbb/SBBControl.h"
#include "main/XmdvToolMainWnd.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "pipeline/Pipeline.h"
#include "operator/sbb/HierDisplayInformation.h"

HandleBrush::HandleBrush(XmdvToolMainWnd* mainWnd){
	this->m_mainWnd = mainWnd;
}

HandleBrush::~HandleBrush() {
}

void HandleBrush::openSBBDialog() {
	ViewManager* vm= m_mainWnd->getViewManager();
	ViewWindow* win = vm->getActiveViewWindow();

	SBBControl *sbbDlg = new SBBControl(m_mainWnd,
			m_mainWnd->getPipelineManager(), win->getPipelineID());
	sbbDlg->show();
}
