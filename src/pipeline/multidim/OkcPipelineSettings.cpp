/*
 * OkcPipelineSetttings.cpp
 *
 *  Created on: May 21, 2009
 *      Author: Zaixian xie
 */

#include "pipeline/multidim/OkcPipelineSettings.h"

OkcPipelineSettings::OkcPipelineSettings() {
	initSettings();
}

OkcPipelineSettings::~OkcPipelineSettings() {
}

void OkcPipelineSettings::initSettings() {
	// The default mode for glyph placement is ordering by the first dimension
	m_glyphPlaceMode = XmdvTool::GLYPHPLACE_ORDERED;
}
