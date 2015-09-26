/*
 * ClusterOperator.cpp
 *
 *  Created on: Jun 4, 2009
 *      Author: Zaixian Xie
 */

#include "operator/sbb/ClusterOperator.h"

#include <cstring>
#include <typeinfo>
#include <assert.h>

#include <QFileInfo>
#include <QMessageBox>

#include "main/XmdvTool.h"
#include "data/Data.h"
#include "data/multidim/OkcData.h"
#include "data/cluster/ClusterTree.h"
#include "operator/sbb/HierDisplayInformation.h"

ClusterOperator::ClusterOperator() : Operator() {
	// set the cg file name to NULL
	m_cgFileName[0] = 0;
}

ClusterOperator::~ClusterOperator() {
}

void ClusterOperator::setCgFileName(char *cgFileName) {
	strcpy(m_cgFileName, cgFileName);
}

char* ClusterOperator::getCgFileName() {
	return m_cgFileName;
}


void ClusterOperator::setHierDisplayInformation(HierDisplayInformation* hierInfo) {
	m_hierInfo = hierInfo;
}

HierDisplayInformation* ClusterOperator::getHierDisplayInformation() {
	return m_hierInfo;
}

void ClusterOperator::setTreeContour(TreeContour* tree_contour) {
	m_tree_contour = tree_contour;
}

TreeContour* ClusterOperator::getTreeContour() {
	return m_tree_contour;
}

void ClusterOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(OkcData));
	Operator::setInput(input);
}

void ClusterOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
}

bool ClusterOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
	OkcData* input = (OkcData*)m_input;

	// Prepare the file name
	char cgfilename[FILENAME_MAXLEN];
	QString dataFilePath(input->filepath);
	QFileInfo fi(dataFilePath);
	// If the suffix of the file name is not "okc", we look for okc file
	// in the same path
	if ( fi.suffix().compare( "okc", Qt::CaseInsensitive ) != 0 ) {
		int n = dataFilePath.length();
		dataFilePath = dataFilePath.remove( n-3, 3).append("okc");
	}
	// If the file does not exist, do nothing and return
	if ( !QFileInfo(dataFilePath).exists() ) {
        QMessageBox::critical(0, QString("File does not exist!"),
                              QString("The file %1 does not exist. "
                            		  "XmdvTool cannot finish clustering operation!")
                              .arg(dataFilePath));
		return false;
	}

	// By default, the cg file contains the clustering result.
	// It has the same location and base name as the okc file
	// but with suffic "cg".
	strcpy(cgfilename, dataFilePath.toAscii() );
	int len = strlen(cgfilename);
	strcpy(&cgfilename[len-4],".cg");
	setCgFileName(cgfilename);

	// Create the cluster tree
	// via load the cg file into the cluster tree
	ClusterTree* btree;
	btree = ClusterTree::hierInitClusterTree(input->filepath, cgfilename, input,
			getHierDisplayInformation(), getTreeContour());
	btree->m_inputOkcData = input;

	SAFE_DELETE(m_output);
	m_output = dynamic_cast<Data*>(btree);

	return true;
}

void ClusterOperator::postOperator() {
}
