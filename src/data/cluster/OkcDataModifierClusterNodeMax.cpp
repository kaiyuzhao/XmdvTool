/*
 * OkcDataModifierClusterNodeMax.cpp
 *
 *  Created on: Jul 12, 2009
 *      Author: Zaixian Xie
 */

#include "data/cluster/OkcDataModifierClusterNodeMax.h"

#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"

OkcDataModifierClusterNodeMax::OkcDataModifierClusterNodeMax() {
}

OkcDataModifierClusterNodeMax::OkcDataModifierClusterNodeMax(OkcData* okcdata) :
	OkcDataModifier(okcdata) {
}

OkcDataModifierClusterNodeMax::~OkcDataModifierClusterNodeMax() {
}

XmdvTool::MODIFIERTYPE OkcDataModifierClusterNodeMax::getModifierType() {
	return XmdvTool::MODIFIER_CLUSTERNODEMAX;
}

void OkcDataModifierClusterNodeMax::initClusterMaxArr() {
	m_clusterMax.resize(m_dims*m_data_size);
}


void OkcDataModifierClusterNodeMax::setClusterMax(double max, int row, int col) {
	m_clusterMax[row*m_dims+col] = max;
}

void OkcDataModifierClusterNodeMax::setClusterMax(std::vector<double>& max, int row) {
	int i;
	for (i=0; i<m_dims; i++) {
		m_clusterMax[row*m_dims+i] = max[i];
	}
}

void OkcDataModifierClusterNodeMax::insertTail(std::vector<double>& max) {
	int i;
	for (i=0; i<m_dims; i++) {
		m_clusterMax.push_back(max[i]);
	}
}

double OkcDataModifierClusterNodeMax::getClusterMax(int row, int col) {
	return m_clusterMax[row*m_dims+col] ;
}

void  OkcDataModifierClusterNodeMax::getClusterMax(std::vector<double> &max, int row) {
	max.resize(m_dims);
	int i;
	for (i=0; i<m_dims; i++) {
		max[i] = m_clusterMax[row*m_dims+i];
	}
}

void OkcDataModifierClusterNodeMax::copyFrom(const OkcDataModifierClusterNodeMax* copy) {
	OkcDataModifier::copyFrom((OkcDataModifier*)copy);
	m_clusterMax = copy->m_clusterMax;

}

void OkcDataModifierClusterNodeMax::operator=(const OkcDataModifierClusterNodeMax &copy) {
	copyFrom(&copy);
}
