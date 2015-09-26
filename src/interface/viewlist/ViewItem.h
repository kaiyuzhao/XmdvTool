/*
 * ViewItem.h
 *
 *  Created on: Feb 23, 2010
 *      Author: Zaixian Xie
 */

/*
 * The class ViewItem is an item in the tree widget corresponding to
 * a view  node.  It is a subclass of QTreeWidgetItem.
 */

#ifndef VIEWITEM_H_
#define VIEWITEM_H_

#include <QTreeWidget>
#include <QTreeWidgetItem>

class ViewWindow;

class ViewItem : public QTreeWidgetItem {
public:
	ViewItem(QTreeWidget * parent, const QString & str, ViewWindow* viewWin);
	virtual ~ViewItem();

private:
	// The index of the corresponding ViewWindow in the vector m_viewWindows
	// in the view manager
	ViewWindow* m_viewWindow;

public:
	ViewWindow* getViewWindow();
};

#endif /* VIEWITEM_H_ */
