/*
 * InterRingDimClusterTree.h
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#ifndef INTERRINGDIMCLUSTERTREE_H_
#define INTERRINGDIMCLUSTERTREE_H_

#include "data/dimreduct/InterRingClusterTree.h"

class OkcData;
class LinkList;

class InterRingDimClusterTree : public InterRingClusterTree {
public:
	InterRingDimClusterTree();
	virtual ~InterRingDimClusterTree();

public:
	// The pointer to the OkcData to generate this dimension cluster tree
	OkcData* m_inputOkcData;

public:
	//build the dim cluster tree according to a file;
	//fname: name of the file;
    bool buildInterRingDimClusterTreeFromFile(OkcData* okcdata, const char* dcfilename);

    // Generate an OkcData containing only those dimensions in the current
    // dimension list represented by current_list
    // parameters:
    //   okcdata: the generation result will be output to this data structure
    //   current_list: the current selected dimension list
	void generateDimReductResult(OkcData* okcdata, LinkList* current_list );

    void resetNames();

    //void PCA();

    void resetSelection();
};

#endif /* INTERRINGDIMCLUSTERTREE_H_ */
