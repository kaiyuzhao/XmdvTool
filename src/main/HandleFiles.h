#ifndef HANDLEFILES_H_
#define HANDLEFILES_H_

#include "main/XmdvToolMainWnd.h"

#include <QtCore/QObject>

class XmdvToolMainWnd;

class HandleFiles : public QObject
{
    Q_OBJECT

public:
	HandleFiles(XmdvToolMainWnd *mainWnd);
	virtual ~HandleFiles();

private:
	XmdvToolMainWnd* mainWnd;

public slots:
	// Pop up a open file dialog and allow users to select an okc or csv file
	// containing a multivariate dataset.  After this dialog closes, the dataset
	// will be rendered in a new view.
	void openFile();
	// Pop up a "save as" file dialog and allow users to specify a new or existing
	// file.  XmdvTool will save the current image in the active view to a image file.
	void saveImage();
};

#endif /*HANDLEFILES_H_*/
