/*
 * ColorStrategy.h
 *
 *  Created on: Jul 8, 2009
 *	Author: Jason Stasik
 *
 *	ColorStrategy allows the user to control what color each data point will be.
 *	This class merely tells the VisualMap what values to use to generate the
 *	color index. The VisualMap calculates the color, and the actual ColorMap is
 *	stored in ColorManager and can be changed through ColorCustom.
 *
 *	The sort operator is always available to the user even if they aren't coloring
 *	by order, to provide a global sorting method.
 */

#ifndef COLORSTRATEGY_H_
#define COLORSTRATEGY_H_

#include <QDialog>
#include <QMdiSubWindow>
#include <QRadioButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QGroupBox>

#include <vector>

class XmdvToolMainWnd;

class ColorStrategy : public QDialog {
	Q_OBJECT

public:
	// Constructor
	ColorStrategy(XmdvToolMainWnd *mainwnd, int pid);
	// Destructor
	virtual ~ColorStrategy();

	// Gets the user-selected dimension for Color By Value
	int getValueDim();

private:
	// Resets the GUI after a change is applied or cancelled
	void			reset();

	// A list of the dimension names
	std::vector<char*>		m_dim_names;
	// The PID of the pipeline
	int				m_pid;

	// The currently selected color strategy
	int				m_selectedRadio;
	// The applied dimension to color by
	int				m_valueDim;
	// The currently selected dimension to color by
	int				m_tempValue;

	// List of all the dimensions, so we can delete them later
	// QListWidgetItem does not derive from QObject, I think
	std::vector<QListWidgetItem*> listitems_dims;

	// Reference to the main window
	XmdvToolMainWnd *mainWnd;

	// No Color Strategy
	QRadioButton	*radio_none;
	// Color By Order
	QRadioButton	*radio_order;
	// Color By Value
	QRadioButton	*radio_value;

	// GroupBox storing value widgets, stored here for enabling/disabling
	QGroupBox		*group_value;

	// CheckBox for sort method
	QCheckBox		*check_asc;
	// Dimension to sort by
	QComboBox		*combo_sort;

	// Shows the user what their selected dimension is
	QLineEdit		*line_value;

	// Lets the user select a dimension
	QListWidget		*list_dims;

private slots:
	// Switches color strategy
	void			switchRadio();
	// Selects a new dimension
	void			selectDim(QListWidgetItem* item);
	// Applies changes
	void			apply();
	// Cancels changes
	void			cancel();

};

inline int ColorStrategy::getValueDim() {
	return m_valueDim;
}

#endif /* COLORSTRATEGY_H_ */
