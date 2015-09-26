#ifndef DIMSTACKCUSTOMIZATION_H
#define DIMSTACKCUSTOMIZATION_H

#include <QtGui/QDialog>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QWidget>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QFrame>
#include <QtGui/QVBoxLayout>
#include <QMdiSubWindow>

#include "main/XmdvToolMainWnd.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "view/multidim/VisDimstack.h"

class VisDimstack;

class DimstackCustomization : public QDialog
{
    Q_OBJECT

public:
    DimstackCustomization(XmdvToolMainWnd *mainwnd, QMdiSubWindow *window);
    ~DimstackCustomization();


private:
	//helper for adding one row for each dimension
	void initRows(VisDimstack *visDimstack);

private:
	int maxBins;

	XmdvToolMainWnd *mainWnd;

	ViewManager *viewManager;
	int windowNo, pID; //window and pipeline IDs

	QPushButton *applyButton;
	QLabel *maxBinsLabel, *nameTitle, *numBinsTitle;
	QLineEdit *maxBinsEdit;
	QScrollArea *binSelectionArea;
	QWidget *selectionAreaContents;
	QFormLayout *selectionTable;
	QGridLayout *windowLayout;



	std::vector<QLabel *> selectionCardinalityLabels;
	std::vector<QSlider *> selectionSliders;

private slots:
	void updateSliderRange();

public slots:
	//apply the current set cardinalities to the underlying VisDimstack view.
	void applyChanges();

};

#endif // DIMSTACKCUSTOMIZATION_H
