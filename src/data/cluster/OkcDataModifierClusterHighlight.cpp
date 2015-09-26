/*
 * OkcDataModifierClusterHighlight.cpp
 *
 *  Created on: Jul 14, 2009
 *      Author: xiezx
 */

#include "OkcDataModifierClusterHighlight.h"

OkcDataModifierClusterHighlight::OkcDataModifierClusterHighlight() {

}

OkcDataModifierClusterHighlight::OkcDataModifierClusterHighlight(OkcData* okcdata) :
	OkcDataModifier(okcdata) {

}

OkcDataModifierClusterHighlight::~OkcDataModifierClusterHighlight() {
}

XmdvTool::MODIFIERTYPE OkcDataModifierClusterHighlight::getModifierType() {
	return XmdvTool::MODIFIER_CLUSTERHIGHLIGHT;
}


void OkcDataModifierClusterHighlight::initHighlightArr() {
	m_highlighted.resize(m_data_size);
	int i;
	//printf("m_data_size=%d\n", m_data_size);
	for (i=0; i<m_data_size; i++) {
		bool value = false;
		m_highlighted[i] = value;
	}
}

void OkcDataModifierClusterHighlight::setHighlight(int i, bool isHighlighted) {
	m_highlighted[i] = isHighlighted;
}

bool OkcDataModifierClusterHighlight::getHighlight(int i) {
	return m_highlighted[i];
}

std::vector<bool> & OkcDataModifierClusterHighlight::getHighlightArr() {
	return m_highlighted;
}

void OkcDataModifierClusterHighlight::insertTail(bool isHighlighted) {
	m_highlighted.push_back(isHighlighted);
}

void OkcDataModifierClusterHighlight::copyFrom(const OkcDataModifierClusterHighlight* copy) {
	OkcDataModifier::copyFrom((OkcDataModifier*)copy);
	m_highlighted = copy->m_highlighted;
}

void OkcDataModifierClusterHighlight::operator=(const OkcDataModifierClusterHighlight &copy) {
	copyFrom(&copy);
}
