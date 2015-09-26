/*
 * ViewListDock.cpp
 *
 *  Created on: Feb 24, 2010
 *      Author: Zaixian Xie
 */

#include "interface/viewlist/ViewListDock.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QListWidget>
#include <QHeaderView>
#include <QSplitter>

#include "interface/viewlist/HandleViewList.h"

ViewListDock::ViewListDock(QWidget* parent, HandleViewList* handleViewList)
	: QWidget(parent) {
	m_handleViewList = handleViewList;
	createWidgets();

	connect( m_viewTree, SIGNAL( currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem *)  ),
			handleViewList, SLOT( currentItemChanged( QTreeWidgetItem * , QTreeWidgetItem *) ) );

	connect( m_btDelete, SIGNAL( clicked()  ), handleViewList, SLOT( deleteNode() ) );
}

ViewListDock::~ViewListDock() {
}

void ViewListDock::createWidgets() {

	// Initialize the actions for adding new views
	initActions();

	// Initialize the menu for adding new views
	initMenu();

	// Create the tree widget
	m_viewTree = new QTreeWidget(this);
	m_viewTree->setColumnCount(1);
	m_viewTree->header()->hide();

	// Create the information list widget
	m_itemInfo = new QListWidget(this);

	// Create the push buttons
	m_btDelete = new QPushButton(tr("Delete"));
	m_btAddView = new QPushButton( tr("Add View"), this);
	m_btAddView->setMenu(m_menuAddView);

	// Create the layout for buttons
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(m_btDelete);
	buttonLayout->addWidget(m_btAddView);

	QWidget *topPart = new QWidget(this);
	QVBoxLayout *topLayout = new QVBoxLayout;
	topLayout->addWidget(m_viewTree);
	topLayout->addLayout(buttonLayout);
	topPart->setLayout(topLayout);

	QSplitter *splitter = new QSplitter(Qt::Vertical, this);
	splitter->addWidget(topPart);
	splitter->addWidget(m_itemInfo);

	// Create the main layout
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(splitter);

	this->setLayout(mainLayout);
}

void ViewListDock::initActions() {
	action_addPA = new QAction(tr("Add Parallel Coordinate"), this);
	action_addPA->setObjectName(QString::fromUtf8("action_addPA"));
	action_addPA->setIcon(QIcon(QString::fromUtf8(":/new/prefix2/icon/imgpa.png")));
	action_addPA->setCheckable(false);
	connect(action_addPA, SIGNAL(triggered()), m_handleViewList, SLOT(addViewPA()));

	action_addSC = new QAction(tr("Add Scatterplot Matrix"), this);
	action_addSC->setObjectName(QString::fromUtf8("action_addSC"));
	action_addSC->setIcon(QIcon(QString::fromUtf8(":/new/prefix2/icon/imgsc.png")));
	action_addSC->setCheckable(false);
	connect(action_addSC, SIGNAL(triggered()), m_handleViewList, SLOT(addViewSC()));

	action_addGL = new QAction(tr("Add Glyph"), this);
	action_addGL->setObjectName(QString::fromUtf8("action_addGL"));
	action_addGL->setIcon(QIcon(QString::fromUtf8(":/new/prefix2/icon/imgst.png")));
	action_addGL->setCheckable(false);
	connect(action_addGL, SIGNAL(triggered()), m_handleViewList, SLOT(addViewGL()));

	action_addDI = new QAction(tr("Add Dimension Stacking"), this);
	action_addDI->setObjectName(QString::fromUtf8("action_addDI"));
	action_addDI->setIcon(QIcon(QString::fromUtf8(":/new/prefix2/icon/imgdi.png")));
	action_addDI->setCheckable(false);
	connect(action_addDI, SIGNAL(triggered()), m_handleViewList, SLOT(addViewDI()));

	action_addPI = new QAction(tr("Add Pixel Oriented Display"), this);
	action_addPI->setObjectName(QString::fromUtf8("action_addPI"));
	action_addPI->setIcon(QIcon(QString::fromUtf8(":/new/prefix2/icon/imgpi.png")));
	action_addPI->setCheckable(false);
	connect(action_addPI, SIGNAL(triggered()), m_handleViewList, SLOT(addViewPI()));
}

void ViewListDock::initMenu() {
	m_menuAddView = new QMenu(this);

	m_menuAddView->addAction(action_addPA);
	m_menuAddView->addAction(action_addSC);
	m_menuAddView->addAction(action_addGL);
	m_menuAddView->addAction(action_addDI);
	m_menuAddView->addAction(action_addPI);
}
