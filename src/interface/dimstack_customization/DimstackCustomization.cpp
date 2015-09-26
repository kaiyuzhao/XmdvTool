#include "interface/dimstack_customization/DimstackCustomization.h"

#include <typeinfo>
#include <assert.h>

#include <QMessageBox>

#include "view/ViewDisplay.h"
#include "view/ViewManager.h"
#include "view/ViewWindow.h"
#include "pipeline/PipelineManager.h"

#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "view/GLDisplay.h"


DimstackCustomization::DimstackCustomization(XmdvToolMainWnd *mainwnd, QMdiSubWindow *window)
    : QDialog(window)
{
	//save references to main window, viewManager and pID
	mainWnd = mainwnd;
	viewManager = mainWnd->getViewManager();
	pID = viewManager->getActiveViewWindow()->getPipelineID();


	//get the VisDimstack
	ViewDisplay *vd = viewManager->getActiveViewWindow()->getViewDisplay();;
	assert(typeid(*vd) == typeid(VisDimstack));
	VisDimstack *visDimstack = dynamic_cast<VisDimstack *>(vd);

	//use it to set the lengths of the arrays
	selectionCardinalityLabels.resize(visDimstack->m_dim_size);
	selectionSliders.resize(visDimstack->m_dim_size);

	//set default value of 10 for maximum bins in a dimension
	maxBins = 10;

	//set up the dialog
	if (objectName().isEmpty())
	            setObjectName(QString::fromUtf8("dimstackCustomizationDialog"));
	resize(300,360);
	//setFixedSize(QSize::QSize (300, 360));
	setWindowTitle(QString::fromUtf8("Dimension Stacking Customization"));
	setAttribute(Qt::WA_DeleteOnClose, false);

	windowLayout = new QGridLayout(this);

	maxBinsLabel = new QLabel(this);
	maxBinsLabel->setObjectName(QString::fromUtf8("maxBinsLabel"));
	maxBinsLabel->setText(QString::fromUtf8("Maximum Number of Bins"));
	//maxBinsLabel->setGeometry(QRect(10, 10, 121, 16));
	windowLayout->addWidget(maxBinsLabel, 0, 0);

	binSelectionArea = new QScrollArea(this);
	binSelectionArea->setObjectName(QString::fromUtf8("binSelectionArea"));
	//binSelectionArea->setGeometry(QRect(9, 39, 290, 280));
	windowLayout->addWidget(binSelectionArea, 1, 0, 1, -1);

	selectionAreaContents = new QWidget(binSelectionArea);

	selectionTable = new QFormLayout();
	selectionTable->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	//add the top row to the selection table
	nameTitle = new QLabel(selectionAreaContents);
	nameTitle->setObjectName(QString::fromUtf8("nameTitle"));
	nameTitle->setText(QString::fromUtf8("Fieldname"));
	nameTitle->setFrameStyle(QFrame::Panel | QFrame::Raised);

	numBinsTitle = new QLabel(selectionAreaContents);
	numBinsTitle->setObjectName(QString::fromUtf8("numBinsTitle"));
	numBinsTitle->setText(QString::fromUtf8("Number of Bins"));
	numBinsTitle->setFrameStyle(QFrame::Panel | QFrame::Raised);

	//selectionTable->setVerticalSpacing(0);
	selectionTable->addRow(nameTitle, numBinsTitle);

	//dynamically add the rest of the rows here
	//(and while we're at it we set maxBins to the highest cardinality
	// of a dimension if it exceeds the default of 10)
	initRows(visDimstack);

	//set the layout after it has been populated
	//and set the contents of the scroll area
	selectionAreaContents->setLayout(selectionTable);
	binSelectionArea->setWidget(selectionAreaContents);

	//now maxBins is equal to the maximum number of bins of all dimensions in the dataset
	//generate the field for specifying max number of bins
	maxBinsEdit = new QLineEdit(this);
	maxBinsEdit->setObjectName(QString::fromUtf8("maxBinsEdit"));
	maxBinsEdit->setText(QString::number(maxBins));
	//maxBinsEdit->setGeometry(QRect(140, 10, 113, 20));
	windowLayout->addWidget(maxBinsEdit, 0, 1);
	//connect the max number of bins specified to the max range of the slider
	connect(maxBinsEdit, SIGNAL(returnPressed()), this, SLOT(updateSliderRange()));

	applyButton = new QPushButton(this);
	applyButton->setObjectName(QString::fromUtf8("applyButton"));
	applyButton->setText(QString::fromUtf8("Apply"));
	applyButton->setDefault(false);
	//applyButton->setGeometry(QRect(10, 330, 161, 24));
	windowLayout->addWidget(applyButton, 2, 0);
	connect(applyButton, SIGNAL(clicked()), this, SLOT(applyChanges()));

	windowLayout->setVerticalSpacing(4);
	setLayout(windowLayout);

}

void DimstackCustomization::initRows(VisDimstack *visDimstack)
{

	//for each dimension, add a row to the selection table with the
	//slider properly set to the current cardinality
	for (int i = 0; i < visDimstack->m_dim_size; i++){
		//the widget in the left column
		//contains the name of the dimension
		QLabel *title = new QLabel(selectionAreaContents);
		title->setText(QString::fromUtf8(visDimstack->m_dim_names[i]));

		//container of a pair of widgets- the label that shows the
		//selected cardinality and the slider that selects it
		QWidget *container = new QWidget(selectionAreaContents);
		container->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

		QHBoxLayout *containerLayout = new QHBoxLayout(); //layout for the container

		QLabel *numBinsLabel = selectionCardinalityLabels[i] = new QLabel(container);
		numBinsLabel->setText(QString::number(visDimstack->m_dim_cardinality[i]));

		//add the label to the container
		containerLayout->addWidget(numBinsLabel);

		//now create the slider
		QSlider *slider = selectionSliders[i] = new QSlider(container);
		slider->setOrientation(Qt::Horizontal);
		slider->setTickInterval(1);
		slider->setMinimum(1);
		//update the maximum cardinality if this dimension's cardinality exceeds the current max
		if (visDimstack->m_dim_cardinality[i] > maxBins)
			maxBins = visDimstack->m_dim_cardinality[i];
		slider->setValue(visDimstack->m_dim_cardinality[i]); //set it's value to the proper cardinality

		//add the slider to the container
		containerLayout->addWidget(slider);

		container->setLayout(containerLayout);

		selectionTable->addRow(title, container); //add the widgets to the table

		//set up the connection between the slider and the displayed number
		//connect(slider, SIGNAL(stateChanged(int, int)), numBinsLabel, SLOT(setNum(int)));
		connect(slider, SIGNAL(valueChanged(int)), numBinsLabel, SLOT(setNum(int)));
	}

	//set the max value for all the sliders
	for (int i = 0; i < visDimstack->m_dim_size; i++){
		selectionSliders[i]->setMaximum(maxBins);
	}

}

DimstackCustomization::~DimstackCustomization()
{

}

void DimstackCustomization::updateSliderRange() {
	bool isInt;
	int newmax = maxBinsEdit->text().toInt(&isInt);

	//if the number entered is valid, save the new number of max bins
	//and update the max range of the sliders.
	if (isInt && newmax >= 1) {
		maxBins = newmax;
		for (int i = 0; i < (int)selectionSliders.size(); i++){
			selectionSliders[i]->setMaximum(maxBins);
		}
	}
	else
		//invalid text entered, set it back to it's previous value
		maxBinsEdit->setText(QString::number(maxBins));

}

void DimstackCustomization::applyChanges(){

	//get the currently active ViewWindow. We are displaying this window's dataset.
	ViewDisplay *vd = viewManager->getActiveViewWindow()->getViewDisplay();;

	//we must be looking at a dimstack visualization for this dialog to be relevant
	if(typeid(*vd) != typeid(VisDimstack))
		return; //not dimstacking, get out
	//the current visualization is DimStacking, we can change the parameters.
	VisDimstack *visDimstack = dynamic_cast<VisDimstack *>(vd);

	//save the old cardinalities in case we need to roll back the changes.
	std::vector<int> old_cardinality = visDimstack->m_dim_cardinality;
	//set the new cardinalities
	for (int i = 0; i < visDimstack->m_dim_size; i++){
		visDimstack->m_dim_cardinality[i] = selectionSliders[i]->value();
	}
	visDimstack->refreshCardinality(visDimstack->m_dim_cardinality);

	if (!visDimstack->checkRenderable()){
		//reset to the old settings
		visDimstack->m_dim_cardinality = old_cardinality;
		visDimstack->refreshCardinality(visDimstack->m_dim_cardinality);

		QMessageBox msgBox;
		msgBox.setText("These changes cannot be made because they increase the total cardinality of "
				       "the dataset such that it can no longer be properly visualized.");
		msgBox.exec();
		return;
	}

	//refresh the display
	viewManager->refreshDisplayByPID(pID);
}
