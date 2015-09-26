#include <QApplication>
#include <QMessageBox>
#include <QGLFormat>
#include <QGLWidget>

#include "main/XmdvTool.h"
#include "main/Globals.h"
#include "main/XmdvToolMainWnd.h"

Globals g_globals;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	// Check whether the current system configuration supports the OpenGL
	if (!QGLFormat::hasOpenGL() ) {
		QMessageBox::information(0, "XmdvTool",
				"This system does not support OpenGL rendering.  XmdvTool cannot start.");
		return -1;
	}

	QGLWidget* tryGLWidget = new QGLWidget( (QWidget*)0 );
	if ( !tryGLWidget->isValid() ) {
		QMessageBox::information(0, "XmdvTool",
				"This system does not support OpenGL rendering.  XmdvTool cannot start.");
		delete tryGLWidget;
		return -1;
	}
	delete tryGLWidget;

	XmdvToolMainWnd w(NULL);
	w.show();
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	w.openInitialDatasets();
	return a.exec();
}
