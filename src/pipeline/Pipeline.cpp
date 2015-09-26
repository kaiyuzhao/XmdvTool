/*
 * Pipeline.cpp
 *
 *  Created on: Jan 8, 2009
 *      Author: Zaixian Xie
 */

#include "pipeline/Pipeline.h"
#include "main/XmdvTool.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "operator/brush/BrushOperator.h"
#include "pipeline/PipelineManager.h"
#include "pipeline/Transformation.h"
#include "pipeline/PipelineSettings.h"
#include "pipeline/multidim/OkcPipelineSettings.h"
#include "visualmap/VisualMap.h"
#include "visualmap/multidim/OkcVisualMap.h"
#include "operator/Operator.h"
#include "operator/brush/BrushOperator.h"
#include "view/ViewManager.h"

#include <vector>
#include <algorithm>
#include <typeinfo>
#include <assert.h>
using namespace std;

Pipeline::Pipeline() {
	m_origDataset = 0;
	m_visualMap = 0;
	m_mainTransformation = new Transformation();
}

Pipeline::~Pipeline() {
	SAFE_DELETE(this->m_visualMap);
	SAFE_DELETE(this->m_mainTransformation);
	int i;
	for (i=0; i<(int)m_assisTransformations.size(); i++) {
		SAFE_DELETE(m_assisTransformations[i]);
	}
	SAFE_DELETE(this->m_plSettings);
}

// Return a string representation of pipeline type
// static function
QString Pipeline::getTypeString(XmdvTool::PLTYPE plType) {
	QString result;
	switch (plType) {
	case XmdvTool::PLTYPE_FLATNORMAL :
		result = QString( "Flat data mode");
		break;
	case XmdvTool::PLTYPE_SBB :
		result = QString( "Hierarchical data mode");
		break;
	case XmdvTool::PLTYPE_DIMR :
		result = QString( "Hierarchical dimension mode");
		break;
	}
	return result;
}

QStringList Pipeline::toStringList() {
	QStringList result;
	result << QString("Pipeline:");

	// Generate the type of the pipeline
	result << QString("-- Type : %1")
		.arg ( Pipeline::getTypeString( getPipelineType() ) );

	// Generate the input of this pipeline
	Data* inputData = getOrigDataset();
	OkcData* inputOkcData = dynamic_cast<OkcData*>( inputData );
	result << QString("-- Input : %1").arg( QString(inputOkcData->filename) ) ;

	// Generate the number of views
	ViewManager* vm = m_pipelineManager->getViewManager();
	int viewCount = vm->getViewWindowsCount( getPipelineID() );
	result << QString("-- The number of views : %1").arg(viewCount);

	return result;
}

void Pipeline::setPipelineManager(PipelineManager* pm) {
	m_pipelineManager = pm;
}

PipelineManager* Pipeline::getPipelineManager() {
	return m_pipelineManager;
}

void Pipeline::setPipelineID(int ID) {
	m_pipelineID = ID;
}

int Pipeline::getPipelineID() {
	return m_pipelineID;
}

void Pipeline::setPipelineType(XmdvTool::PLTYPE type) {
	m_pipelineType = type;
}

XmdvTool::PLTYPE Pipeline::getPipelineType() {
	return m_pipelineType;
}

void Pipeline::setPipelineSettings(PipelineSettings* plSettings) {
	m_plSettings = plSettings;
}

PipelineSettings* Pipeline::getPipelineSettings() {
	return m_plSettings;
}

void Pipeline::setOrigDataset(Data* data) {
	SAFE_DELETE(this->m_origDataset);
	this->m_origDataset = data;
}

Data* Pipeline::getOrigDataset() {
	return this->m_origDataset;
}

VisualMap* Pipeline::getVisualMap() {
	return this->m_visualMap;
}

void Pipeline::setupPipeline() {
	// Call the function setupTransformation() for each transformation
	// to set the pointer to this pipeline for each operator.
	m_mainTransformation->setupTransformation(this);
	int i, n=m_assisDataType.size();
	for (i=0; i<n; i++) {
		m_assisTransformations[i]->setupTransformation(this);
	}

}

void Pipeline::setupMainTransformation() {

}

Transformation* Pipeline::getMainTransformation() {
	return m_mainTransformation;
}

Transformation* Pipeline::getAssisTransformation(XmdvTool::ASSISDATATYPE dataType) {
	int i, n = m_assisDataType.size();
	for (i=0; i<n; i++) {
		if (m_assisDataType[i]==dataType) {
			return m_assisTransformations[i];
		}
	}
	return 0;
}

VisualMapResult* Pipeline::assemble(int startOperatorNo) {
	// Configure the pipeline before doing all of operators
	setupPipeline();

	// We first do all assistant transformations, and then do the
	// main transformations, because operators in the main transformations
	// need the result of assistant transformation.  For example,
	// the BrushOperator need the BrushStorage after the Operator DimOOOOperator
	// (dimension on/off/reordering).
	//
	// verifying these vectors for assistant data and
	// transformations should have the same sizes,
	// and then do all assistant transformations.
	assert(m_assisDataType.size()==m_assisInputData.size());
	assert(m_assisDataType.size()==m_assisTransformations.size());
	int i, n=m_assisDataType.size();
	if (n>0) {
		Data** assisResult = new Data*[n];
		XmdvTool::ASSISDATATYPE* assisResultType = new XmdvTool::ASSISDATATYPE[n];
		for (i=0; i<n; i++) {
			m_assisTransformations[i]->setInput(m_assisInputData[i]);
			assisResult[i] = m_assisTransformations[i]->doOperator(0);
			assisResultType[i] = m_assisDataType[i];
		}
		// Now the assistant input will go to visual map
		m_visualMap->setAssisInput(n, assisResult);
		m_visualMap->setAssisInputType(n, assisResultType);
		delete []assisResult;
		delete []assisResultType;
	}

	// Use the result from the assistant transformation to set the main transformation.
	setupMainTransformation();

	// Do the main transformation
	m_mainTransformation->setInput(m_origDataset);
	Data* operatorResult = m_mainTransformation->doOperator(startOperatorNo);
	// Now input will go to visual map
	m_visualMap->setInput(operatorResult);

	// if the last operator in the operator list is a BrushOperator
	// set the brush operator for m_visualMap
	BrushOperator* lastBrushO = m_mainTransformation->findLastBrushOperator();
	if ( lastBrushO ) {
		assert(typeid(*m_visualMap)==typeid(OkcVisualMap));
		OkcVisualMap* ovm = dynamic_cast<OkcVisualMap*>(m_visualMap);
		ovm->setBrushOperator(lastBrushO);
	}
	m_visualMap->doVisualMap();

	VisualMapResult* vmr = m_visualMap->getVisualMapResult();

	return vmr;
}


VisualMapResult* Pipeline::assemble(Operator* startOperator) {
	if (startOperator==NULL) {
		return assemble(-1);
	} else {
		// Find the operator in the operator list and call assemble(int)
		vector<Operator*>::iterator it, itBegin, itEnd;
		itBegin = m_mainTransformation->m_operatorList.begin();
		itEnd = m_mainTransformation->m_operatorList.end();
		it = find(itBegin, itEnd, startOperator);
		if (it==itEnd) {
			// Cannot find the specified operator in the operator list
			return NULL;
		} else {
			return assemble(it-itBegin);
		}
	}
}

void Pipeline::updateAssisInput(Data* output, XmdvTool::ASSISDATATYPE type) {
	vector<XmdvTool::ASSISDATATYPE>::iterator it, itBegin, itEnd;
	itBegin = m_assisDataType.begin();
	itEnd = m_assisDataType.end();
	it = find(itBegin, itEnd, type);
	if (it==itEnd) {
		// if this type does not exist, something wrong happened.
		assert(0);
	} else {
		// index is the location the assistant transformation
		int index = it-itBegin;
		m_assisTransformations[index]->updateAssisInput(output);
		// If this is an OkcData, we will use it to update the input of the assistant transformation.
		// Currently, all the input of the assistant transformation is OkcData.  In the future,
		// if other data type becomes the input of one assistant transformation, we need add more code here.
/*
		if ( typeid(*updatedInput)==typeid(OkcData) ) {
			OkcData* input = dynamic_cast<OkcData*>( m_assisInputData[index] );
			OkcData* upatedOkcData  = dynamic_cast<OkcData*>( updatedInput );
			(*input) = (*upatedOkcData);
		}
*/
	}
}

bool Pipeline::willOuputToVis(XmdvTool::VISTYPE visType) {
	PipelineManager* pm = getPipelineManager();
	ViewManager* vm = pm->getViewManager();
	return vm->willOuputToVis(getPipelineID(), visType);
}

bool Pipeline::hasOperator(Operator* op) {
	// First check the main transformation
	if ( m_mainTransformation->hasOperator(op) ) {
		// Return true if it contains this operator
		return true;;
	}

	// And then check the assistant transformations
	int i;
	for ( i=0; i<(int)m_assisTransformations.size(); i++ ) {
		if ( m_assisTransformations[i]->hasOperator(op) ) {
			// Return true if it contains this operator
			return true;;
		}
	}

	// If the program reaches here, this pipeline does not contain this operator
	return false;
}
