/*
 * DimReducer.h
 *
 *  Created on: Sep 24, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class DimReducer is used to online generate dimension cluster tree.
 */

#ifndef DIMREDUCER_H_
#define DIMREDUCER_H_

#include <stdio.h>
#include <vector>
#include "data/cluster/Cluster.h"

class OkcData;
class ClusterTree;
class LinkList;
class LinkListNode;

class DimReducer {
public:
	DimReducer();
	virtual ~DimReducer();

private:
	//data: store the data for building dimension cluster tree. data[i][j]: i: data item id; j: dimension
    double** data_array;
    int** entries_array;
    //data_num: number of data items in data_array
    int data_num;
    std::vector<double> dim_max, dim_min;
    double *data_min;
    double *data_max;

public:
	bool BuildDimClusterTree(OkcData* okcdata, const char* dcfilename);
	bool GetData(OkcData* okcdata, const char* dcfilename);
	bool GetDataFromClusters(OkcData* okcdata, const char* dcfilename);
	void CollectClustersOnContour(OkcData* okcdata, ClusterTree* btree, double contour_radius, const Cluster &node, LinkList* data_linkList, LinkList* entries_linkList);
	void RefineData(OkcData* okcdata);
	void SaveDataToFile(OkcData* okcdata, const char * fname);
	bool OnlineClusterDimension(OkcData* okcdata, const char* input_file_name, const char* output_file_name, int times, int threshold);
	LinkList *GetInputClusterData(FILE *fp, int &cluster_amount);
	int *GetInputClusterEntries(FILE *fp, int cluster_amount);
	void WriteDimClusteringOutput(OkcData* okcdata, FILE *fp, int cluster_amount, LinkList* data_link);
	void DimClustering(int n, LinkList *data_link1, LinkList *data_link2, int *cluster_entries, double min_distance, int threshold);
	LinkListNode* FindWhoIsMaximum( LinkList *data_link );

};

#endif /* DIMREDUCER_H_ */
