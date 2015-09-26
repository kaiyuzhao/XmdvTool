/*
 * HierHist.cpp
 *
 *  Created on: Jun 16, 2009
 *      Author: Zaixian Xie
 */

#include "data/cluster/HierHist.h"
#include "data/cluster/Cluster.h"
#include "data/cluster/ClusterTree.h"

std::vector<int> HierHist::histo;
int  HierHist::HISTO_NUMBIN =100;

HierHist::HierHist() {
}

HierHist::~HierHist() {
	histo.clear();
}

void HierHist::nodes_selected()
{


}

void HierHist::cfHistoFinalize()
{
	histo.clear();
}

void HierHist::cfHistoInit(ClusterTree* btree)
{
    histo.resize(HISTO_NUMBIN);

    int i;
    for (i=0; i<HISTO_NUMBIN; i++)
        histo[i]=0;

    double max_radius = btree->nodes[0].sqrt_radius;

	for(int i=0;i<(int)btree->nodes.size();i++)
	{
		int pos = (int)((btree->nodes[i].sqrt_radius/max_radius)*HISTO_NUMBIN);
		if (pos>=HISTO_NUMBIN) pos=HISTO_NUMBIN-1;
		histo[pos]++;
	}
}
