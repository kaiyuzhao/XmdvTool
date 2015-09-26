/*
 * OkcDataModifierClusterNodeMin.cpp
 *
 *  Created on: Jul 16, 2009
 *      Author: xiezx
 */

#include "data/cluster/OkcDataModifierClusterNodeMin.h"

#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"

OkcDataModifierClusterNodeMin::OkcDataModifierClusterNodeMin() {
}

OkcDataModifierClusterNodeMin::OkcDataModifierClusterNodeMin(OkcData* okcdata) :
	OkcDataModifier(okcdata) {
}

OkcDataModifierClusterNodeMin::~OkcDataModifierClusterNodeMin() {
}

XmdvTool::MODIFIERTYPE OkcDataModifierClusterNodeMin::getModifierType() {
	return XmdvTool::MODIFIER_CLUSTERNODEMIN;
}

void OkcDataModifierClusterNodeMin::initClusterMinArr() {
	m_clusterMin.resize(m_dims*m_data_size);
}


void OkcDataModifierClusterNodeMin::setClusterMin(double max, int row, int col) {
	m_clusterMin[row*m_dims+col] = max;
}

void OkcDataModifierClusterNodeMin::setClusterMin(std::vector<double>& max, int row) {
	int i;
	for (i=0; i<m_dims; i++) {
		m_clusterMin[row*m_dims+i] = max[i];
	}
}

void OkcDataModifierClusterNodeMin::insertTail(std::vector<double>& max) {
	int i;
	for (i=0; i<m_dims; i++) {
		m_clusterMin.push_back(max[i]);
	}
}

double OkcDataModifierClusterNodeMin::getClusterMin(int row, int col) {
	return m_clusterMin[row*m_dims+col] ;
}

void  OkcDataModifierClusterNodeMin::getClusterMin(std::vector<double> &max, int row) {
	max.resize(m_dims);
	int i;
	for (i=0; i<m_dims; i++) {
		max[i] = m_clusterMin[row*m_dims+i];
	}
}

void OkcDataModifierClusterNodeMin::copyFrom(const OkcDataModifierClusterNodeMin* copy) {
	OkcDataModifier::copyFrom((OkcDataModifier*)copy);
	m_clusterMin = copy->m_clusterMin;

}

void OkcDataModifierClusterNodeMin::operator=(const OkcDataModifierClusterNodeMin &copy) {
	copyFrom(&copy);
}
