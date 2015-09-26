/*
 * HandleZoom.h
 *
 *  Created on: Oct 7, 2009
 *      Author: zyguo
 */

#ifndef HANDLEZOOM_H_
#define HANDLEZOOM_H_

#include "main/XmdvToolMainWnd.h"

#include <QtCore/QObject>

class XmdvToolMainWnd;

class HandleZoom : public QObject{
	Q_OBJECT
public:
	HandleZoom(XmdvToolMainWnd *mainWnd);
	virtual ~HandleZoom();
	void updateZoomBarInfoFromGLDisplay();
private:
	XmdvToolMainWnd* mainWnd;

	GLDisplay* getGLDisplay();

	void changeLineEditValue();

public slots:
	void setZoomDirection(int _zoomDirection);
	void zoomIn();
	void zoomOut();
	void zoomReset();
	void setZoomFactors();
};

#endif /* HANDLEZOOM_H_ */
