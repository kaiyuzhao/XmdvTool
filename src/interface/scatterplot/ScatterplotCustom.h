/*
 * ScatterplotCustom.h
 *
 *  Created on: Jul 27, 2009
 *      Author: zyguo
 */

#ifndef SCATTERPLOTCUSTOM_H_
#define SCATTERPLOTCUSTOM_H_



#include <vector>
#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "view/ViewManager.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/multidim/VisScatter.h"
#include "pipeline/multidim/OkcPipeline.h"
#include "operator/pcaderive/PCADeriveOperator.h"

#include <QDialog>
#include <QMdiSubWindow>
#include <QRadioButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QGroupBox>

#include <QtGui/QDialog>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QWidget>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QFrame>
#include <QtGui/QVBoxLayout>
#include <QMdiSubWindow>



class Pipeline;
class XmdvToolMainWnd;
class OkcPipeline;
class OkcPipelineSettings;


class ScatterplotCustom : public QDialog{

	 Q_OBJECT

public:
	ScatterplotCustom(XmdvToolMainWnd *mainwnd, QMdiSubWindow *window);
	virtual ~ScatterplotCustom();

private:
	XmdvToolMainWnd *mainWnd;
	ViewManager *viewManager;
	int windowNo, pID; //window and pipeline IDs

    // The dimension names
    std::vector<char*> m_dim_names;

     // The sorting order
    XmdvTool::SORTORDER m_sortOrder;
    // The selected dimension number
    int m_selectedDimNo;

    // The X and Y dimension number for data driven mode
    int m_dataDrivenXDimNumber;
    int m_dataDrivenYDimNumber;

    // The X and Y dimension number for derived mode
    int m_derivedXDimNumber;
    int m_derivedYDimNumber;

    //define radio and buttons

    // the display type for the diagonal plot
    // original display
    QRadioButton	*radio_original;
    // histogram display
    QRadioButton	*radio_histogram;
    // one dimensional display
    QRadioButton	*radio_one_dimension;
    // two dimensional display
    QRadioButton	*radio_two_dimension;
    // point shape is square
    QRadioButton	*radio_shape_square;
    // point shape is circle
    QRadioButton *radio_shape_circle;

    QRadioButton *radio_one_D_order_original;
    QRadioButton *radio_one_D_order_data_driven;

    // Dimension to sort by
    QComboBox	*combo_sort;

	QRadioButton *radio_Implicit;

	QRadioButton *radio_from_file;

	QRadioButton *radio_data_drive;

	QRadioButton *radio_derived;

	QSlider *slider_HistogramSize;

	QPushButton *pushButton_selectX;

	QLineEdit *lineEdit_dimX;
	QPushButton *pushButton_selectY;
	QLineEdit *lineEdit_dimY;
	QPushButton *pushButton_apply;
	QListWidget *listWidget_dims;




    // GroupBox storing all display type, stored here for enabling/disabling
    QGroupBox		*group_value;

    VisScatter* getScatterplotViewDisplay();

    void setTwoDimOptionsEnabled(bool enableFlag);


    void setTwoDimDataDrivenOptionEnabled(bool enableFlag);
    void setOneDimOptionsEnabled(bool enableFlag);
    void enableDerivedDims(bool enableFlag);

    OkcPipeline* getOkcPipeline();


private slots:
	// Switches point type
	void			switchPointType();

	//set the point size
	void 			setPointSize(int size);

	//switches the diagonal dispaly type
	void  			switchDiagDisplayType();

	//set the histogram size (histogram bin number)
	void  			setHistoSize(int histoSize);

	void 			switchOneDOrder(int sortDim);

	// For the button to select dimension X and Y
	void on_pushButton_selectX_clicked();
	void on_pushButton_selectY_clicked();

	// Selects a new dimension
	//void			selectDim(QListWidgetItem* item);
	// Applies changes
	void			apply();
	// Cancels changes
	//void			cancel();

};

#endif /* SCATTERPLOTCUSTOM_H_ */
