/*
 * PixelCustomization.h
 *
 *  Created on: May 18, 2009
 *      Author: jstasik
 *
 *  Handles the dialog box that controls VisPixel customization
 */

#ifndef PIXELCUSTOMIZATION_H_
#define PIXELCUSTOMIZATION_H_

#include <QtGui/QDialog>
#include <QtGui/QComboBox>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QScrollArea>
#include <QtGui/QWidget>
#include <QtGui/QFormLayout>
#include <QtGui/QCheckBox>

#include <QMdiSubWindow>

#include <vector>

class VisPixel;
class XmdvToolMainWnd;
class PixelCustomizationSettings;
class ViewDisplay;
class OkcPipeline;

class PixelCustomization : public QDialog
{
	Q_OBJECT

public:
	PixelCustomization(XmdvToolMainWnd *mainwnd, QMdiSubWindow *window,
			PixelCustomizationSettings *setting);
	~PixelCustomization();

	// Update the visualization with all the fields at once
	void			applyChanges(ViewDisplay *view);

	PixelCustomizationSettings* getSettings();

	// Update the dialog with input from the visualization
	void			update();

	// Update the settings class with MDS position
	void			updateMDS(double **mds);

	VisPixel*		getVisPixel();
	void			brushClosed();

	void			disableGUI();

private:

	// Create the settings class
	void			setDefaultValues();

private:
	bool			brushOpen;

	XmdvToolMainWnd *mainWnd;

	VisPixel		*visPixel;
	// The pointer to the Pipeline
	OkcPipeline		*m_okcPipeline;
	// the number of displayed dimensions
	int				m_dim_size;
	// the names for displayed dimensions
	std::vector<char*>		m_dim_names;
	// the number of all dimensions
	int				m_orig_dim_size;
	// the names for all dimensions
	std::vector<char*>		m_orig_dim_names;

	PixelCustomizationSettings *settings;

	QComboBox		*combo_selectedDim;
	QComboBox		*combo_sortedDim;
	QComboBox		*combo_pixelLayout;

	QRadioButton	*radio_ascending;
	QRadioButton	*radio_descending;

	QRadioButton	*radio_normal;
	QRadioButton	*radio_mds;

	QSlider			*slider_scale;
	QCheckBox		*check_scaleAll;

	QCheckBox		*check_enableSampling;
	QSlider			*slider_sampling;

	//QPushButton 	*button_brush;

private slots:
	void			setSortedDim(int dim);
	void			setSelectedDim(int dim);
	void			setOrder(int id);
	void			setScale(int scale);
	void			setPlacement(int id);
	void			setScaleAll(bool all);
	void			setSampling(int rate);
	void			enableSampling(bool enable);
	void			setPixelLayout(int layout);

	//void			openBrush();

};

inline void PixelCustomization::brushClosed() {
	brushOpen = false;
}

#endif // PIXELCUSTOMIZATION_H_
