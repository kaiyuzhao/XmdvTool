/*
 * OkcDataModifierClusterColor.cpp
 *
 *  Created on: Jul 14, 2009
 *      Author: xiezx
 */

#include "data/cluster/OkcDataModifierClusterColor.h"

#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"

OkcDataModifierClusterColor::OkcDataModifierClusterColor() {
}

OkcDataModifierClusterColor::OkcDataModifierClusterColor(OkcData* okcdata) :
	OkcDataModifier(okcdata) {

}

OkcDataModifierClusterColor::~OkcDataModifierClusterColor() {

}

XmdvTool::MODIFIERTYPE OkcDataModifierClusterColor::getModifierType() {
	return XmdvTool::MODIFIER_CLUSTERCOLOR;
}


void OkcDataModifierClusterColor::initColorArr() {
	m_color.resize(m_data_size);
	int i;
	for (i=0; i<m_data_size; i++) {
		m_color[i] = false;
	}
}

void OkcDataModifierClusterColor::setColor(int i, double color) {
	m_color[i] = color;
}

double OkcDataModifierClusterColor::getColor(int i) {
	return m_color[i];
}

std::vector<double> & OkcDataModifierClusterColor::getColorArr() {
	return m_color;
}

void OkcDataModifierClusterColor::insertTail(double color) {
	m_color.push_back(color);
}

void OkcDataModifierClusterColor::copyFrom(const OkcDataModifierClusterColor* copy) {
	OkcDataModifier::copyFrom((OkcDataModifier*)copy);
	m_color = copy->m_color;
}

void OkcDataModifierClusterColor::operator=(const OkcDataModifierClusterColor &copy) {
	copyFrom(&copy);
}
