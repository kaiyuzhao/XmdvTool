/*
 * ColorStrategyManager.h
 *
 *  Created on: Jul 9, 2009
 *  Author: Jason Stasik
 *
 *	Stores ColorStrategy dialogs, one for each PID
 */

#ifndef COLORSTRATEGYMANAGER_H_
#define COLORSTRATEGYMANAGER_H_

#include <map>
#include "interface/color/ColorStrategy.h"

class XmdvToolMainWnd;

class ColorStrategyManager {
public:
	ColorStrategyManager(XmdvToolMainWnd *main);
	virtual ~ColorStrategyManager();

	// Gets the dialog for the active window
	ColorStrategy* getDialog();
	// Gets the selected dimension for Color By Value
	// \param pid The pId of the pipeline to get the value for, -1 means the active pipeline
	int getValueDim(int pid = -1);

private:
	XmdvToolMainWnd *mainWnd;
	std::map<int, ColorStrategy *> dialogs;
};

#endif /* COLORSTRATEGYMANAGER_H_ */
