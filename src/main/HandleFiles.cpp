#include "XmdvTool.h"
#include "HandleFiles.h"
#include "pipeline/PipelineManager.h"
#include "view/ViewManager.h"

#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QGLWidget>
#include <QSize>

#include <string>
#include <new>
#include <cstdlib>
using namespace std;

HandleFiles::HandleFiles(XmdvToolMainWnd *mainWnd) {
	this->mainWnd=mainWnd;
}

HandleFiles::~HandleFiles() {
};

void HandleFiles::openFile() {
    QString fn = QFileDialog::getOpenFileName(this->mainWnd, tr("Open File..."),
    		QString(), tr("Okc Files (*.okc);;CSV Files (*.csv);;All Files (*)"));
    // if users clicked the button "Cancel", fn will be null
    if (fn.isNull()) {
    	return;
    }

    try {
    //pm is the current pipeline manager
    PipelineManager* pm = this->mainWnd->getPipelineManager();
#ifdef WIN32
    // Under win32, we open the parallel coordinates
    pm->openFile(fn, false, XmdvTool::VISTYPE_PA);
#else
    // Under unix, we cannot open the parallel coordinates
    // because the text cannot be displayed correctly
    pm->openFile(fn, false, XmdvTool::VISTYPE_GL);
#endif
    } catch (bad_alloc& ba) {
    	QMessageBox::critical(mainWnd,
    			"Failure to request memory",
    			"XmdvTool failed to request enough memory to visualize "
    			"the current dataset and has to exit.  Please report "
    			"this problem to xmdv@cs.wpi.edu with the description "
    			"of your dataset and system configuration.  Thank you! "
    			"--Xmdv Group");
    	exit(1);
    }
}

void HandleFiles::saveImage() {
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this->mainWnd,
                                tr("Save Image As..."),
                                QString(),
                                tr("BMP Files (*.bmp);;JPG Files (*.jpg);;PNG Files (*.png)"),
                                &selectedFilter,
                                options);
    if (!fileName.isEmpty()) {
    	ViewManager* vm = this->mainWnd->getViewManager();
    	vm->saveImage(fileName, selectedFilter);
    }

}
