/*
 *
 * HandleTools.h
 *
 *  Created on: May 14, 2009
 *      Author: Dan Spitz
 */

/*
 * Class HandleToos provides slot functions triggered by the menu "Tools"
 */
#ifndef HANDLETOOLS_H_
#define HANDLETOOLS_H_

#include "main/XmdvToolMainWnd.h"
#include "interface/dimstack_customization/DimstackCustomizationManager.h"

#include <QtCore/QObject>

class XmdvToolMainWnd;
class DimstackCustomization;
class PixelCustomizationManager;
class ColorCustom;
class ColorStrategyManager;
class ScatterplotCustomManager;

class HandleTools : public QObject
{
	Q_OBJECT

public:
	HandleTools(XmdvToolMainWnd* mainWnd);
	virtual ~HandleTools();

public:
	DimstackCustomizationManager *getDimstackCustomizationManager();
	PixelCustomizationManager *getPixelCustomizationManager();
	ColorStrategyManager *getColorStrategyManager();
	ScatterplotCustomManager *getScatterplotCustomManager();

private:
	XmdvToolMainWnd* m_mainWnd;
	DimstackCustomizationManager *dimManager;
	PixelCustomizationManager	*pixelManager;
	ColorCustom					*colorCustom;
	ColorStrategyManager		*stratManager;
	ScatterplotCustomManager    *scatterplotManager;

private:
	// Turn on or turn off dimension reduction mode
	// This function will by called by slot functions
	// turnOnDimReductMode(0 and turnOffDimReductMode()
	void switchDimReductMode(bool checked);

private:
	signals:
	void dimReductModeEnabled();

public slots:
	// For menu item "Manual Dimension On/Off/Reorder"
	void showDimManualDlg();
	// For menu item "Automated Dimension Reorder"
	void showDimAutoDlg();
	// For menu item "Dimension Stacking Customization"
	void openDimstackCustomization();
	// For menu item "Pixel-Oriented Customization"
	void openPixelCustomization();
	// For menu item "Glyph Customization"
	void showGlyphCustom();
	// For menu item "Color Customization"
	void showColorCustom();
	// For menu item "Color Strategy"
	void showColorStrategy();
	// For menu item "Scatterplot Customization"
	void ShowScatterplotCustom();

	// Turn on dimension reduction mode
	void turnOnDimReductMode();
	// Turn off dimension reduction mode
	void turnOffDimReductMode();
	// For menu item "Dimension Reduction Dialog"
	void openDimReductDlg();
};

#endif /* HANDLETOOLS_H_ */
