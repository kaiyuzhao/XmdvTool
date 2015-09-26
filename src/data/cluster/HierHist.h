/*
 * HierHist.h
 *
 *  Created on: Jun 16, 2009
 *      Author: Zaixian Xie
 */

#ifndef HIERHIST_H_
#define HIERHIST_H_

#include <vector>

class ClusterTree;

class HierHist
{
public:
	HierHist();
	virtual ~HierHist();

	// The number of bins in the histogram for the hierarchical cluster tree
	static int HISTO_NUMBIN;
	static std::vector<int> histo;
	static void cfHistoInit(ClusterTree* btree);
	static void cfHistoFinalize();
	static void nodes_selected();
};

#endif /* HIERHIST_H_ */
