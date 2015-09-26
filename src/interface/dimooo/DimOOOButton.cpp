/*
 * DimOOOButton.cpp
 *
 *  Created on: Nov 4, 2009
 *      Author: xiezx
 */

#include "DimOOOButton.h"

DimOOOButton::DimOOOButton ( int rowID, DimOOOMap::DIRECTION direction, const QString & text, QWidget * parent )
	: QPushButton (text, parent){
	m_rowID = rowID;
	m_direction = direction;
	connect(this, SIGNAL( clicked () ),
	        this, SLOT( buttonClicked() ) );
}

DimOOOButton::~DimOOOButton() {
}

void DimOOOButton::buttonClicked () {
	emit clicked(m_rowID, (int)m_direction);
}

