/*
 * GlyphCustom.h
 *
 *  Created on: May 15, 2009
 *      Author: Zaixian Xie
 */

/*
 * class GlyphCustom manages the dialog
 * "Glyph Customization for Flat Display"
 * The data member "m_pipelineID" means the pipeline ID
 * corresponding to this dialog.
 * When users changed the options in this dialog,
 * arguments in this pipeline will be changed,
 * and then the pipeline will be reassembled.
 *
 */
#ifndef GLYPHCUSTOM_H
#define GLYPHCUSTOM_H

#include <QDialog>

#include "ui_glyphcustom.h"
#include <vector>
#include "main/XmdvTool.h"

class Pipeline;
class XmdvToolMainWnd;
class OkcPipeline;
class OkcPipelineSettings;

class GlyphCustom : public QDialog
{
    Q_OBJECT

public:
	GlyphCustom(QWidget * parent);
    virtual ~GlyphCustom();

private:
    Ui::GlyphCustomClass ui;

    // The pointer to the main window
    XmdvToolMainWnd* m_mainWnd;
    // The pipeline ID linked to this dialog
    int m_pipelineID;

    // The dimension names
    std::vector<char*> m_dim_names;

    // The placement mode selected by the user
    XmdvTool::GLYPHPLACE_MODE m_placeMode;

    // The sorting order
    XmdvTool::SORTORDER m_sortOrder;
    // The selected dimension number
    int m_selectedDimNo;

    // The X and Y dimension mode and number for data driven and derived mode
    XmdvTool::OKCDIM m_XDim;
    int m_XDimNumber;
    XmdvTool::OKCDIM m_YDim;
    int m_YDimNumber;

    // m_prin_num is the number of principal components.
    // Assume that n is the number of dimension in the original datasets.
    // If n<3, then prin_num=n; otherwise, prin_num =3;
    int m_prin_num;


protected:
	void showEvent( QShowEvent* event );

private:
	// Initialize the widgets on the dialog
	// in terms of the pipeline configuration
	void initWidget();
	// Set some parameters, such as dimension names
	void setParameters();

	// Update the widgets after users change the place mode
	void changeToOriginal();
	void changeToOrdered();
	void changeToDatadriven();
	void changeToDerived();

public:
	// set the pointer to the XmdvTool main window
	void setMainWnd(XmdvToolMainWnd* mainWnd);
	// set the pipeline corresponding to this dialog
	void setPipelineID(int pID);
	// return the pointer to the pipeline corresponding to this dialog
	OkcPipeline* getOkcPipeline();
	// return the pointer to the pipeline settings
	OkcPipelineSettings* getOkcPipelineSettings();


private slots:
	// For radio buttons to choose sorting mode
	void on_radioButton_orig_toggled ( bool checked );
	void on_radioButton_order_toggled ( bool checked );
	void on_radioButton_datadriven_toggled ( bool checked );
	void on_radioButton_derived_toggled ( bool checked );

	// For check box to control increasing or decreasing sorting
	void on_checkBox_increase_stateChanged ( int state );

	// For the button to select dimension X and Y
	void on_pushButton_selectX_clicked();
	void on_pushButton_selectY_clicked();

	void on_listWidget_dims_currentRowChanged ( int currentRow );

	void on_pushButton_apply_clicked();
};

#endif // GLYPHCUSTOM_H
