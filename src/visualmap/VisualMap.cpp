#include "VisualMap.h"
#include "main/XmdvTool.h"
#include "visualmap/VisualMapResult.h"
#include "data/Data.h"
#include <vector>
using namespace std;

VisualMap::VisualMap() {
	this->m_input = 0;
	this->m_visualMapResult = 0;
}

VisualMap::VisualMap(Data* input) {
	this->m_input = input;
	this->m_visualMapResult = 0;
}

VisualMap::~VisualMap() {

};

void VisualMap::setPipeline(Pipeline* pipeline) {
	m_pipeline = pipeline;
}

Pipeline* VisualMap::getPipeline() {
	return m_pipeline;
}

void VisualMap::setInput(Data* input) {
	this->m_input = input;
}

Data* VisualMap::getInput() {
	return m_input;
}

void VisualMap::setAssisInput(int n, Data* assisInput[]) {
	int i;
	m_assisInput.resize(n);
	for (i=0; i<n; i++) {
		m_assisInput[i] = assisInput[i];
	}
}

void VisualMap::setSingleAssisInput(int i, Data* assisInput) {
	m_assisInput[i] = assisInput;
}

int VisualMap::getAssisInputSize() {
	return m_assisInput.size();
}

Data* VisualMap::getAssisInput(int i) {
	return m_assisInput[i];
}

void VisualMap::setAssisInputType(int n, XmdvTool::ASSISDATATYPE assisInputType[]) {
	int i;
	m_assisInputType.resize(n);
	for (i=0; i<n; i++) {
		m_assisInputType[i] = assisInputType[i];
	}
}

void VisualMap::setSingleAssisInputType(int i, XmdvTool::ASSISDATATYPE assisInputType) {
	m_assisInputType[i] = assisInputType;
}

XmdvTool::ASSISDATATYPE VisualMap::getAssisInputType(int i) {
	return m_assisInputType[i];
}

void VisualMap::doVisualMap() {
	if (this->m_input) {
		this->m_visualMapResult = new VisualMapResult();
	} else {
		this->m_visualMapResult = 0;
	}
}

VisualMapResult* VisualMap::getVisualMapResult() {
	return this->m_visualMapResult;
}

/*

void VisualMap::visualMapBasedOnBrush() {

	int dataSize = dataMapResult->getDataset()->data_size;
	int dimSize = dataMapResult->getDataset()->all_dims;
	ARR <double> curData(dimSize);
	//ARR <VisualAttribute*> allDataVisAttr = visualMapResult->getAllDataVisAttr();

	//VisualAttribute *tBrushVisAttr;
	for (int i=0; i<dataSize; i++){
		dataMapResult->getDisplayDataset()->GetAllData(curData,i);
		visualMapResult->setSingleDataVisAttr(visualMapResult->getInBrushVisAttr(), i);
		for (int j=0; j<dimSize; j++){
			if (curData[j]>visualMapResult->getBrush()->max[j]||curData[j]<visualMapResult->getBrush()->min[j]){
				visualMapResult->setSingleDataVisAttr(visualMapResult->getOutBrushVisAttr(), i);
				break;
			}

		}
		//tBrushVisAttr = all_data_VisAttr[i];
	}
	//visualMapResult->getBrush()->MapVisualBrush();

}
*/
