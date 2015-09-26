/*
 * DimReducer.cpp
 *
 *  Created on: Sep 24, 2009
 *      Ported by Zaixian Xie
 */

#include "util/algo/dimreduct/DimReducer.h"

#include <string>
#include <math.h>
#include <QDir>
using namespace std;

#include "data/multidim/OkcData.h"
#include "data/cluster/ClusterTree.h"
#include "data/cluster/Cluster.h"
#include "data/dimreduct/DimClusterNode.h"
#include "data/dimreduct/DataPair.h"
#include "datatype/LinkList.h"
#include "datatype/LinkListNode.h"

#define NODEELE(_node) ((DimClusterNode *)_node->element)
#define PAIRELE(_node) ((DataPair *)_node->element)

DimReducer::DimReducer() {
    data_array = NULL;
    entries_array = NULL;
}

DimReducer::~DimReducer() {
    if ( data_array != NULL )
    {
        for ( int i = 0; i < data_num; i++ )
        {
            delete [] data_array[i];
        }
        delete [] data_array;
    }

    if ( entries_array != NULL )
    {
        for ( int i = 0; i < data_num; i++ )
        {
            delete [] entries_array[i];
        }
        delete [] entries_array ;
    }
}

bool DimReducer::BuildDimClusterTree(OkcData* okcdata, const char* dcfilename)
{
	if ( !GetData(okcdata, dcfilename) )
		return false;
	RefineData(okcdata);
	// tempPath is system temporary file path
	QDir tempPath(QDir::tempPath ());
	// This file is used to online generating dimension cluster tree
	char* tempFileName = "xmdvogdc.tmp";
	// tempFilePath is the absolute path of temporary file.
	QString tempFilePath(tempPath.absoluteFilePath(tempFileName) );
	// filePath.toStdString().c_str() is the c string of temporary file absolute path
	SaveDataToFile ( okcdata, tempFilePath.toStdString().c_str() );
	bool succ = OnlineClusterDimension(okcdata, tempFilePath.toStdString().c_str(), dcfilename, 30, int(okcdata->getDataSize()*0.7 ));
	tempPath.remove(tempFileName);

	return succ;
}


bool DimReducer::GetData(OkcData* okcdata, const char* dcfilename)
{
	int i;
	if ( data_array != NULL )
	{
		for ( i = 0; i < data_num; i++ )
		{
			delete [] data_array[i];
		}
		delete [] data_array;
	}
	if ( entries_array != NULL )
	{
		for ( i = 0; i < data_num; i++ )
		{
			delete [] entries_array[i];
		}
		delete [] entries_array ;
	}

	return GetDataFromClusters(okcdata, dcfilename);
}

bool DimReducer::GetDataFromClusters(OkcData* okcdata, const char*)
{
	double contour_radius = 0.02;
	LinkList* data_linkList = new LinkList();
	LinkList* entries_linkList = new LinkList();

	// Prepare the file name for clustering results
	char cgfilename[FILENAME_MAXLEN];
	// Assume the file name extension is ".okc"
	int N = strlen(okcdata->filepath);
	strcpy(cgfilename, okcdata->filepath);
	strcpy(&cgfilename[N-4],".cg");

	// First generate a cluster tree
	ClusterTree* btree = new ClusterTree();
	btree->ReadClusterTreeFile(okcdata->filepath, cgfilename, okcdata);
	if (!btree) {
		// If we fail to create the cluster tree, return false
		return false;
	}

	CollectClustersOnContour(okcdata, btree, contour_radius, btree->nodes[0], data_linkList, entries_linkList);

	data_array = (double**) data_linkList->TransToArray();
	entries_array = (int**) entries_linkList->TransToArray();
	data_num = data_linkList->GetLength();
	delete data_linkList;
	delete entries_linkList;
	delete btree;
	return true;
}

void DimReducer::CollectClustersOnContour(OkcData* okcdata, ClusterTree* btree, double contour_radius, const Cluster &node, LinkList* data_linkList, LinkList* entries_linkList)
{
    int i;
    if ( node.num_children == 0 || node.sqrt_radius < contour_radius )
    {
    	int dim_size = okcdata->getDimSize();
	    double* data = new double [dim_size];
        int *entries = new int[1];

        for ( i=0; i < dim_size; i++)  {
			data[i] = node.mean[i];
        }

        entries[0] = node.entries;
        data_linkList->AddNodeAtEnd(data);
        entries_linkList->AddNodeAtEnd(entries);
        return;
    }
    else
    {
        for ( i = 0; i < node.num_children; i++ )
        {
            CollectClustersOnContour(okcdata, btree, contour_radius, btree->nodes[node.child[i]], data_linkList, entries_linkList);
        }
        return;
    }
}

void DimReducer::RefineData(OkcData* okcdata)
{
    if ( data_num <= 0 )
        return;

    int dim_size = okcdata->getDimSize();
    double *min, *max, *diff;

    min = new double [dim_size];
    max = new double [dim_size];
    diff = new double [dim_size];

    int i, j;
    for ( j = 0; j < dim_size; j++ )
    {
        min[j] = max[j] = data_array[0][j];
        for ( i = 1; i < data_num; i++ )
        {
            if ( min[j] > data_array[i][j] )
                min[j] = data_array[i][j];
            else if ( max[j] < data_array[i][j] )
                max[j] = data_array[i][j];
        }

        if ( fabs(max[j]-min[j]) < 1e-5 )
            min[j] = max[j] - 0.05;
        diff[j] = 1/(max[j]-min[j]);

        for ( i = 0; i < data_num; i++ )
        {
            data_array[i][j] = (data_array[i][j]-min[j])*diff[j];
        }
    }

    data_min = new double [data_num];
    data_max = new double [data_num];

    for ( i = 0; i < data_num; i++ )
    {
        data_min[i] = data_max[i] = data_array[i][0];
        for ( j = 1; j < dim_size; j++ )
        {
            if ( data_min[i] > data_array[i][j] )
                data_min[i] = data_array[i][j];
            else if ( data_max[i] < data_array[i][j] )
                data_max[i] = data_array[i][j];
        }
    }

}


void DimReducer::SaveDataToFile(OkcData* okcdata, const char * fname)
{
    int i, j;

    int dim_size = okcdata->getDimSize();
    FILE *fptr = fopen(fname,"w");
    fprintf(fptr, "%d %d\n", data_num, dim_size);

    for ( i = 0; i < data_num; i++ )
    {
        fprintf(fptr, "%lf %lf\n", data_min[i], data_max[i]);
    }

    delete [] data_min;
    delete [] data_max;

    for ( j = 0; j < dim_size; j++ )
    {
        fprintf(fptr, "%d ", j);
        for ( i = 0; i < data_num; i++ )
        {
            fprintf(fptr, "%lf ", data_array[i][j]);
        }
        fprintf(fptr, "\n");
    }

    for ( i = 0; i < data_num; i++ )
    {
        fprintf(fptr, "%d ", entries_array[i][0]);
    }
    fprintf(fptr, "\n");

    fclose(fptr);
}

bool DimReducer::OnlineClusterDimension(OkcData* okcdata, const char* input_file_name, const char* output_file_name, int times, int threshold)
{
    int cluster_amount, i;

    FILE *fp1 = fopen(input_file_name, "r");

    if ( fp1 == NULL )
        return false;

    FILE *fp2 = fopen(output_file_name, "w");

    LinkList *data_link1 = GetInputClusterData(fp1, cluster_amount);
    int *cluster_entries = GetInputClusterEntries(fp1, cluster_amount);
    fclose(fp1);
    if ( data_link1 == NULL ) {
        fclose(fp2);
        return false;
    }

    LinkList *data_link2 = new LinkList();

    double *max_radiuses = new double[times];
    max_radiuses[times-1] = 1.0;
    for ( i = times-1; i > 0; i-- )
         max_radiuses[i-1] = max_radiuses[i] - 1.0 / times;

    for ( i = 0; i < times; i++ )
    {
        DimClustering(cluster_amount, data_link1, data_link2, cluster_entries, max_radiuses[i], threshold);
    }

    WriteDimClusteringOutput(okcdata, fp2, cluster_amount, data_link2);

    fclose(fp2);
    delete data_link1;
    delete data_link2;

    return true;
}

LinkList *DimReducer::GetInputClusterData(FILE *fp, int &cluster_amount)
{
    LinkList *data_link = NULL;
    int i, j, dims;
    double *mins, *maxes, *fac, value;

    fscanf(fp, "%d %d\n", &cluster_amount, &dims);
    if ( cluster_amount <=0 || dims <= 0 )
        return NULL;

    mins = new double[cluster_amount];
    maxes = new double[cluster_amount];
    fac = new double[cluster_amount];

    for ( i = 0; i < cluster_amount; i++ )
    {
        fscanf(fp, "%lf %lf \n", &mins[i], &maxes[i]);
        if ( fabs(maxes[i]-mins[i]) > XMDV_DBL_EPSILON )
            fac[i] = 1/(maxes[i]-mins[i]);
        else
            fac[i] = 1;
    }

    data_link = new LinkList();

    for ( i = 0; i < dims; i++ )
    {
        DimClusterNode *new_node = new DimClusterNode();
        new_node->entries = 1;
        new_node->count = 0;
        new_node->radius = 0.0;
        new_node->parent = NULL;
        fscanf(fp, "%d", &(new_node->okc_id));
        new_node->data = new double[cluster_amount];
        for ( j = 0; j < cluster_amount; j++ )
        {
            fscanf(fp, "%lf", &value);
            new_node->data[j] = (value-mins[j]) * fac[j];
        }

        data_link->AddNodeAtEnd(new_node);
    }

    return data_link;
}

int *DimReducer::GetInputClusterEntries(FILE *fp, int cluster_amount)
{
    int *cluster_entries = new int[cluster_amount];

    int i;
    for (i = 0; i < cluster_amount; i++ )
    {
        fscanf(fp, "%d", &cluster_entries[i]);
    }
    return cluster_entries;
}


void DimReducer::WriteDimClusteringOutput(OkcData* okcdata, FILE *fp, int, LinkList* data_link)
{
    LinkListNode *node;

    int id = 0;
    node = data_link->GetFirst();
    while ( node != NULL )
    {
        NODEELE(node)->id = id;
        id++;
        node = data_link->GetNext(node);
    }

    fprintf(fp, "%7d \n", data_link->GetLength() );

    int parent_id;
    DimClusterNode *cnode;
    node = data_link->GetFirst();
    char temp_name[50];

    // dim_names is the array to contain the dimension names
    std::vector<char*> dim_names;
	okcdata->getDimNameArr(dim_names);

	while ( node != NULL )
    {
        cnode = NODEELE(node);
        if ( cnode->parent != NULL )
            parent_id = NODEELE(cnode->parent)->id;
        else
            parent_id = -1;

        fprintf(fp, "%7d %7d %7d ", cnode->id, parent_id, cnode->entries);
        fprintf(fp, "%14.7lf %d ", cnode->radius, cnode->okc_id);
		if(cnode->okc_id >=0)
            strcpy(temp_name, dim_names[cnode->okc_id]);
		 else
			 sprintf(temp_name, "Node%d", cnode->id);
		((DimClusterNode *)cnode)->name = new char[strlen(temp_name)+1];
		//C;

		strcpy((cnode->name), temp_name);
		fprintf(fp, "%s\n", cnode->name);
        node = data_link->GetNext(node);
    }

}

void DimReducer::DimClustering(int n, LinkList *data_link1, LinkList *data_link2, int *cluster_entries, double min_distance, int threshold)
{
    LinkListNode *node1, *node2, *center, *pair1, *pair2, *pair_temp;
    double *data1, *data2;
    int i;
    bool flag;
    int count;

    if ( data_link1->GetLength() < 1 )
        return;
    else if ( data_link1->GetLength() == 1 )
    {
        node1 = data_link1->GetFirst();
        data_link1->RemoveNode(node1);
        data_link2->AddNodeAtStart(node1);
        return;
    }

    LinkList *data_pairs = new LinkList();

    node1 = data_link1->GetFirst();
    while( node1 != NULL )
    {
        NODEELE(node1)->count = 0;
        node1 = data_link1->GetNext(node1);
    }

    node1 = data_link1->GetFirst();
    while ( node1 != NULL )
    {
        data1 = ((DimClusterNode *)node1->element)->data;

        node2 = data_link1->GetNext(node1);
        while ( node2 != NULL )
        {
            data2 = ((DimClusterNode *)node2->element)->data;

            flag = true;
            count = 0;
            for ( i = 0; i < n; i++ )
            {
                if ( fabs(data1[i] - data2[i]) <= min_distance )
                {
                    count += cluster_entries[i];
                    if ( count >= threshold )
                        break;
                }
            }

            if ( count < threshold )
                flag = false;

            if ( flag == true )
            {
                DataPair *pair = new DataPair(node1, node2);
                data_pairs->AddNodeAtEnd(pair);
                NODEELE(node1)->count++;
                NODEELE(node2)->count++;
            }

            node2 = data_link1->GetNext(node2);
        }

        node1 = data_link1->GetNext(node1);
    }

    while ( (center = FindWhoIsMaximum(data_link1)) != NULL &&  NODEELE(center)->count > 0 )
    {
        LinkListNode *new_node = new LinkListNode();
        NODEELE(center)->parent = new_node;
        data_link1->RemoveNode(center);
        data_link2->AddNodeAtStart(center);

        double *data = new double[n];
        int entries = NODEELE(center)->entries;
        for ( i = 0; i < n; i++ )
            data[i] = ( NODEELE(center)->data[i] ) * entries;

        pair1 = data_pairs->GetFirst();
        while ( pair1 != NULL && NODEELE(center)->count > 0)
        {
            node1 = NULL;
            if ( PAIRELE(pair1)->p1 == center )
                node1 = PAIRELE(pair1)->p2;
            else if ( PAIRELE(pair1)->p2 == center )
                node1 = PAIRELE(pair1)->p1;

            if ( node1 != NULL )
            {
                NODEELE(center)->count--;
                NODEELE(node1)->count--;

                NODEELE(node1)->parent = new_node;
                data_link1->RemoveNode(node1);
                data_link2->AddNodeAtStart(node1);

                entries += NODEELE(node1)->entries;
                for ( i = 0; i < n; i++ )
                    data[i] += ( NODEELE(node1)->data[i] ) * NODEELE(node1)->entries;
                pair_temp = pair1;
                pair1 = data_pairs->GetNext(pair_temp);
                data_pairs->DeleteNode(pair_temp);

                pair2 = data_pairs->GetFirst();
                while ( pair2 != NULL && NODEELE(node1)->count > 0)
                {
                    node2 = NULL;
                    if ( PAIRELE(pair2)->p1 == node1 )
                        node2 = PAIRELE(pair2)->p2;
                    else if ( PAIRELE(pair2)->p2 == node1 )
                        node2 = PAIRELE(pair2)->p1;
                    if ( node2 != NULL )
                    {
                        NODEELE(node2)->count--;
                        NODEELE(node1)->count--;
                        pair_temp = pair2;
                        pair2 = data_pairs->GetNext(pair_temp);
                        if ( pair1 == pair_temp )
                            pair1 = pair2;
                        data_pairs->DeleteNode(pair_temp);
                    }
                    else
                        pair2 = data_pairs->GetNext(pair2);
                }
            }
            else
                pair1 = data_pairs->GetNext(pair1);
        }

        for ( i = 0; i < n; i++ )
            data[i] /= entries;

        DimClusterNode *new_clusternode = new DimClusterNode();
        new_clusternode->data = data;
        new_clusternode->entries = entries;
        new_clusternode->count = 0;
        new_clusternode->radius = min_distance;
        new_clusternode->parent = NULL;
        new_clusternode->okc_id = -1;

        new_node->element = new_clusternode;

        data_link1->AddNodeAtStart(new_node);
    }

    delete data_pairs;

    if ( data_link1->GetLength() == 1 )
    {
        node1 = data_link1->GetFirst();
        data_link1->RemoveNode(node1);
        data_link2->AddNodeAtStart(node1);
    }

    return;
}

LinkListNode* DimReducer::FindWhoIsMaximum( LinkList *data_link )
{
    LinkListNode *node1, *who;
    int max, temp;
    if ( (node1 = data_link->GetFirst()) == NULL )
        return NULL;

    max = NODEELE(node1)->count;
    who = node1;

    while( node1 != NULL )
    {
        temp = NODEELE(node1)->count;

        if ( temp > max )
        {
            max = temp;
            who = node1;
        }

        node1 = data_link->GetNext(node1);
    }
    return who;
}
