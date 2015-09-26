/*
 * HandleViewList.h
 *
 *  Created on: Feb 23, 2010
 *      Author: Zaixian Xie
 */

/*
 * Class HandleViewList manages the dock view
 * to display a tree showing all opened datasets,
 * and their associated pipelines and views.
 */

#ifndef HANDLEVIEWLIST_H_
#define HANDLEVIEWLIST_H_

#include <QObject>
#include <QTreeWidget>
#include <QListWidget>
#include <QTreeWidgetItem>
#include <QDockWidget>

#include "main/XmdvTool.h"

#define DatasetItemType (QTreeWidgetItem::UserType + 1)
#define PipelineItemType (QTreeWidgetItem::UserType + 2)
#define ViewItemType (QTreeWidgetItem::UserType + 3)


class XmdvToolMainWnd;
class PipelineManager;
class ViewManager;
class ViewListDock;
class ViewItem;
class DatasetItem;

class HandleViewList : public QObject {
	friend class HandleView;

	Q_OBJECT
public:
	HandleViewList(XmdvToolMainWnd* mainWnd);
	virtual ~HandleViewList();

private:
	XmdvToolMainWnd* m_mainWnd;
	PipelineManager* m_pipelineManager;
	ViewManager* m_viewManager;

	// The dock view to show the tree widget
	ViewListDock* m_viewListDock;
	// The dock widget to hold the dock view
	QDockWidget *m_dock;

public slots:
	// Activated when the current item changed in the tree widget
	void currentItemChanged(  QTreeWidgetItem * current, QTreeWidgetItem * previous );
	// Delete the selected node
	void deleteNode();
	// Add a view whose type is specified by the parameter
	void addView(XmdvTool::VISTYPE visType);
	// Add a parallel coordinate
	void addViewPA();
	// Add a scatterplot matrix
	void addViewSC();
	// Add a glyphs
	void addViewGL();
	// Add a dimension stacking
	void addViewDI();
	// Add a pixel oriented display
	void addViewPI();

private:
	// Activate the windows associated with a widget item
	// This function will be called by slot function currentItemChanged()
	// when the selected item is a view node.
	void activateView(QTreeWidgetItem * current);
	// Add a new view for the pipeline associated witht the specified pipeline node
	void addViewForPipeline( QTreeWidgetItem* pipelineItem, XmdvTool::VISTYPE visType );

	// Remove a sub window associated with a view item
	void deleteView( ViewItem* viewItem );
	// Remove a dataset associated with a dataset item
	void deleteDataset( DatasetItem* datasetItem );

public:
	// Initialize the dock view, including m_viewTree and m_itemInfo
	void initDock();

	// Update widgets m_viewTree and m_itemInfo
	void updateWidgets();

	// Update the tree widget to show datasets, pipelines and views
	void updateViewTree();

	// Update the dialog for the information of the selected item in m_viewTree
	void updateItemInfo(QTreeWidgetItem * current);
};

#endif /* HANDLEVIEWLIST_H_ */
