/*
 * ZoomLineEdit.h
 *
 *  Created on: Oct 6, 2009
 *      Author: zyguo
 */

#ifndef ZOOMLINEEDIT_H_
#define ZOOMLINEEDIT_H_

#include <QtGui/QLineEdit>
#include "main/XmdvToolMainWnd.h"

class ZoomLineEdit : public QLineEdit{
	Q_OBJECT
public:
	ZoomLineEdit();
	ZoomLineEdit(XmdvToolMainWnd* _mainWnd, QString qString);
	virtual ~ZoomLineEdit();

private:
	XmdvToolMainWnd* mainWnd;


};

#endif /* ZOOMLINEEDIT_H_ */
