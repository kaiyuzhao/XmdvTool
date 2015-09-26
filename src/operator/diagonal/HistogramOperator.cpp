/*
 * HistogramOperator.cpp
 *
 *  Created on: Aug 14, 2009
 *      Author: zyguo
 */

#include "HistogramOperator.h"

#include "main/XmdvTool.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierHighlight.h"
#include "data/multidim/diagonal/HistogramModifier.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "operator/Operator.h"
#include <stdlib.h>
#include <typeinfo>
#include "datatype/VectorUtil.h"

HistogramOperator::HistogramOperator() {
}

HistogramOperator::~HistogramOperator() {
}

void HistogramOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(OkcData));
	Operator::setInput(input);
}

void HistogramOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
}

void HistogramOperator::setHistogramSize(int _histogram_size){
	this->histogram_size = _histogram_size;
}

bool HistogramOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input) == typeid(OkcData));
	OkcData* input = (OkcData*) m_input;

	// Create a new instance of class OkcData as output
	// or use the old one
	OkcData* output = new OkcData(input);
	m_output = dynamic_cast<Data*> (output);

	OkcDataModifierManager* manager = output->getOkcDataModifierManager();
	OkcDataModifier* modifier = manager->getOkcDataModifier(
			XmdvTool::MODIFIER_HISTOGRAM);

	HistogramModifier* histogramModifier;

	if (modifier) {
		// the modifier does exist
		histogramModifier = dynamic_cast<HistogramModifier*> (modifier);

		// calculate histogram information here
		this->initRequiredArr();
		histogramModifier->initRequiredArr(this->histogram_size);
		Calculate_Histogram_Data();
		setHistogramModifierArray(histogramModifier);
	} else {
		// the modifier does not exist
		histogramModifier = new HistogramModifier(manager->getOkcData());
		this->initRequiredArr();
		histogramModifier->initRequiredArr(this->histogram_size);
		Calculate_Histogram_Data();
		setHistogramModifierArray(histogramModifier);
		manager->addOkcDataModifier(XmdvTool::MODIFIER_HISTOGRAM,
				histogramModifier);
	}

	return true;
}

void HistogramOperator::initRequiredArr(){

	OkcData* okcdata = (OkcData*) m_input;

	int dimSize = okcdata->getDimSize();

	VectorUtil::init2DVector(frequency, dimSize, histogram_size);

	VectorUtil::init2DVector(marked_freq, dimSize, histogram_size);

	VectorUtil::init2DVector(histo_cut_points, dimSize, histogram_size + 1);

	max_frequency.assign( dimSize, 0 );

}

void HistogramOperator::Calculate_Histogram_Data() {
	Calculate_Histogram_Cutpoint();
	Calculate_Frequency();
}


// calculate all the cut points position and save in a one dimensional array (i.e. histo_cut_points)
void HistogramOperator::Calculate_Histogram_Cutpoint(){
	OkcData* okcdata = (OkcData*) m_input;

	std::vector<double> dim_min, dim_max;

	okcdata->getDimMinArr(dim_min);
	okcdata->getDimMaxArr(dim_max);
	int dim_num = okcdata->getDimSize();
	int histo_size = histogram_size;

	for(int i=0; i<dim_num; i++){
		double histo_bin_dis = (dim_max[i] - dim_min[i])/histo_size;
		for(int j=0; j<=histo_size; j++){
			histo_cut_points[i][j] = dim_min[i] + j*histo_bin_dis;
		}
	}
}


void HistogramOperator::getDimCutpoints(std::vector<double> &cutpoints, int dimIdx)
{
	int histo_size = histogram_size;
	assert( (int)cutpoints.size()==histo_size+1 );
	int i;
	for (i = 0; i < histo_size + 1; i++){
		cutpoints[i] = histo_cut_points[dimIdx][i];
	}
}

void HistogramOperator::getBinIdxForAllDim(std::vector<double> &binIdx, int dataIdx){
	std::vector<double> buf;

	OkcData* okcdata = (OkcData*) m_input;
	int dim_size = okcdata->getDimSize();
	buf.resize(dim_size);


	okcdata->getData(buf, dataIdx);

	int histo_size = histogram_size;
	std::vector<double> cutpoints(histo_size+1);
	for(int i=0; i<dim_size; i++){
		getDimCutpoints(cutpoints, i);
		for(int j=1; j<histo_size+1; j++){
			if(buf[i]<cutpoints[j]){
				binIdx[i] = j-1;
				break;
			}
		}
	}
}

void HistogramOperator::Calculate_Frequency() {

	OkcData* okcdata = (OkcData*) m_input;
	int dim_num = okcdata->getDimSize();
	int histo_size = histogram_size;

	int data_size = okcdata->getDataSize();

	OkcData* output = (OkcData*) m_output;
	OkcDataModifierManager* manager = output->getOkcDataModifierManager();
	OkcDataModifier* hightlight_modifier = manager->getOkcDataModifier(
				XmdvTool::MODIFIER_HIGHLIGHT);

	OkcDataModifierHighlight* okcDataModifierHighlight = 0;

	if (hightlight_modifier) {
		// the modifier does exist
		okcDataModifierHighlight = dynamic_cast<OkcDataModifierHighlight*> (hightlight_modifier);
	}

	assert(okcDataModifierHighlight!=0);

	std::vector<double> binIdx;
	binIdx.clear();
	binIdx.resize(dim_num);
	for(int i=0; i<data_size; i++){
		getBinIdxForAllDim(binIdx, i);
		for(int dim=0; dim<dim_num; dim++){
			int binIdxAtDim = (int)binIdx[dim];
			frequency[dim][binIdxAtDim]++;
			if(okcDataModifierHighlight->getHighlight(i)==true){
				marked_freq[dim][binIdxAtDim]++;
			}
		}
	}

	for(int dim=0; dim<dim_num; dim++){
		for(int binNum =0; binNum<histo_size; binNum++){
			if(frequency[dim][binNum]>max_frequency[dim]){
				max_frequency[dim] = frequency[dim][binNum];
			}
		}
	}
}


void HistogramOperator::clearARRARR(std::vector< std::vector<double> > &array){
	int row = array.size();
	if(row==0){
		return;
	}
	std::vector<double> item = array[0];

	for(int i=0; i<row; i++){
		array[i].resize(0);
	}
	array.resize(0);
}

void HistogramOperator::clearARRARR(std::vector< std::vector<int> > &array){
	int row = array.size();
	if(row==0){
		return;
	}
	std::vector<int> item = array[0];

	for(int i=0; i<row; i++){
		array[i].resize(0);
	}
	array.resize(0);
}

void HistogramOperator::setARRZero(std::vector< std::vector<int> > &array){
	int row = array.size();
	std::vector<int> item = array[0];
	int col = item.size();

	for(int i=0; i<row; i++){
		for(int j=0; j<col; j++){
			array[i][j] = 0;
		}

	}
}

void HistogramOperator::setARRZero(std::vector<int> &array){
	for(int i=0; i<(int)array.size(); i++){
		array[i]=0;
	}
}

void HistogramOperator::setHistogramModifierArray(HistogramModifier* histogramModifier){

	OkcData* okcdata = (OkcData*) m_input;

	int dimSize = okcdata->getDimSize();

	for (int i = 0; i < dimSize; i++) {
		for(int j=0; j<histogram_size; j++){
			histogramModifier->frequency[i][j] = this->frequency[i][j];
		}
	}
	for (int i = 0; i < dimSize; i++) {
		for (int j = 0; j < histogram_size; j++) {
			histogramModifier->marked_freq[i][j] = this->marked_freq[i][j];
		}
	}
	for (int i = 0; i < dimSize; i++) {
		for (int j = 0; j < histogram_size + 1; j++) {
			histogramModifier->histo_cut_points[i][j] = this->histo_cut_points[i][j];
		}
	}
	for (int i = 0; i < dimSize; i++) {
		histogramModifier->max_frequency[i] = this->max_frequency[i];
	}
}



