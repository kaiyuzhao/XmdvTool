/*
 * ViewListDock.h
 *
 *  Created on: Feb 24, 2010
 *      Author: Zaixian Xie
 */

/*
 * Class ViewListDock manages the dock view to
 * show a tree widget and an information list
 */
#ifndef VIEWLISTDOCK_H_
#define VIEWLISTDOCK_H_

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QTreeWidget>
#include <QListWidget>
#include <QSize>
#include <QAction>
#include <QMenu>
#include <QLabel>

class HandleViewList;

class ViewListDock : public QWidget {
	friend class HandleViewList;

	Q_OBJECT

public:
	ViewListDock(QWidget* parent, HandleViewList* handleViewList);
	virtual ~ViewListDock();

private:

	// The pointer to the class HandleViewList
	HandleViewList* m_handleViewList;

	// m_viewTree is the widget to show a tree including all datasets,
	// pipelines and views.
	QTreeWidget* m_viewTree;
	// m_itemInfo is the widget to show the information of the selected item
	// in m_viewTree
	QListWidget* m_itemInfo;

	// The push button to delete the dataset or view nodes
	QPushButton *m_btDelete;
	// The push button to add a new view
	QPushButton *m_btAddView;

	// Add a parallel coordiate view
	QAction* action_addPA;
	// Add a scatterplot matrix view
	QAction* action_addSC;
	// Add a glyph view
	QAction* action_addGL;
	// Add a dimension stacking view
	QAction* action_addDI;
	// Add a pixel-oriented display view
	QAction* action_addPI;

	QMenu* m_menuAddView;

private:
	// Create all widgets and put them in the dock view
	void createWidgets();

	// Initialize the actions for adding new views
	void initActions();

	// Initialize the menu for adding new views
	void initMenu();

public:

};

#endif /* VIEWLISTDOCK_H_ */
