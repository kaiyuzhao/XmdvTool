/*
 * DimOOOButton.h
 *
 *  Created on: Nov 4, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class DimOOOButton represents a QPushButton in the
 * Dimension On/Off/ReOrder dialog.  It is an subclass of
 * QPushButton and introduces two new data member, m_boxID
 * which indicates which row this QCheckBox lies in.
 * This is to allow the slot function to know which Checkbox
 * the user is clicking.
 */

#ifndef DIMOOOBUTTON_H_
#define DIMOOOBUTTON_H_

#include <QPushButton>
#include <QString>
#include "data/multidim/DimOOOMap.h"

class DimOOOButton : public QPushButton {
	Q_OBJECT
public:
	DimOOOButton ( int rowID, DimOOOMap::DIRECTION direction, const QString & text, QWidget * parent = 0 );
	virtual ~DimOOOButton();

private:
	int m_rowID;
	DimOOOMap::DIRECTION m_direction;

private slots:
	void buttonClicked ();

private:
	signals:
	void clicked ( int rowID,  int direction );
};

#endif /* DIMOOOBUTTON_H_ */
