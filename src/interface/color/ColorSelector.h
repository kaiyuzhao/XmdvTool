/*
 * ColorSelector.h
 *
 *  Created on: Jun 12, 2009
 *  Author: Jason Stasik
 *
 *  ColorSelector is a dialog that lets the user replace
 *  an existing color ramp with a new one. They are allowed
 *  to pick from any of the allowed types of ramps for the
 *  given color map. For each ramp type, they can pick any
 *  of the predefined ramps, and for RAMP_SINGLE, they can
 *  choose any color they want.
 */

#ifndef COLORSELECTOR_H_
#define COLORSELECTOR_H_

#include <vector>

#include <QDialog>
#include <QSignalMapper>
#include <QLabel>

class ColorCustom;
class ColorMap;

class ColorSelector : public QDialog {
	Q_OBJECT

public:
	// Constructor, a reference to the ColorCustom class and the color to change
	ColorSelector(ColorCustom *colorCustom, ColorMap *color, int idx);
	// Destructor
	virtual ~ColorSelector();

private:
	int			m_selected; // the index of the currently selected ramp
	int			m_numBins; // the number of bins to make the ramps
	bool		m_bins; // whether the ramps should be in bins or continuous
	int			m_index; // which color is currently selected

	ColorMap	*m_color; // the color to change
	ColorCustom	*m_colorCustom; // the global ColorCustom class instance
	QLabel		*m_singleLabel; // widget to draw on for RAMP_SINGLE
	ColorMap	*m_singleColor; // stored RAMP_SINGLE color
	std::vector<QLabel*> seq_labels, div_labels, qual_labels, link_labels; // widgets to draw on for each type

	QSignalMapper *m_map; // maps radio buttons to ColorManager::PredefinedColorRamps

private:
	// Draws the color bars on the QGLWidgets
	void		drawColorBars();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	// Gets a new RAMP_SINGLE color
	void		getNewColor();
	// Applies the selected changes to the color
	void		apply();
	// Discards all changes made to the color
	void		cancel();
	// Updates the selected color
	void		setSelectedRadio(int button);
	// Updates whether or not bins are used
	void		setUseBins(bool checked);
	// Updates the number of bins
	void		setNumBins(int bins);
};

#endif /* COLORSELECTOR_H_ */
