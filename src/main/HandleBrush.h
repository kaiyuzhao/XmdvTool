/*
 * HandleBrush.h
 *
 *  Created on: Aug 11, 2009
 *      Author: xiezx
 */

/*
 * Class HandleToos provides slot functions triggered by the menu "Brushes"
 */

#ifndef HANDLEBRUSH_H_
#define HANDLEBRUSH_H_

#include <QtCore/QObject>

class XmdvToolMainWnd;

class HandleBrush : public QObject
{
	Q_OBJECT

public:
	HandleBrush(XmdvToolMainWnd* mainWnd);
	virtual ~HandleBrush();

private:
	XmdvToolMainWnd* m_mainWnd;

public slots:
	// open the dialog for structure-based brush
	void openSBBDialog();
};

#endif /* HANDLEBRUSH_H_ */
