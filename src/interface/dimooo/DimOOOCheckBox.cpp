/*
 * DimOOOCheckBox.cpp
 *
 *  Created on: Nov 4, 2009
 *      Author: Zaixian Xie
 */

#include "interface/dimooo/DimOOOCheckBox.h"

DimOOOCheckBox::DimOOOCheckBox(  int rowID, QWidget * parent )
	: QCheckBox(parent) {
	m_rowID = rowID;
	connect(this, SIGNAL( stateChanged (int) ),
	        this, SLOT( boxStateChanged(int) ) );
}

DimOOOCheckBox::~DimOOOCheckBox() {
}

void DimOOOCheckBox::boxStateChanged (int state) {
	emit stateChanged(m_rowID, state );
}
