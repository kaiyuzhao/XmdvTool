/*
 * HandleMdiArea.cpp
 *
 *  Created on: Jan 27, 2009
 *      Author: Zaixian Xie
 */

#include "HandleMdiArea.h"

HandleMdiArea::HandleMdiArea() {
}

HandleMdiArea::~HandleMdiArea() {
}

void HandleMdiArea::setMdiArea(QMdiArea* mdiArea) {
	m_mdiArea = mdiArea;
}

QMdiArea* HandleMdiArea::getMdiArea() {
	return m_mdiArea;
}
