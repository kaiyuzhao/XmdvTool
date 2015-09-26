/*
 * OkcPipelineSetttings.h
 *
 *  Created on: May 21, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class OkcPipelineSettings manages all of settings
 * for a multivariate pipeline
 *
 */

#ifndef OKCPIPELINESETTTINGS_H_
#define OKCPIPELINESETTTINGS_H_

#include "pipeline/PipelineSettings.h"
#include "main/XmdvTool.h"

class OkcPipelineSettings: public PipelineSettings {
public:
	OkcPipelineSettings();
	virtual ~OkcPipelineSettings();

public:
	XmdvTool::GLYPHPLACE_MODE m_glyphPlaceMode;

public:
	void initSettings();


};

#endif /* OKCPIPELINESETTTINGS_H_ */
