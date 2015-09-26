/*
 * DimTreeOperator.cpp
 *
 *  Created on: Sep 8, 2009
 *      Ported by Zaixian Xie
 */

#include "operator/dimreduct/DimTreeOperator.h"

#include <typeinfo>
#include <queue>
#include <cstdlib>
#include <assert.h>
using namespace std;

#include "data/multidim/OkcData.h"
#include "data/dimreduct/InterRingDimClusterTree.h"
#include "data/dimreduct/InterRingDimCluster.h"
#include "datatype/LinkList.h"

DimTreeOperator::DimTreeOperator()  : Operator() {
	// Set a null string to the dc file name
	m_dcFileName[0] = 0;
}

DimTreeOperator::~DimTreeOperator() {
}

void DimTreeOperator::setDcFileName(char *dcFileName) {
	strcpy(m_dcFileName, dcFileName);
}

char* DimTreeOperator::getDcFileName() {
	return m_dcFileName;
}

void DimTreeOperator::setCurrentList(LinkList* current_list){
	m_current_list = current_list;
}

LinkList* DimTreeOperator::getCurrentList() {
	return m_current_list;
}

void DimTreeOperator::setInput(Data* input) {
	//Verify that we have a correct data type for input
	assert(typeid(*input)==typeid(OkcData));
	Operator::setInput(input);
}

void DimTreeOperator::preOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
}

bool DimTreeOperator::doOperator() {
	//Verify that we have a correct data type for input
	assert(typeid(*m_input)==typeid(OkcData));
	OkcData* input = (OkcData*)m_input;

	// Prepare the file name
	char dcfilename[FILENAME_MAXLEN];
	// Assume the file name extension is ".okc"
	int N = strlen(input->filepath);
	strcpy(dcfilename, input->filepath);
	strcpy(&dcfilename[N-4],".dc");
	setDcFileName(dcfilename);

	// Create the dimension cluster tree
	// via loading the dc file into the output of this operator
	InterRingDimClusterTree* dtree = new InterRingDimClusterTree();
	dtree->buildInterRingDimClusterTreeFromFile(input, dcfilename);
	// Initialize the current dimension list via
	// adding the original okc dimensions to the list
	initCurrentList(dtree);
	dtree->m_inputOkcData = input;

	SAFE_DELETE(m_output);
	m_output = dynamic_cast<Data*>(dtree);

	return true;
}

void DimTreeOperator::postOperator() {
}


void DimTreeOperator::initCurrentList(InterRingDimClusterTree* dtree) {
	// We get all leaf nodes (original dimensions in okc file) in the dimension tree
	// and put it to the current list.  In order to do this, we create a empty queue first,
	// and then put the root node into the queue.  A while loop will do the main work.
	// In this loop, we first retrieve the first node from the queue, if it is original
	// okc dimension (leaf node), we added it into the current list; otherwise,
	// put all of its children into the queue.  After this while loop, the current list
	// contains only the original dimension, and then we sort this current list by the okc dimension id.

	// The queue to help traverse all children
	queue<InterRingDimCluster*> nodeq;
	// The list to store the result before sorting
	LinkList listTemp;
	// push the root into the node
	nodeq.push( dynamic_cast<InterRingDimCluster*>(dtree->root) );

	// This while loop adds all original dimenions into the current list
	while (!nodeq.empty() ) {
		// Pop the first node in the queue
		InterRingDimCluster* head = nodeq.front();
		nodeq.pop();
		if (head->okc_id>=0) {
			// This node corresponds to an original dimension,
			// add it into the current list
			listTemp.AddNodeAtEnd(head);
		} else {
			// This node is not an original dimension,
			// get all its children and push them into the queue
			//
			// Get the first child
			InterRingCluster* child = head->children;
			while (child!=0) {
				// push this child into the queue
				nodeq.push( dynamic_cast<InterRingDimCluster*>(child) );
				// get the next sibling
				child = child->siblings;
			}
		}
	}

	// convert this list to an array
	void** leafNodes = listTemp.TransToArray();
	int n = listTemp.GetLength();

	// sort this array by the okc id
	qsort( leafNodes, n, sizeof( void* ), DimTreeOperator::compareDim );

	// Clear the current dimension list
	m_current_list->clear();
	// Add the sorted nodes (original dimension ) into the list
	int i;
	for (i=0; i<n; i++) {
		m_current_list->AddNodeAtEnd( leafNodes[i] );
	}

	delete []leafNodes;
}

int DimTreeOperator::compareDim( const void *elm1, const void *elm2)
{
	double value1 = ((InterRingDimCluster *)(*((void**)elm1)) )->okc_id;
	double value2 = ((InterRingDimCluster *)(*((void**)elm2)) )->okc_id;

	if ( value1 < value2 )
		return -1;
	else if ( value1 == value2 )
		return 0;
	else
		return 1;
}
