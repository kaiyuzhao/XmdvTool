#include <vector>
#include <algorithm>
#include <string>
using namespace std;

 #include <QtWidgets/QMessageBox>
#include <QFileInfo>
#include <new>
#include <cstdlib>
#include <assert.h>

#include "PipelineManager.h"

#include "main/XmdvTool.h"
#include "main/XmdvToolMainWnd.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "data/storage/CardStorage.h"
#include "operator/brush/Brush.h"
#include "operator/brush/BrushOperator.h"
#include "operator/dimooo/DimOOOOperator.h"
#include "operator/rowindex/SortRowsOperator.h"
#include "operator/diagonal/HistogramOperator.h"
#include "operator/diagonal/SortRowsOnAllDimOperator.h"
#include "operator/sbb/ClusterOperator.h"
#include "operator/sbb/SBBOperator.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "operator/pcaderive/PCADeriveOperator.h"
#include "operator/dimreduct/DimTreeOperator.h"
#include "operator/dimreduct/DimReductViewOperator.h"
#include "visualmap/VisualMapResult.h"
#include "visualmap/multidim/OkcVisualMap.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "visualmap/multidim/GlyphPlace.h"
#include "view/ViewManager.h"
#include "view/ViewDisplay.h"
#include "view/ViewWindow.h"
#include "pipeline/Pipeline.h"
#include "pipeline/Transformation.h"
#include "pipeline/multidim/OkcPipeline.h"
#include "pipeline/multidim/OkcPipelineSettings.h"
#include "pipeline/multidim/SBBPipeline.h"
#include "pipeline/multidim/DimRPipeline.h"
#include "util/exception/ReadOkcException.h"
#include "visualmap/multidim/ScatterVisAttr.h"

PipelineManager::PipelineManager(XmdvToolMainWnd* wnd)
{
	scatterVisAttr = 0;
	this->setMainWnd(wnd);
	m_pipelines.clear();
	m_visualMapResults.clear();
	m_origDatasets.clear();
	m_allOperators.clear();
	m_allAssisInputData.clear();
}

PipelineManager::~PipelineManager()
{
	int i;
	for (i=0; i<(int)m_pipelines.size(); i++) {
		SAFE_DELETE(m_pipelines[i]);
	}
	for (i=0; i<(int)m_visualMapResults.size(); i++) {
		SAFE_DELETE(m_visualMapResults[i]);
	}
	for (i=0; i<(int)m_origDatasets.size(); i++) {
		SAFE_DELETE(m_origDatasets[i]);
	}
	// release memory occupied by operator objects,
	// so each transformation does not need to release memory
	// of operator objects which belong to them.
	for (i=0; i<(int)m_allOperators.size(); i++) {
		SAFE_DELETE(m_allOperators[i]);
	}
	// release memory occupied by assistant input data objects,
	for (i=0; i<(int)m_allAssisInputData.size(); i++) {
		SAFE_DELETE(m_allAssisInputData[i]);
	}
}

void PipelineManager::setMainWnd(XmdvToolMainWnd* wnd) {
	this->mainWnd = wnd;
}

XmdvToolMainWnd* PipelineManager::getMainWnd() {
	return mainWnd;
}

ViewManager* PipelineManager::getViewManager() {
	return getMainWnd()->getViewManager();
}

Pipeline* PipelineManager::getPipeline(int pID) {
	return m_pipelines[pID];
}

int PipelineManager::getPipelineID(Pipeline* pl) {
	vector<Pipeline*>::iterator it;
	// Search the specified pipeline pointer in the vector m_pipelines.
	it = find( m_pipelines.begin(), m_pipelines.end(), pl);
	if ( it==m_pipelines.end() ) {
		// If not found
		return -1;
	} else {
		return it-m_pipelines.begin();
	}
}

int PipelineManager::getPipelinesCount() {
	return m_pipelines.size();
}

// Delete the pipeline specified by the pID
void PipelineManager::deletePipeline(int pID) {
	// Before deleting the pipeline, we check whether the operators
	// are being shared by other pipelines.  If no, we delete those operators too.
	tryDeleteOperators(pID);
	delete m_pipelines[pID];
	m_pipelines[pID] = 0;
}

vector<int> PipelineManager::getPipelinesByData(Data* origData) {
	int i;
	vector<int> result;
	for ( i=0; i<(int)m_pipelines.size(); i++ ) {
		Pipeline* pl = m_pipelines[i];
		// If this is null pointer, skip it
		if ( !pl ) {
			continue;
		}
		if ( pl->getOrigDataset() == origData ) {
			result.push_back(i);
		}
	}

	return result;
}


int PipelineManager::getActivePipelineID() {
	ViewManager* vm = getViewManager();
	ViewWindow* activeWin = vm->getActiveViewWindow();
	return activeWin->getPipelineID();
}

Pipeline* PipelineManager::getActivePipeline() {
	return getPipeline(getActivePipelineID());
}


void PipelineManager::openFile(QString fileName, bool needRefresh, XmdvTool::VISTYPE visType) {

	ViewManager* vm = this->mainWnd->getViewManager();

	OkcData* origDataset = new OkcData();
	try {
		bool success = origDataset->read(fileName.toStdString().c_str());
		if (!success) {
			QMessageBox::warning(this->mainWnd,
					"Invalid input file!",
					"This is not a valid okc file!");
			return;
		}
	} catch (ReadOkcException& e) {
		QMessageBox::warning(this->mainWnd,
				"Invalid input file!",
				e.what());
		return;
	}
	// add to origDataset list
	// to allow PipelineManager destructor
	// to release memory occupied by this data
	addOrigDatasets((Data*)origDataset);

	// Create a new pipeline
	Operator** operatorArr;
	// prepare the main transformation
	Pipeline *newline = new OkcPipeline();
	m_pipelines.push_back(newline);
	int pipelineID = m_pipelines.size()-1;
	// set the pipeline ID
	newline->setPipelineID(pipelineID);
	// set the pipeline type
	newline->setPipelineType(XmdvTool::PLTYPE_FLATNORMAL);
	// set the input dataset
	newline->setOrigDataset((Data*)origDataset);
	// set the pipeline manager
	newline->setPipelineManager(this);
	// set the pipeline settings
	OkcPipelineSettings* plSettings = new OkcPipelineSettings();
	newline->setPipelineSettings(plSettings);

	// Create all operators
	//
	// Create an instance of PCADeriveOperator
	PCADeriveOperator* pcaQ = new PCADeriveOperator();
	//create an instance of BrushOperator
	BrushOperator* brushQ = new BrushOperator();
	//create an instance of SortRowsOperator
	SortRowsOperator* sortRowsQ = new SortRowsOperator();
	sortRowsQ->setSortWay(XmdvTool::OKCDIM_ORIGINAL);
	sortRowsQ->setSortDimNo(0);
	sortRowsQ->setSortOrder(XmdvTool::SORTORDER_INCREASE);
	//create an instance of DimOOOOperator
	DimOOOOperator* dimoooQ = new DimOOOOperator();
	dimoooQ->initMap(origDataset->getDimSize());

	// create an instance of HistogramOperator
	HistogramOperator* histogramQ = new HistogramOperator();
	histogramQ->setHistogramSize(17);

	// create an instance of SortRowsOnAllDimOperator
	SortRowsOnAllDimOperator* sortRowAllDimQ = new SortRowsOnAllDimOperator();


	operatorArr = new Operator*[PL_FN_OP_CNT];
	operatorArr[PL_FN_OP_PCADERIVE] = pcaQ;
	operatorArr[PL_FN_OP_BRUSH] = brushQ;
	operatorArr[PL_FN_OP_SORT] = sortRowsQ;
	operatorArr[PL_FN_OP_DIMOOO] = dimoooQ;
	operatorArr[PL_FN_OP_HISTOGRAM] = histogramQ;
	operatorArr[PL_FN_OP_SORTROWALLDIM] = sortRowAllDimQ;
	newline->m_mainTransformation->setOperators(PL_FN_OP_CNT, operatorArr);
	delete []operatorArr;
	//disable the SortOperator because the default setting is not sorting
	//newline->m_mainTransformation->enableOperator(PL_FN_OP_SORT, false);
	//disable the PCA operator because the default settings does not need PCA
	newline->m_mainTransformation->enableOperator(PL_FN_OP_PCADERIVE, false);
	//newline->m_mainTransformation->enableOperator(PL_FN_OP_HISTOGRAM, false);
	//newline->m_mainTransformation->enableOperator(PL_FN_OP_SORTROWALLDIM, false);
	// add to operator list to allow PipelineManager destructor
	// to release memory occupied by these operators
	this->addOperator((Operator*)pcaQ);
	this->addOperator((Operator*)brushQ);
	this->addOperator((Operator*)sortRowsQ);
	this->addOperator((Operator*)dimoooQ);
	this->addOperator((Operator*)histogramQ);
	this->addOperator((Operator*)sortRowAllDimQ);

	// prepare the brush storage
	Brush brush;
	brush.Init(origDataset);
	OkcData* brushStorage = brush.toOkcDataStorage();
	//brushQ->setBrushStorage(brushStorage);
	// add to operator list to allow PipelineManager destructor
	// to release memory occupied by this data
	this->addAssisInputData((Data*)brushStorage);

	// prepare the cardinality storage
	CardStorage* cardStorage = CardStorage::createCardStorageFromOkcData(origDataset);
	this->addAssisInputData((Data*)cardStorage);

	// prepare the assistant transformation
	//
	// We have two assistant transformations
	newline->m_assisDataType.resize(PL_FN_ASSIS_CNT);
	newline->m_assisInputData.resize(PL_FN_ASSIS_CNT);
	newline->m_assisTransformations.resize(PL_FN_ASSIS_CNT);
	//
	//set the first assistant transformation for brush storage
	//
	newline->m_assisDataType[PL_FN_ASSIS_BRUSH_INDEX] = XmdvTool::ASSISDATA_BRUSH;
	// set the assistant input data
	// the first assistant input data is brushStorage
	newline->m_assisInputData[PL_FN_ASSIS_BRUSH_INDEX] = (Data*)brushStorage;
	// The operator array
	operatorArr = new Operator*[PL_FN_ASSIS_BRUSH_OP_CNT];
	operatorArr[PL_FN_ASSIS_BRUSH_OP_DIMOOO] = dimoooQ;
	//set the assistant transformation
	newline->m_assisTransformations[PL_FN_ASSIS_BRUSH_INDEX] = new Transformation();
	newline->m_assisTransformations[PL_FN_ASSIS_BRUSH_INDEX]->setOperators(PL_FN_ASSIS_BRUSH_OP_CNT, operatorArr);
	delete []operatorArr;
	//
	//set the second assistant transformation for cardinality storage
	//
	newline->m_assisDataType[PL_FN_ASSIS_CARD_INDEX] = XmdvTool::ASSISDATA_CARD;
	// set the assistant input data
	// the assistant input data is cardStorage
	newline->m_assisInputData[PL_FN_ASSIS_CARD_INDEX] = (Data*)cardStorage;
	// The operator array
	operatorArr = new Operator*[PL_FN_ASSIS_CARD_OP_CNT];
	operatorArr[PL_FN_ASSIS_CARD_OP_DIMOOO] = dimoooQ;
	//set the assistant transformation
	newline->m_assisTransformations[PL_FN_ASSIS_CARD_INDEX] = new Transformation();
	newline->m_assisTransformations[PL_FN_ASSIS_CARD_INDEX]->setOperators(PL_FN_ASSIS_CARD_OP_CNT, operatorArr);
	delete []operatorArr;

	OkcVisualMap* newVisualMap = new OkcVisualMap();
	newline->m_visualMap = newVisualMap;
	newline->m_visualMap->setPipeline(newline);
	GlyphPlace* glyphPlace = new GlyphPlace();
	// The default mode for glyph placement is ordering by the first dimension
	//glyphPlace->setGlyphPlaceMode(XmdvTool::GLYPHPLACE_ORDERED);
	newVisualMap->setGlyphPlace(glyphPlace);

	SAFE_DELETE(scatterVisAttr);
	scatterVisAttr = new ScatterVisAttr();
	newVisualMap->setScatterVisAttr(scatterVisAttr);

	// use QFileInfo to remove dir name from the file name
	ViewWindow* viewWin = vm->createViewWindow(
			pipelineID, QFileInfo( QString(fileName) ).baseName (), visType );

	/*
	 * Assembling pipeline must be put after creating the ViewWindow
	 * because the class VisualMap will do different things based on
	 * the type of ViewWindow
	 */
	// The vmr always has the same position as the pipeline
	VisualMapResult *vmr = newline->assemble(0);
	m_visualMapResults.push_back(vmr);

	// set the flat mode pipeline ID associated with this view
	viewWin->setFlatPipelineID(pipelineID);
	viewWin->getViewDisplay()->setVisualMapResult(vmr);
	if (needRefresh) {
		viewWin->updateWindow();
	}

}

int PipelineManager::createSBBPipeline(OkcData* origData) {
	int pipelineID;

	// create a new pipeline with the type PLTYPE_SBB
	SBBPipeline *newline = new SBBPipeline();
	m_pipelines.push_back(newline);
	// Push a empty visual map result,
	// which ensures the vmr has the same position as the pipeline.
	// We will replace it with a non-empty one after assembling the pipeline
	OkcVisualMapResult *vmr = new OkcVisualMapResult();
	m_visualMapResults.push_back(vmr);

	pipelineID = m_pipelines.size()-1;
	// set the pipeline ID
	newline->setPipelineID(pipelineID);
	// set the pipeline type
	newline->setPipelineType(XmdvTool::PLTYPE_SBB);
	// set the input dataset
	newline->setOrigDataset((Data*)origData);
	// set the pipeline manager
	newline->setPipelineManager(this);
	// set the pipeline settings
	OkcPipelineSettings* plSettings = new OkcPipelineSettings();
	newline->setPipelineSettings(plSettings);

	// create an instance of HierDisplayInformation
	HierDisplayInformation* hierInfo = new HierDisplayInformation();
	newline->setHierInfo(hierInfo);

	// Set the operators for the pipeline
	//
	//create an instance of ClusterOperator
	ClusterOperator* clusterQ = new ClusterOperator();
	//clusterQ->setHierDisplayInformation(hierInfo);

	//create an instance of SBBOperator
	SBBOperator* SBBQ = new SBBOperator();
	// Clear the change flag for the SBB Operator,
	// so the SBB Operator will not modify the flag for nodes
	// because this has been done by cluster operator
	SBBQ->clearChangeFlag();

	// Create an instance of PCADeriveOperator
	PCADeriveOperator* pcaQ = new PCADeriveOperator();

	//create an instance of SortRowsOperator
	SortRowsOperator* sortRowsQ = new SortRowsOperator();
	sortRowsQ->setSortWay(XmdvTool::OKCDIM_ORIGINAL);
	sortRowsQ->setSortDimNo(0);
	sortRowsQ->setSortOrder(XmdvTool::SORTORDER_INCREASE);

	//create an instance of DimOOOOperator
	DimOOOOperator* dimoooQ = new DimOOOOperator();
	dimoooQ->initMap(origData->getDimSize());

	Operator** operatorArr;

	operatorArr = new Operator*[PL_SBB_OP_CNT];
	operatorArr[PL_SBB_OP_CLUSTER] = clusterQ;
	operatorArr[PL_SBB_OP_SBB] = SBBQ;
	operatorArr[PL_SBB_OP_PCADERIVE] = pcaQ;
	operatorArr[PL_SBB_OP_SORT] = sortRowsQ;
	operatorArr[PL_SBB_OP_DIMOOO] = dimoooQ;
	newline->m_mainTransformation->setOperators(PL_SBB_OP_CNT, operatorArr);
	delete []operatorArr;

	// add to operator list to allow PipelineManager destructor
	// to release memory occupied by these operators
	this->addOperator((Operator*)clusterQ);
	this->addOperator((Operator*)SBBQ);
	this->addOperator((Operator*)pcaQ);
	this->addOperator((Operator*)sortRowsQ);
	this->addOperator((Operator*)dimoooQ);

	// prepare the cardinality storage
	CardStorage* cardStorage = CardStorage::createCardStorageFromOkcData(origData);
	this->addAssisInputData((Data*)cardStorage);

	// prepare the assistant transformation
	//
	// We have two assistant transformations
	newline->m_assisDataType.resize(PL_SBB_ASSIS_CNT);
	newline->m_assisInputData.resize(PL_SBB_ASSIS_CNT);
	newline->m_assisTransformations.resize(PL_SBB_ASSIS_CNT);
	//
	//set the first assistant transformation for cardinality storage
	//
	newline->m_assisDataType[PL_SBB_ASSIS_CARD_INDEX] = XmdvTool::ASSISDATA_CARD;
	// set the assistant input data
	// the assistant input data is cardStorage
	newline->m_assisInputData[PL_SBB_ASSIS_CARD_INDEX] = (Data*)cardStorage;
	// The operator array
	operatorArr = new Operator*[PL_SBB_ASSIS_CARD_OP_CNT];
	operatorArr[PL_SBB_ASSIS_CARD_OP_DIMOOO] = dimoooQ;
	//set the assistant transformation
	newline->m_assisTransformations[PL_SBB_ASSIS_CARD_INDEX] = new Transformation();
	newline->m_assisTransformations[PL_SBB_ASSIS_CARD_INDEX]->setOperators(PL_SBB_ASSIS_CARD_OP_CNT, operatorArr);
	delete []operatorArr;


	// Add the Visual Mapping object
	OkcVisualMap* newVisualMap = new OkcVisualMap();
	newline->m_visualMap = newVisualMap;
	newline->m_visualMap->setPipeline(newline);
	GlyphPlace* glyphPlace = new GlyphPlace();
	//glyphPlace->setGlyphPlaceMode(XmdvTool::GLYPHPLACE_ORIGINAL);
	newVisualMap->setGlyphPlace(glyphPlace);
	newVisualMap->setGlyphShape(XmdvTool::GLYPHSHAPE_STAR);

	SAFE_DELETE(scatterVisAttr);
	scatterVisAttr = new ScatterVisAttr();
	newVisualMap->setScatterVisAttr(scatterVisAttr);

	return pipelineID;
}

int PipelineManager::createDimRPipeline(OkcData* origData) {
	int pipelineID;

	// create a new pipeline with the type PLTYPE_SBB
	DimRPipeline *newline = new DimRPipeline();
	m_pipelines.push_back(newline);
	// Push a empty visual map result,
	// which ensures the vmr has the same position as the pipeline.
	// We will replace it with a non-empty one after assembling the pipeline
	OkcVisualMapResult *vmr = new OkcVisualMapResult();
	m_visualMapResults.push_back(vmr);

	pipelineID = m_pipelines.size()-1;
	// set the pipeline ID
	newline->setPipelineID(pipelineID);
	// set the pipeline type
	newline->setPipelineType(XmdvTool::PLTYPE_DIMR);
	// set the input dataset
	newline->setOrigDataset((Data*)origData);
	// set the pipeline manager
	newline->setPipelineManager(this);
	// set the pipeline settings
	OkcPipelineSettings* plSettings = new OkcPipelineSettings();
	newline->setPipelineSettings(plSettings);

	// Set the operators for the pipeline
	//
	//create an instance of DimTreeOperator
	DimTreeOperator* dimTreeQ = new DimTreeOperator();
	//clusterQ->setHierDisplayInformation(hierInfo);

	//create an instance of DimReductViewOperator
	DimReductViewOperator* drViewQ = new DimReductViewOperator();

	//create an instance of DimOOOOperator
	DimOOOOperator* dimoooQ = new DimOOOOperator();
	dimoooQ->initMap(origData->getDimSize());

	//create an instance of BrushOperator
	BrushOperator* brushQ = new BrushOperator();

	// create an instance of HistogramOperator
	HistogramOperator* histogramQ = new HistogramOperator();
	histogramQ->setHistogramSize(17);

	// create an instance of SortRowsOnAllDimOperator
	SortRowsOnAllDimOperator* sortRowAllDimQ = new SortRowsOnAllDimOperator();

	Operator** operatorArr;

	operatorArr = new Operator*[PL_DIMR_OP_CNT];
	operatorArr[PL_DIMR_OP_DIMTREE] = dimTreeQ;
	operatorArr[PL_DIMR_OP_DRVIEW] = drViewQ;
	operatorArr[PL_DIMR_OP_DIMOOO] = dimoooQ;
	operatorArr[PL_DIMR_OP_BRUSH] = brushQ;
	operatorArr[PL_DIMR_OP_HISTOGRAM] = histogramQ;
	operatorArr[PL_DIMR_OP_SORTROWALLDIM] = sortRowAllDimQ;
	newline->m_mainTransformation->setOperators(PL_DIMR_OP_CNT, operatorArr);
	delete []operatorArr;

	// add to operator list to allow PipelineManager destructor
	// to release memory occupied by these operators
	this->addOperator((Operator*)dimTreeQ);
	this->addOperator((Operator*)drViewQ);
	this->addOperator((Operator*)dimoooQ);
	this->addOperator((Operator*)brushQ);
	this->addOperator((Operator*)histogramQ);
	this->addOperator((Operator*)sortRowAllDimQ);

	// prepare the brush storage
	Brush brush;
	brush.Init(origData);
	OkcData* brushStorage = brush.toOkcDataStorage();
	// add to operator list to allow PipelineManager destructor
	// to release memory occupied by this data
	this->addAssisInputData((Data*)brushStorage);

	// prepare the cardinality storage
	CardStorage* cardStorage = CardStorage::createCardStorageFromOkcData(origData);
	this->addAssisInputData((Data*)cardStorage);

	// prepare the assistant transformation
	//
	// We have two assistant transformations
	newline->m_assisDataType.resize(PL_DIMR_ASSIS_CNT);
	newline->m_assisInputData.resize(PL_DIMR_ASSIS_CNT);
	newline->m_assisTransformations.resize(PL_DIMR_ASSIS_CNT);

	//
	//set the first assistant transformation for brush storage
	//
	newline->m_assisDataType[PL_DIMR_ASSIS_BRUSH_INDEX] = XmdvTool::ASSISDATA_BRUSH;
	// set the assistant input data
	// the first assistant input data is brushStorage
	newline->m_assisInputData[PL_DIMR_ASSIS_BRUSH_INDEX] = (Data*)brushStorage;
	// The operator array
	operatorArr = new Operator*[PL_DIMR_ASSIS_BRUSH_OP_CNT];
	operatorArr[PL_DIMR_ASSIS_BRUSH_OP_DIMOOO] = dimoooQ;
	//set the assistant transformation
	newline->m_assisTransformations[PL_DIMR_ASSIS_BRUSH_INDEX] = new Transformation();
	newline->m_assisTransformations[PL_DIMR_ASSIS_BRUSH_INDEX]->setOperators(PL_FN_ASSIS_BRUSH_OP_CNT, operatorArr);
	delete []operatorArr;

	//
	//set the second assistant transformation for cardinality storage
	//
	newline->m_assisDataType[PL_DIMR_ASSIS_CARD_INDEX] = XmdvTool::ASSISDATA_CARD;
	// set the assistant input data
	// the assistant input data is cardStorage
	newline->m_assisInputData[PL_DIMR_ASSIS_CARD_INDEX] = (Data*)cardStorage;
	// The operator array
	operatorArr = new Operator*[PL_DIMR_ASSIS_CARD_OP_CNT];
	operatorArr[PL_DIMR_ASSIS_CARD_OP_DIMOOO] = dimoooQ;
	//set the assistant transformation
	newline->m_assisTransformations[PL_DIMR_ASSIS_CARD_INDEX] = new Transformation();
	newline->m_assisTransformations[PL_DIMR_ASSIS_CARD_INDEX]->setOperators(PL_DIMR_ASSIS_CARD_OP_CNT, operatorArr);
	delete []operatorArr;


	// Add the Visual Mapping object
	OkcVisualMap* newVisualMap = new OkcVisualMap();
	newline->m_visualMap = newVisualMap;
	newline->m_visualMap->setPipeline(newline);
	// Add the GlyphPlace object for this pipeline
	GlyphPlace* glyphPlace = new GlyphPlace();
	newVisualMap->setGlyphPlace(glyphPlace);
	// Add the ScatterVisAttr object for this pipeline
	SAFE_DELETE(scatterVisAttr);
	scatterVisAttr = new ScatterVisAttr();
	newVisualMap->setScatterVisAttr(scatterVisAttr);

	return pipelineID;

}

void PipelineManager::assembleAll() {
	// Verify that the vector m_pipelines is not empty
	assert(m_pipelines.size()>0);

	int k;
	// Release all of objects in the vector m_assembledResults
	if (m_visualMapResults.size()>0) {
		for (k=0; k<(int)m_visualMapResults.size(); k++) {
			if (m_visualMapResults[k]) {
				delete m_visualMapResults[k];
				m_visualMapResults[k] = 0;
			}
		}
	}
	m_visualMapResults.clear();
	m_visualMapResults.resize(m_pipelines.size());

	for (k=0; k<(int)m_pipelines.size(); k++) {
		// call assemble function of each pipeline
		// and create a new object
		VisualMapResult *vmr = m_pipelines[k]->assemble(0);
		m_visualMapResults[k] = vmr;
	}
}

void PipelineManager::assemble(int i, int startOperatorNo=-1) {
	assert(i>=0);
	assert(i<(int)m_pipelines.size());
	assert(m_pipelines.size()==m_visualMapResults.size());

	try {
		SAFE_DELETE(m_visualMapResults[i]);
		VisualMapResult *vmr = m_pipelines[i]->assemble(startOperatorNo);
		m_visualMapResults[i] = vmr;
	} catch (bad_alloc& ba) {
		QMessageBox::critical(mainWnd,
				"Failure to request memory",
				"XmdvTool failed to request enough memory to visualize "
				"the current dataset and has to exit.  Please report "
				"this problem to xmdv@cs.wpi.edu with the description "
				"of your dataset and system configuration.  Thank you! "
				"--Xmdv Group");
		exit(1);
	}
}

void PipelineManager::updateAssisInput(int pID, Data* output, XmdvTool::ASSISDATATYPE type) {
	m_pipelines[pID]->updateAssisInput(output, type);
}

void PipelineManager::assemble(int i, Operator* oper) {
	assert(i>=0);
	assert(i<(int)m_pipelines.size());
	assert(m_pipelines.size()==m_visualMapResults.size());

	try {
		SAFE_DELETE(m_visualMapResults[i]);
		VisualMapResult *vmr = m_pipelines[i]->assemble(oper);
		m_visualMapResults[i] = vmr;
	} catch (bad_alloc& ba) {
		QMessageBox::critical(mainWnd,
				"Failure to request memory",
				"XmdvTool failed to request enough memory to visualize "
				"the current dataset and has to exit.  Please report "
				"this problem to xmdv@cs.wpi.edu with the description "
				"of your dataset and system configuration.  Thank you! "
				"--Xmdv Group");
		exit(1);
	}
}

VisualMapResult* PipelineManager::getVisualMapResult(int i) {
	// verify i is an appropriate value
	assert(i>=0);
	assert((int)m_visualMapResults.size()>i);
	return m_visualMapResults[i];
}

VisualMapResult* PipelineManager::getVisualMapResult(Pipeline* pl) {
	vector<Pipeline*>::iterator it;
	//Seach the position of pl in the vector m_pipelines
	it = find( m_pipelines.begin(), m_pipelines.end(), pl);
	if ( it == m_pipelines.end() ) {
		// If not exist
		return 0;
	} else {
		int pos =  it-m_pipelines.begin();
		// The vector m_visualMapResults has the same size as the the vector m_pipelines.
		// If not same, this pipeline has not been assembled.
		int n = m_visualMapResults.size();
		if ( pos < n ) {
			return m_visualMapResults[ pos ];
		} else {
			return 0;
		}
	}
}

void PipelineManager::addOrigDatasets(Data* data){
	vector<Data*>::iterator it, itBegin, itEnd;
	itBegin = m_origDatasets.begin();
	itEnd = m_origDatasets.end();
	it = find(itBegin, itEnd, data);
	if (it==itEnd) {
		// add to the vector only if it is not in the vector
		m_origDatasets.push_back(data);
	}
}

// Return the number of all datasets (including opened and closed)
int PipelineManager::getOrigDatasetsCount() {
	return m_origDatasets.size();
}

// Return the pointer to the data with index i.
// If the return value is 0, it means this data has been closed
Data* PipelineManager::getOrigDataset(int index) {
	assert( index>=0 );
	assert( index<(int)m_origDatasets.size() );

	return m_origDatasets[index];
}

void PipelineManager::deleteOrigDataset(Data* origData) {
	int i;
	for ( i=0; i<(int)m_origDatasets.size(); i++ ) {
		if ( m_origDatasets[i]==origData ) {
			m_origDatasets[i] = 0;
			break;
		}
	}

	delete origData;
}

void PipelineManager::addOperator(Operator* oper) {
	vector<Operator*>::iterator it, itBegin, itEnd;
	itBegin = m_allOperators.begin();
	itEnd = m_allOperators.end();
	it = find(itBegin, itEnd, oper);
	if (it==itEnd) {
		// add to the vector only if it is not in the vector
		m_allOperators.push_back(oper);
	}
}

void PipelineManager::tryDeleteOperators(int pID) {
	// This function will try to delete operators used by pID.  An operator of pID
	// will be deleted only if this operator is not used by any other pipelines.
	// Because operators are actually used by transformations, we first try to delete
	// the operators in the main transformation, and then try to do the same thing in
	// all of assistant transformations.  All actions to delete operators are done in
	// the function tryDeleteOperators(int pID, Transformation* t).
	Pipeline* pl = getPipeline(pID);

	// Delete the operators in the main transformation
	tryDeleteOperators(pID, pl->m_mainTransformation);
	int i;
	for (i=0; i<(int)pl->m_assisTransformations.size(); i++) {
		// Delete the operators in the assistant transformations
		tryDeleteOperators( pID, pl->m_assisTransformations[i] );
	}
}

void PipelineManager::tryDeleteOperators(int pID, Transformation* t) {
	// We will visit each operator in this transformation, and check whether
	// it is being used other pipelines.  If no, we will find this operator
	// in the vector m_allOperators, and then set the item in the vector to null.
	// Finally, we release the memory occupied by this operator using delete
	int i;
	for (i=0; i<(int)t->m_operatorList.size(); i++) {
		// Check whether this operator is used by other pipelines.
		// If no, we can safely delete it from the vector m_allOperators
		if ( !operatorUsedByOtherPipelines(t->m_operatorList[i], pID) ) {
			// Search the operator in the vector m_allOperators, and then set it to null
			vector<Operator*>::iterator it;
			it = find (m_allOperators.begin(), m_allOperators.end(), t->m_operatorList[i]);
			if ( it!=m_allOperators.end() ) {
				(*it) = 0;
				delete t->m_operatorList[i];
			}
		}
	}
}

bool PipelineManager::operatorUsedByOtherPipelines(Operator* op, int pID) {
	int i;
	for ( i=0; i<(int)m_pipelines.size(); i++ ) {
		Pipeline* pl = m_pipelines[i];
		// We do not check the pipeline pID itself and null pipelines
		if ( i==pID || !pl ) {
			continue;
		}
		if ( pl->hasOperator(op) ) {
			return true;
		}
	}

	return false;
}

void PipelineManager::addAssisInputData(Data* data) {
	vector<Data*>::iterator it, itBegin, itEnd;
	itBegin = m_allAssisInputData.begin();
	itEnd = m_allAssisInputData.end();
	it = find(itBegin, itEnd, data);
	if (it==itEnd) {
		// add to the vector only if it is not in the vector
		m_allAssisInputData.push_back(data);
	}
}
