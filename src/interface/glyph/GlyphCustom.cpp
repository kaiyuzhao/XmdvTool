#include "interface/glyph/GlyphCustom.h"

#include <typeinfo>
#include <QMessageBox>
#include <QListWidget>
#include <algorithm>
#include <assert.h>
using namespace std;

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include <vector>
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "pipeline/Pipeline.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Transformation.h"
#include "pipeline/multidim/OkcPipeline.h"
#include "pipeline/multidim/OkcPipelineSettings.h"
#include "operator/Operator.h"
#include "operator/rowindex/SortRowsOperator.h"
#include "operator/pcaderive/PCADeriveOperator.h"
#include "visualmap/VisualMap.h"
#include "visualmap/multidim/OkcVisualMap.h"
#include "visualmap/multidim/GlyphPlace.h"
#include "view/ViewManager.h"

GlyphCustom::GlyphCustom(QWidget * parent)
	: QDialog(parent) {
	ui.setupUi(this);
	setFixedSize(width(), height());
}

GlyphCustom::~GlyphCustom() {

}

void GlyphCustom::setMainWnd(XmdvToolMainWnd * mainWnd) {
	m_mainWnd = mainWnd;
}

void GlyphCustom::setPipelineID(int pID) {
	m_pipelineID = pID;
}

OkcPipeline* GlyphCustom::getOkcPipeline() {
    PipelineManager* pm = m_mainWnd->getPipelineManager();
    Pipeline* pl = pm->getPipeline(m_pipelineID);
    return dynamic_cast<OkcPipeline*>(pl);
}

OkcPipelineSettings* GlyphCustom::getOkcPipelineSettings() {
	return getOkcPipeline()->getOkcPipelineSettings();
}

void GlyphCustom::showEvent( QShowEvent* ) {
	setParameters();
	initWidget();
}

void GlyphCustom::initWidget() {
	PipelineManager* pm = m_mainWnd->getPipelineManager();
	Pipeline* pl = pm->getPipeline(m_pipelineID);

	// Get the instance of OkcVisualMap
	VisualMap* visMap = pl->getVisualMap();
	assert( typeid(*visMap)==typeid(OkcVisualMap) );
	OkcVisualMap* okcVisMap = dynamic_cast<OkcVisualMap*>(visMap);

	// Get the instance of GlyphPlace
	GlyphPlace* place = okcVisMap->getGlyphPlace();
	XmdvTool::GLYPHPLACE_MODE mode = place->getGlyphPlaceMode();

	// Check one radio button about sorting mode in terms of
	// the OkcVisualMap configuration
	switch (mode) {
	case XmdvTool::GLYPHPLACE_ORIGINAL :
		ui.radioButton_orig->setChecked(true);
		changeToOriginal();
		break;
	case XmdvTool::GLYPHPLACE_ORDERED :
		ui.radioButton_order->setChecked(true);
		changeToOrdered();
		break;
	case XmdvTool::GLYPHPLACE_DATA_DRIVEN	:
		ui.radioButton_datadriven->setChecked(true);
		changeToDatadriven();
		break;
	case XmdvTool::GLYPHPLACE_DERIVED :
		ui.radioButton_derived->setChecked(true);
		changeToDerived();
		break;
	}

}

void GlyphCustom::setParameters() {
    Transformation* mainT = getOkcPipeline()->getMainTransformation();
    // We get the former one from the PCADerivedOperator and SortRowsOperator.
    Data* pca_sortOpInput = mainT->getOperatorInput(
    		min( PL_FN_OP_PCADERIVE, PL_FN_OP_SORT ) );
    assert(typeid(*pca_sortOpInput)==typeid(OkcData));
    ((OkcData*)pca_sortOpInput)->getDimNameArr(m_dim_names);

    // calculate the number of principal components
    int dim_size = ((OkcData*)pca_sortOpInput)->getDimSize();
    if (dim_size<3) {
    	m_prin_num = dim_size;
    } else {
    	m_prin_num = 3;
    }

    m_sortOrder = XmdvTool::SORTORDER_INCREASE;
    m_selectedDimNo = -1;

    m_XDim = XmdvTool::OKCDIM_ORIGINAL;
    m_XDimNumber = -1;
    m_YDim = XmdvTool::OKCDIM_ORIGINAL;
    m_YDimNumber = -1;

}

void GlyphCustom::on_radioButton_orig_toggled ( bool checked ) {
	if (checked) {
		changeToOriginal();
	}
}

void GlyphCustom::on_radioButton_order_toggled ( bool checked ) {
	if (checked) {
		changeToOrdered();
	}
}

void GlyphCustom::on_radioButton_datadriven_toggled ( bool checked ) {
	if (checked) {
		changeToDatadriven();
	}
}

void GlyphCustom::on_radioButton_derived_toggled ( bool checked ) {
	if (checked) {
		changeToDerived();
	}
}

void GlyphCustom::on_checkBox_increase_stateChanged ( int state ) {
	if (state) {
		m_sortOrder = XmdvTool::SORTORDER_INCREASE;
	} else {
		m_sortOrder = XmdvTool::SORTORDER_DECREASE;
	}
}

void GlyphCustom::on_pushButton_selectX_clicked() {
	int row = ui.listWidget_dims->currentRow();
	if (row>=0 && row<(int)ui.listWidget_dims->count()) {
		ui.lineEdit_dimX->setText(ui.listWidget_dims->currentItem()->text());
		if (m_placeMode==XmdvTool::GLYPHPLACE_DERIVED) {
			// For derived mode,
			if (row<m_prin_num) {
				// The case that users select one principal component
				m_XDim = XmdvTool::OKCDIM_PCADERIVE;
				m_XDimNumber = row;
			} else {
				// The case that users select one original dimension
				m_XDim = XmdvTool::OKCDIM_ORIGINAL;
				m_XDimNumber = row-m_prin_num;
			}
		} else if (m_placeMode==XmdvTool::GLYPHPLACE_DATA_DRIVEN) {
			// For other mode
			m_XDim = XmdvTool::OKCDIM_ORIGINAL;
			m_XDimNumber = row;
		}
	}
}

void GlyphCustom::on_pushButton_selectY_clicked() {
	int row = ui.listWidget_dims->currentRow();
	if ( row>=0 && row<(int)ui.listWidget_dims->count()) {
		ui.lineEdit_dimY->setText(ui.listWidget_dims->currentItem()->text());
		if (m_placeMode==XmdvTool::GLYPHPLACE_DERIVED) {
			// For derived mode,
			if (row<m_prin_num) {
				// The case that users select one principal component
				m_YDim = XmdvTool::OKCDIM_PCADERIVE;
				m_YDimNumber = row;
			} else {
				// The case that users select one original dimension
				m_YDim = XmdvTool::OKCDIM_ORIGINAL;
				m_YDimNumber = row-m_prin_num;
			}
		} else if (m_placeMode==XmdvTool::GLYPHPLACE_DATA_DRIVEN) {
			// For other mode
			m_YDim = XmdvTool::OKCDIM_ORIGINAL;
			m_YDimNumber = row;
		}
	}
}

void GlyphCustom::on_listWidget_dims_currentRowChanged ( int currentRow ) {
	if (m_placeMode==XmdvTool::GLYPHPLACE_ORDERED
			&& currentRow>=0
			&& currentRow<(int)m_dim_names.size()) {
		ui.lineEdit_sortDim->setText(m_dim_names[currentRow]);
		m_selectedDimNo = currentRow;
	}
}

void GlyphCustom::changeToOriginal() {
	// Update the enabled status
    ui.checkBox_increase->setEnabled(false);
    ui.lineEdit_sortDim->setEnabled(false);
    ui.pushButton_selectX->setEnabled(false);
    ui.lineEdit_dimX->setEnabled(false);
    ui.pushButton_selectY->setEnabled(false);
    ui.lineEdit_dimY->setEnabled(false);
    ui.listWidget_dims->setEnabled(false);
    ui.lineEdit_dimX->clear();
    ui.lineEdit_dimY->clear();
    ui.lineEdit_sortDim->clear();

    m_placeMode = XmdvTool::GLYPHPLACE_ORIGINAL;

	ui.listWidget_dims->clear();
}

void GlyphCustom::changeToOrdered() {
	// Update the enabled status
    ui.checkBox_increase->setEnabled(true);
    ui.lineEdit_sortDim->setEnabled(true);
    ui.pushButton_selectX->setEnabled(false);
    ui.lineEdit_dimX->setEnabled(false);
    ui.pushButton_selectY->setEnabled(false);
    ui.lineEdit_dimY->setEnabled(false);
    ui.listWidget_dims->setEnabled(true);
    ui.lineEdit_dimX->clear();
    ui.lineEdit_dimY->clear();
    ui.lineEdit_sortDim->clear();

    m_placeMode = XmdvTool::GLYPHPLACE_ORDERED;

	ui.listWidget_dims->clear();
    int i, n=m_dim_names.size();
    for (i=0; i<n; i++) {
    	new QListWidgetItem(m_dim_names[i], ui.listWidget_dims);
    }

    // Get the sorting operator
	SortRowsOperator* sortOp = getOkcPipeline()->getSortRowsOperator();

	// If the dataset is currently ordered by some dimension,
	// show the dimension name in the lineEdit_sortDim
	// and check or uncheck the checkBox to indicate
	// increase or decrease ordering
	if ( sortOp->getSortWay() == XmdvTool::OKCDIM_ORIGINAL ) {
		ui.lineEdit_sortDim->setText( m_dim_names[sortOp->getSortDimNo() ] );
		if ( sortOp->getSortOrder()==XmdvTool::SORTORDER_INCREASE ) {
			ui.checkBox_increase->setCheckState(Qt::Checked);
		} else {
			ui.checkBox_increase->setCheckState(Qt::Unchecked);
		}
	}

}

void GlyphCustom::changeToDatadriven() {
	// Update the enabled status
    ui.checkBox_increase->setEnabled(false);
    ui.lineEdit_sortDim->setEnabled(false);
    ui.pushButton_selectX->setEnabled(true);
    ui.lineEdit_dimX->setEnabled(true);
    ui.pushButton_selectY->setEnabled(true);
    ui.lineEdit_dimY->setEnabled(true);
    ui.listWidget_dims->setEnabled(true);
    ui.lineEdit_dimX->clear();
    ui.lineEdit_dimY->clear();
    ui.lineEdit_sortDim->clear();

    m_placeMode = XmdvTool::GLYPHPLACE_DATA_DRIVEN;

    ui.listWidget_dims->clear();
    int i, n=m_dim_names.size();
    for (i=0; i<n; i++) {
    	new QListWidgetItem(m_dim_names[i], ui.listWidget_dims);
    }
}

void GlyphCustom::changeToDerived() {
	// Update the enabled status
    ui.checkBox_increase->setEnabled(false);
    ui.lineEdit_sortDim->setEnabled(false);
    ui.pushButton_selectX->setEnabled(true);
    ui.lineEdit_dimX->setEnabled(true);
    ui.pushButton_selectY->setEnabled(true);
    ui.lineEdit_dimY->setEnabled(true);
    ui.listWidget_dims->setEnabled(true);
    ui.lineEdit_dimX->clear();
    ui.lineEdit_dimY->clear();
    ui.lineEdit_sortDim->clear();

    m_placeMode = XmdvTool::GLYPHPLACE_DERIVED;

    ui.listWidget_dims->clear();
    int i;
    for (i=0; i<m_prin_num; i++) {
        QString prin_name = QString("Principal_Component_%1").arg(i);
    	new QListWidgetItem(prin_name, ui.listWidget_dims);
    }
    for (i=0; i<(int)m_dim_names.size(); i++) {
    	new QListWidgetItem(m_dim_names[i], ui.listWidget_dims);
    }
}

void GlyphCustom::on_pushButton_apply_clicked() {
	getOkcPipelineSettings()->m_glyphPlaceMode = m_placeMode;
	PipelineManager* pm = m_mainWnd->getPipelineManager();
	ViewManager * vm = m_mainWnd->getViewManager();
	OkcPipeline* pl = getOkcPipeline();
	Transformation* mainT = pl->getMainTransformation();
	XmdvTool::PLTYPE plType = pl->getPipelineType();
	assert (plType==XmdvTool::PLTYPE_FLATNORMAL ||
			plType==XmdvTool::PLTYPE_SBB);

	SortRowsOperator* sortO = pl->getSortRowsOperator();
	PCADeriveOperator* pcaO = pl->getPCADeriveOperator();

	// Setup the pipeline in terms of the current glyph place mode
	switch (m_placeMode) {
	case XmdvTool::GLYPHPLACE_ORIGINAL :
	{
		// Disable the sort operator
		// Disable the PCA operator
		switch (plType) {
		case XmdvTool::PLTYPE_FLATNORMAL :
			mainT->enableOperator(PL_FN_OP_SORT, false);
			mainT->enableOperator(PL_FN_OP_PCADERIVE, false);
			break;
		case XmdvTool::PLTYPE_SBB :
			mainT->enableOperator(PL_SBB_OP_SORT, false);
			mainT->enableOperator(PL_SBB_OP_PCADERIVE, false);
			break;
		default:
			// The code should not go to here;
			return;
		}
		// Reassemble the pipeline
		pm->assemble(m_pipelineID, sortO);
		// If the current views include the glyphs, refresh the output
		if (vm->willOuputToVis(m_pipelineID, XmdvTool::VISTYPE_GL)) {
			vm->refreshDisplayByPID(m_pipelineID);
		}
	}
	break;
	case XmdvTool::GLYPHPLACE_ORDERED :
	{
		if (m_selectedDimNo<0 || m_selectedDimNo>=ui.listWidget_dims->count()) {
			// If users did not select anything, return immediately
			return;
		}
		// Enable the sort operator
		// Disable the PCA operator
		switch (plType) {
		case XmdvTool::PLTYPE_FLATNORMAL :
			mainT->enableOperator(PL_FN_OP_SORT, true);
			mainT->enableOperator(PL_FN_OP_PCADERIVE, false);
			break;
		case XmdvTool::PLTYPE_SBB :
			mainT->enableOperator(PL_SBB_OP_SORT, true);
			mainT->enableOperator(PL_SBB_OP_PCADERIVE, false);
			break;
		default:
			// The code should not go to here;
			return;
		}
		// Get the sort operator
		// Setup the sort Operator
		sortO->setSortWay (XmdvTool::OKCDIM_ORIGINAL);
		sortO->setSortDimNo (m_selectedDimNo);
		sortO->setSortOrder (m_sortOrder);
		// Reassemble the pipeline
		pm->assemble(m_pipelineID, sortO);
		// If the current views include the glyphs, refresh the output
		if (vm->willOuputToVis(m_pipelineID, XmdvTool::VISTYPE_GL)) {
			vm->refreshDisplayByPID(m_pipelineID);
		}
	}
	break;
	case XmdvTool::GLYPHPLACE_DATA_DRIVEN :
	{
		if (m_XDimNumber<0
				|| m_XDimNumber>=ui.listWidget_dims->count()
				|| m_YDimNumber<0
				|| m_YDimNumber>=ui.listWidget_dims->count()) {
			// If users did not select anything for X dim or Y dim, return immediately
			return;
		}
		// Disable the sort operator
		// Disable the PCA operator
		switch (plType) {
		case XmdvTool::PLTYPE_FLATNORMAL :
			mainT->enableOperator(PL_FN_OP_SORT, false);
			mainT->enableOperator(PL_FN_OP_PCADERIVE, false);
			break;
		case XmdvTool::PLTYPE_SBB :
			mainT->enableOperator(PL_SBB_OP_SORT, false);
			mainT->enableOperator(PL_SBB_OP_PCADERIVE, false);
			break;
		default:
			// The code should not go to here;
			return;
		}
		OkcPipeline* okcpl = getOkcPipeline();
		OkcVisualMap* okcvm = okcpl->getOkcVisualMap();
		GlyphPlace* gp = okcvm->getGlyphPlace();
		gp->setDimX(XmdvTool::OKCDIM_ORIGINAL);
		gp->setDimXNo(m_XDimNumber);
		gp->setDimY(XmdvTool::OKCDIM_ORIGINAL);
		gp->setDimYNo(m_YDimNumber);
		// Reassemble the pipeline
		pm->assemble(m_pipelineID, sortO);
		// If the current views include the glyphs, refresh the output
		if (vm->willOuputToVis(m_pipelineID, XmdvTool::VISTYPE_GL)) {
			vm->refreshDisplayByPID(m_pipelineID);
		}
	}
	break;
	case XmdvTool::GLYPHPLACE_DERIVED :
	{
		if (m_XDimNumber<0 || m_YDimNumber<0 ) {
			// If users did not select anything for X dim or Y dim, return immediately
			return;
		}
		// Disable the sort operator
		// Enable the PCA operator
		switch (plType) {
		case XmdvTool::PLTYPE_FLATNORMAL :
			mainT->enableOperator(PL_FN_OP_SORT, false);
			mainT->enableOperator(PL_FN_OP_PCADERIVE, true);
			break;
		case XmdvTool::PLTYPE_SBB :
			mainT->enableOperator(PL_SBB_OP_SORT, false);
			mainT->enableOperator(PL_SBB_OP_PCADERIVE, true);
			break;
		default:
			// The code should not go to here;
			return;
		}
		OkcPipeline* okcpl = getOkcPipeline();
		OkcVisualMap* okcvm = okcpl->getOkcVisualMap();
		GlyphPlace* gp = okcvm->getGlyphPlace();
		gp->setDimX(m_XDim);
		gp->setDimXNo(m_XDimNumber);
		gp->setDimY(m_YDim);
		gp->setDimYNo(m_YDimNumber);
		// Reassemble the pipeline
		if (okcpl->needRunPCA()) {
			// If the PCA algorithm has been run once,
			// assemble the pipeline from the pca operator
			pm->assemble(m_pipelineID, pcaO);
			okcpl->setNeedRunPCA(false);
		} else {
			// If the PCA algorithm has been run once,
			// we only need to do visual mapping
			pm->assemble(m_pipelineID, -1);
		}
		// If the current views include the glyphs, refresh the output
		if (vm->willOuputToVis(m_pipelineID, XmdvTool::VISTYPE_GL)) {
			vm->refreshDisplayByPID(m_pipelineID);
		}
	}
	break;
	}
}
