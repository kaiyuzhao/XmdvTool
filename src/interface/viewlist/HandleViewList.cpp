/*
 * HandleViewList.cpp
 *
 *  Created on: Feb 23, 2010
 *      Author: Zaixian Xie
 */

#include "interface/viewlist/HandleViewList.h"

#include <QList>
#include <QString>
#include <QHeaderView>
#include <QDockWidget>

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Pipeline.h"
#include "visualmap/VisualMapResult.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "view/ViewDisplay.h"
#include "interface/viewlist/DatasetItem.h"
#include "interface/viewlist/PipelineItem.h"
#include "interface/viewlist/ViewItem.h"
#include "interface/viewlist/ViewListDock.h"

HandleViewList::HandleViewList(XmdvToolMainWnd* mainWnd){
	this->m_mainWnd = mainWnd;
	m_viewListDock = 0;

	m_pipelineManager = m_mainWnd->getPipelineManager();
	m_viewManager = m_mainWnd->getViewManager();
}

HandleViewList::~HandleViewList() {
	SAFE_DELETE(m_viewListDock);
}

void HandleViewList::initDock() {
	// Create and add the tree view to the dock
	m_dock = new QDockWidget(QString("View Tree"), m_mainWnd);
	m_dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	m_viewListDock = new ViewListDock(m_dock, this);
	m_dock->setWidget(m_viewListDock);
	m_dock->setFeatures(QDockWidget::DockWidgetClosable);
	m_mainWnd->addDockWidget(Qt::LeftDockWidgetArea, m_dock);
}

// Update widgets m_viewTree and m_itemInfo
void HandleViewList::updateWidgets() {
	updateViewTree();
}

// Update the tree widget to show datasets, pipelines and views
void HandleViewList::updateViewTree() {
	QTreeWidget* viewTree = m_viewListDock->m_viewTree;

	viewTree->clear();

	// Get the active window No. We will select it in the tree widget
	int activeWinNo = m_viewManager->getActiveViewWindowNo();
	QTreeWidgetItem * currentItem = 0;

	int i,j,k;
	QList<QTreeWidgetItem *> datasets;
	// Traverse all original dataset
	for ( i=0; i<m_pipelineManager->getOrigDatasetsCount(); i++) {
		Data* data = m_pipelineManager->getOrigDataset(i);
		if ( data ) {
			// We assume that all data is OkcData
			OkcData* okcdata = dynamic_cast<OkcData*>(data);
			DatasetItem* newDatasetItem = new DatasetItem( (QTreeWidget*)0, okcdata->filename, data);

			// Search all pipelines to find those associated with this data
			QList<QTreeWidgetItem *> pipelines;
			for ( j=0; j<m_pipelineManager->getPipelinesCount(); j++ ) {
				Pipeline* pl = m_pipelineManager->getPipeline(j);
				if (!pl) {
					// If this is a null pointer, the pipeline has been removed
					continue;
				}
				int pipelineID = pl->getPipelineID();
				if ( pl->getOrigDataset()==data ) {
					PipelineItem* newPipelineItem = 0;
					// If this pipeline is associated with this data
					switch  ( pl->getPipelineType() ) {
					case XmdvTool::PLTYPE_FLATNORMAL :
						newPipelineItem = new PipelineItem( (QTreeWidget*)0, "Flat Data Mode Pipeline", pl );
						break;
					case XmdvTool::PLTYPE_SBB :
						newPipelineItem = new PipelineItem( (QTreeWidget*)0, "Hierarchical Data Mode Pipeline", pl );
						break;
					case XmdvTool::PLTYPE_DIMR :
						newPipelineItem = new PipelineItem( (QTreeWidget*)0, "Hierarchical Dimension Mode Pipeline", pl );
						break;
					}

					QList<QTreeWidgetItem *> views;
					int viewNo = 0;
					for ( k=0; k<m_viewManager->getViewWindowsCount(); k++ ) {
						ViewWindow* win = m_viewManager->getViewWindow(k);
						if ( !win ){
							// If this is a null pointer, the view has been removed
							continue;
						}
						if (win && win->getPipelineID()==pipelineID ) {
							ViewItem* newViewItem =
								new ViewItem((QTreeWidget*)0,
										ViewWindow::createViewName( win->getOrder() ),
										win );
							viewNo++;
							views.append( newViewItem);

							// If this is the active view, we record it to set as the current item
							// after we finish initializing the tree widget
							if ( k == activeWinNo ) {
								currentItem = (QTreeWidgetItem*)newViewItem;
							}
						}
					}
					// Add the view nodes as the children of the pipeline node
					newPipelineItem->addChildren (views);

					pipelines.append( newPipelineItem );
				}
			}

			// Add the pipeline nodes as the children of the dataset node
			newDatasetItem->addChildren(pipelines);

			datasets.append( newDatasetItem );
		}
	}

	viewTree->insertTopLevelItems(0, datasets);
	viewTree->setCurrentItem(currentItem, 0, QItemSelectionModel::SelectCurrent);

	viewTree->expandAll ();
}

// Update the dialog for the information of the selected item in m_viewTree
void HandleViewList::updateItemInfo(QTreeWidgetItem * current) {
	if (!current) {
		// If current is a NULL pointer, clear the ListWidget for
		// the selected node information.
		m_viewListDock->m_itemInfo->clear();
	} else {
		// If current is not a NULL pointer, update the ListWidget
		// for the selected node information.
		QStringList info;
		switch ( current->type() ) {
		case DatasetItemType :
			info = dynamic_cast<DatasetItem*>(current)
			->getDataset()->toStringList();
			break;
		case PipelineItemType :
			info = dynamic_cast<PipelineItem*>(current)
			->getPipeline()->toStringList();
			break;
		case ViewItemType :
			info = dynamic_cast<ViewItem*>(current)
			->getViewWindow()->toStringList();
			break;
		default:
			return;
		}

		m_viewListDock->m_itemInfo->clear();
		m_viewListDock->m_itemInfo->addItems(info);
	}
}

void HandleViewList::currentItemChanged(  QTreeWidgetItem * current, QTreeWidgetItem * ) {
	// First, update the item info ListWidget.
	// If current is NULL, this will clear the item info ListWidget
	updateItemInfo(current);
	if (current) {
		// If current is not NULL, set the status of "Delete" and "AddView" buttons
		// based on the type of the current node;
		switch ( current->type() ) {
		case DatasetItemType :
			m_viewListDock->m_btDelete->setEnabled(true);
			m_viewListDock->m_btAddView->setEnabled(false);
			break;
		case PipelineItemType :
			m_viewListDock->m_btDelete->setEnabled(false);
			m_viewListDock->m_btAddView->setEnabled(true);
			break;
		case ViewItemType :
			m_viewListDock->m_btDelete->setEnabled(true);
			m_viewListDock->m_btAddView->setEnabled(false);
			activateView(current);
			break;
		}
	} else {
		// if current is NULL, disable both "Delete" and "AddView" buttons.
		m_viewListDock->m_btDelete->setEnabled(false);
		m_viewListDock->m_btAddView->setEnabled(false);
	}
}

void HandleViewList::deleteNode() {
	// Get the current item
	QTreeWidgetItem * current = m_viewListDock->m_viewTree->currentItem ();

	if (!current) {
		return;
	}
	switch ( current->type() ) {
	case DatasetItemType :
		deleteDataset( dynamic_cast<DatasetItem*>(current) );
		break;
	case PipelineItemType :
		break;
	case ViewItemType :
		deleteView( dynamic_cast<ViewItem*>(current) );
		break;
	}

}

void HandleViewList::addView( XmdvTool::VISTYPE visType) {
	// Get the current item
	QTreeWidgetItem * current = m_viewListDock->m_viewTree->currentItem ();

	if (!current) {
		return;
	}
	switch ( current->type() ) {
	case DatasetItemType :
		break;
	case PipelineItemType :
		addViewForPipeline( current, visType );
		break;
	case ViewItemType :
		break;
	}
}

void HandleViewList::addViewPA() {
	addView( XmdvTool::VISTYPE_PA );
}

void HandleViewList::addViewSC() {
	addView ( XmdvTool::VISTYPE_SC );
}

void HandleViewList::addViewGL() {
	addView ( XmdvTool::VISTYPE_GL );
}

void HandleViewList::addViewDI() {
	addView ( XmdvTool::VISTYPE_DI );
}

void HandleViewList::addViewPI() {
	addView ( XmdvTool::VISTYPE_PI );
}

void HandleViewList::activateView(QTreeWidgetItem * current) {
	ViewItem* currentItem = dynamic_cast<ViewItem*>(current);
	ViewWindow* viewWin = currentItem->getViewWindow();
	m_viewManager->activateView(viewWin);
}

void HandleViewList::addViewForPipeline( QTreeWidgetItem* pipelineItem, XmdvTool::VISTYPE visType ) {
	// dataItem corresponds to the dataset associated with this pipeline
	DatasetItem* dataItem =  dynamic_cast<DatasetItem*>
	(pipelineItem->parent());
	Data* data = dataItem->getDataset();

	PipelineItem* plItem = dynamic_cast<PipelineItem*>(pipelineItem);
	int pipelineID = m_pipelineManager->getPipelineID( plItem->getPipeline() );
	ViewWindow* viewWin = m_viewManager->createViewWindow(pipelineID,
			data->getTitle(), visType ) ;

	// Scan all of pipelines associated with this dataset
	int j;
	for ( j=0; j<m_pipelineManager->getPipelinesCount(); j++ ) {
		Pipeline* pl = m_pipelineManager->getPipeline(j);
		// If this is a null pointer, skip it
		if ( !pl ) {
			continue;
		}
		int pipelineID = pl->getPipelineID();
		if ( pl && pl->getOrigDataset()==data ) {
			// If this pipeline is associated with this data
			switch  ( pl->getPipelineType() ) {
			case XmdvTool::PLTYPE_FLATNORMAL :
				viewWin->setFlatPipelineID(pipelineID);
				break;
			case XmdvTool::PLTYPE_SBB :
				viewWin->setSBBPipelineID(pipelineID);
				break;
			case XmdvTool::PLTYPE_DIMR :
				viewWin->setDimRPipelineID(pipelineID);
				break;
			}
		}
	}

	// Re-assemble the pipeline with only executing visual mapping
	m_pipelineManager->assemble(pipelineID, -1);
	m_viewManager->refreshDisplayByPID(pipelineID);
}

void HandleViewList::deleteView( ViewItem* viewItem ) {
	ViewWindow* viewWin = viewItem->getViewWindow();
	m_viewManager->closeViewWindow(viewWin);
}

void HandleViewList::deleteDataset( DatasetItem* datasetItem ) {
	// Clear the ListWidget by passing NULL pointer to
	// the function updateItemInfo().
	// updateItemInfo(0);
	Data* data = datasetItem->getDataset();
	m_viewManager->closeViewWindows(data);
}
