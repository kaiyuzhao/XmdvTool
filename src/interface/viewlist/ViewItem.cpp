/*
 * ViewItem.cpp
 *
 *  Created on: Feb 23, 2010
 *      Author: Zaixian Xie
 */

#include "interface/viewlist/ViewItem.h"

#include <QStringList>
#include <QIcon>

#include "main/XmdvTool.h"
#include "interface/viewlist/HandleViewList.h"
#include "view/ViewWindow.h"

ViewItem::ViewItem(QTreeWidget * parent, const QString & str, ViewWindow* viewWin)
	: QTreeWidgetItem(parent, QStringList(str),  ViewItemType) {
	m_viewWindow = viewWin;
	// Set the icon of view item in terms of its vis type
	switch ( viewWin->getVisType() ) {
	case XmdvTool::VISTYPE_PA :
		setIcon ( 0, ICON_PA );
		break;
	case XmdvTool::VISTYPE_SC :
		setIcon ( 0, ICON_SC );
		break;
	case XmdvTool::VISTYPE_GL :
		setIcon ( 0, ICON_ST );
		break;
	case XmdvTool::VISTYPE_DI :
		setIcon ( 0, ICON_DI );
		break;
	case XmdvTool::VISTYPE_PI :
		setIcon ( 0, ICON_PI );
		break;
	}
}

ViewItem::~ViewItem() {
}

ViewWindow* ViewItem::getViewWindow() {
	return m_viewWindow;
}
