/*
 * DimstackCustomizationManager.h
 *
 *  Created on: May 20, 2009
 *      Author: Dan Spitz
 *
 *
 * Manager class for all DimstackCustomization dialogs- maintains up to one dialog per pipeline.
 */

#ifndef DIMSTACKCUSTOMIZATIONMANAGER_H_
#define DIMSTACKCUSTOMIZATIONMANAGER_H_

#include <map>
#include "main/XmdvToolMainWnd.h"
#include "interface/dimstack_customization/DimstackCustomization.h"

class XmdvToolMainWnd;
class DimstackCustomization;

class DimstackCustomizationManager {
public:
	DimstackCustomizationManager(XmdvToolMainWnd *);
	virtual ~DimstackCustomizationManager();

public:
	DimstackCustomization *getDialog();

private:
	XmdvToolMainWnd *mainWnd;
};

#endif /* DIMSTACKCUSTOMIZATIONMANAGER_H_ */
