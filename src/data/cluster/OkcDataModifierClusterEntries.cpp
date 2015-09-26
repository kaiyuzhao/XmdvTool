/*
 * OkcDataModifierClusterEntries.cpp
 *
 *  Created on: Jul 23, 2009
 *      Author: Zaixian Xie
 */

#include "data/cluster/OkcDataModifierClusterEntries.h"

OkcDataModifierClusterEntries::OkcDataModifierClusterEntries() {
}

OkcDataModifierClusterEntries::OkcDataModifierClusterEntries(OkcData* okcdata) :
	OkcDataModifier(okcdata) {

}

OkcDataModifierClusterEntries::~OkcDataModifierClusterEntries() {

}

XmdvTool::MODIFIERTYPE OkcDataModifierClusterEntries::getModifierType() {
	return XmdvTool::MODIFIER_CLUSTERENTRIES;
}


void OkcDataModifierClusterEntries::initEntriesArr() {
	m_entries.resize(m_data_size);
	int i;
	for (i=0; i<m_data_size; i++) {
		m_entries[i] = 0;
	}
}

void OkcDataModifierClusterEntries::setEntries(int i, int entries) {
	m_entries[i] = entries;
}

int OkcDataModifierClusterEntries::getEntries(int i) {
	return m_entries[i];
}

std::vector<int> & OkcDataModifierClusterEntries::getEntriesArr() {
	return m_entries;
}


void OkcDataModifierClusterEntries::insertTail(int entries) {
	m_entries.push_back(entries);
}

int OkcDataModifierClusterEntries::getTotalEntries() {
	int i, s = 0;
	for (i=0; i<(int)m_entries.size(); i++) {
		s += m_entries[i];
	}
	return s;
}

void OkcDataModifierClusterEntries::copyFrom(const OkcDataModifierClusterEntries* copy) {
	OkcDataModifier::copyFrom((OkcDataModifier*)copy);
	m_entries = copy->m_entries;
}

void OkcDataModifierClusterEntries::operator=(const OkcDataModifierClusterEntries &copy) {
	copyFrom(&copy);
}
