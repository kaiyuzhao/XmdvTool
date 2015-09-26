/*
 * ZoomLineEdit.cpp
 *
 *  Created on: Oct 6, 2009
 *      Author: zyguo
 */

#include "ZoomLineEdit.h"

ZoomLineEdit::ZoomLineEdit() {
}

ZoomLineEdit::ZoomLineEdit(XmdvToolMainWnd* _mainWnd, QString qString) :
	QLineEdit(QString(qString)){
	this->mainWnd = _mainWnd;
	setMaxLength(4);
	setMaximumWidth(40);
}

ZoomLineEdit::~ZoomLineEdit() {
}


