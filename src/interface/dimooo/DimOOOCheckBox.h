/*
 * DimOOOCheckBox.h
 *
 *  Created on: Nov 4, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class DimOOOCheckBox represents a QCheckBox in the
 * Dimension On/Off/ReOrder dialog.  It is an subclass of
 * QCheckBox and introduces a new data member, m_rowID
 * which indicates which row this QCheckBox lies in.
 * This is to allow the slot function to know which Checkbox
 * the user is clicking.
 */

#ifndef DIMOOOCHECKBOX_H_
#define DIMOOOCHECKBOX_H_

#include <QCheckBox>

class DimOOOCheckBox : public QCheckBox {
	Q_OBJECT
public:
	DimOOOCheckBox( int rowID, QWidget * parent = 0 );
	virtual ~DimOOOCheckBox();

private:
	int m_rowID;


private slots:
	void boxStateChanged (int state);

private:
	signals:
	void stateChanged ( int rowID, int state );
};

#endif /* DIMOOOCHECKBOX_H_ */
