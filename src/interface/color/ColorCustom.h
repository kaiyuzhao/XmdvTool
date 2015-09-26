/*
 * ColorCustom.h
 *
 *  Created on: Jun 9, 2009
 *  Author: Jason Stasik
 *
 *  The Color Customization dialog allows users to switch the color
 *  of nearly everything in the application (unless the programmer
 *  defines that color uneditable). There is one instance of this class
 *  stored in HandleTools and it is simply hidden and made to reappear
 *  rather than deleting it and recreating it every time the user wants
 *  to change a color.
 */

#ifndef COLORCUSTOM_H_
#define COLORCUSTOM_H_

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QSignalMapper>
#include <QComboBox>
#include <QLabel>

#include <vector>

class ColorSelector;
class XmdvToolMainWnd;
class ColorMap;

class ColorCustom : public QDialog {
	Q_OBJECT

public:
	// Constructor
	ColorCustom(XmdvToolMainWnd *mainWnd);
	// Destructor
	~ColorCustom();

	// Draws the color maps in the corresponding QGLWidget
	void				drawColorMaps();
	// Gets the main window
	XmdvToolMainWnd*	getMainWnd();
	// Sets m_openSelector to the opposite of it's current value
	// \param index The index in m_openSelectors to swap
	void				swapOpenSelector(int index);

	// Sets m_openSelector to a specified value
	// \param index The index in m_openSelectors to swap
	// \param value The value to set
	void				setOpenSelector(int index, bool value);

private:
	XmdvToolMainWnd		*m_mainWnd; // the main window

	std::vector<QPushButton*>	pbutton_colorSelect; // array of buttons that open the color selector
	std::vector<QLabel*>		label_colorBars; // labels are used to show the drawn color bars

	std::vector<bool>			m_openSelectors; // which color selectors are already open

	QComboBox			*selectTheme; // allows the user to select a theme

	QSignalMapper		*m_map; // maps ColorManager::Colors to pbutton_colorSelect

private:
	// Opens the color dialog window rather than the color selector when the color is only
	// allowed to be a single color
	void				getSingleColor(ColorMap *c);

private slots:
	// Opens the color selector for the color at index
	// \param index The number that corresponds to the color in ColorManager::Colors
	void				openColorSelector(int index);

	// Switches every color to a predefined color specified by the theme
	// \param theme The theme that corresponds to ColorManager::Themes
	void				theme(int theme);
};

inline XmdvToolMainWnd* ColorCustom::getMainWnd() {
	return m_mainWnd;
}

#endif /* COLORCUSTOM_H_ */
