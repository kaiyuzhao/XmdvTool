#ifndef DIMSTACKCUSTOMIZATION_H
#define DIMSTACKCUSTOMIZATION_H

#include <QtWidgets/QDialog>

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>
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
