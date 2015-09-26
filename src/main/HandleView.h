/*
 * HandleView.h
 *
 *  Created on: Mar 1, 2010
 *      Author: Zaixian Xie
 */

/*
 * Class HandleView manages the menu items under "View".
 * It can control:
 * (1) the visibility of view tree;
 * (2) font
 */
#ifndef HANDLEVIEW_H_
#define HANDLEVIEW_H_

#include <QObject>
#include <QAction>

class XmdvToolMainWnd;
class HandleViewList;

class HandleView :public QObject
{
	Q_OBJECT

public:
	HandleView(XmdvToolMainWnd *mainWnd);
	virtual ~HandleView();

private:
	XmdvToolMainWnd* m_mainWnd;
	HandleViewList* m_handleViewList;

public:
	void updateDockWidgets();
	// Return a checkable action that can be used to show or close this dock widget.
	QAction* viewTreeToggleAction();

public slots:
	// The function to show a dialog to change the system font after the user
	// selects the menu item "Font"
	void changeFont();
};

#endif /* HANDLEVIEW_H_ */
