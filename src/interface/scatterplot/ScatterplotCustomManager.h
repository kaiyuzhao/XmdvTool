/*
 * ScatterplotCustomManager.h
 *
 *  Created on: Jul 27, 2009
 *      Author: zyguo
 */

#ifndef SCATTERPLOTCUSTOMMANAGER_H_
#define SCATTERPLOTCUSTOMMANAGER_H_

#include <map>
#include "main/XmdvToolMainWnd.h"
#include "interface/scatterplot/ScatterplotCustom.h"


class XmdvToolMainWnd;
class ScatterplotCustom;

class ScatterplotCustomManager {
public:
	ScatterplotCustomManager(XmdvToolMainWnd *mainWnd);
	virtual ~ScatterplotCustomManager();

public:
	ScatterplotCustom *getDialog();

private:
	XmdvToolMainWnd *mainWnd;
	map<int, ScatterplotCustom *> dialogs; //PipelineID -> ScatterplotCustom dialog

};

#endif /* SCATTERPLOTCUSTOMMANAGER_H_ */
