/*
 * OnlineDataClustering.cpp
 *
 *  Created on: Sep 16, 2009
 *      Author: mukherab
 */

#include <list>
#include <math.h>
#include <string.h>
#include <vector>

#include "util/algo/cluster/OnlineDataClustering.h"
#include "datatype/LinkList.h"
#include "datatype/LinkListNode.h"
#include "data/cluster/Cluster.h"



using std::list;
using std::pair;

OnlineDataClustering::OnlineDataClustering() {
}

OnlineDataClustering::~OnlineDataClustering() {
}


#define NODEELE(_node) ((ClusterNode *)_node->element)
#define PAIRELE(_node) ((DataPair *)_node->element)

// PAUL: lang_org
// Why would anyone want to use a custom list class that uses void ptrs?
bool OnlineDataClustering::GetInput(FILE * fp, int &dim, list<ClusterNode*> & data)
{
	int i, j, size, temp;
    double *dim_min, *dim_max, *fac, value;

    fscanf(fp, "%d %d\n", &dim, &size);
    if ( dim <=0 || size <= 0 )
        return false;

    dim_min = new double[dim];
    dim_max = new double[dim];
    fac = new double[dim];

    int ch;
    for ( i = 0; i < dim; i++ )
        while ( (ch = fgetc(fp)) != '\n' )
        {
        }

    for ( i = 0; i < dim; i++ )
    {
        fscanf(fp, "%lf %lf %d \n", &dim_min[i], &dim_max[i], &temp);
        if ( fabs(dim_max[i]-dim_min[i]) > 1e-9 )
            fac[i] = 1/(dim_max[i]-dim_min[i]);
        else
            fac[i] = 1;
    }

	data.clear();

#ifdef _DEBUG
	int cntInvalid = 0;
#endif
    for ( i = 0; i < size; i++ )
    {
        ClusterNode *new_clusternode = new ClusterNode();
        new_clusternode->entries = 1;
        new_clusternode->count = 0;
        new_clusternode->radius = 0.0;
        new_clusternode->data = new double[dim];
        new_clusternode->parent = NULL;
        for ( j = 0; j < dim; j++ )
        {
            fscanf(fp, "%lf", &value);
			// PAUL: cluster_org
			// Is the point invalid (by being out of bounds?)
			if (value < dim_min[j] || value > dim_max[j])
			{
#ifdef _DEBUG
				cntInvalid++;
				FILE *fpdbg = fopen("xmdvtool.log","a");
				fprintf(fpdbg, "No. %d: row %d, col %d, val = %lf, min= %lf, max=%lf\n",
					cntInvalid, i, j, value, dim_min[j], dim_max[j]);
				fclose(fpdbg);
#endif
				delete new_clusternode;
				new_clusternode = NULL;
				break;
			}
			new_clusternode->data[j] = (value-dim_min[j]) * fac[j];
        }

		if (new_clusternode)
			data.push_back(new_clusternode);
    }
	return true;
}

list<OnlineDataClustering::ClusterNode*>::iterator OnlineDataClustering::WhoIsMaximum( list<OnlineDataClustering::ClusterNode*> &data_link )
{
    list<ClusterNode*>::iterator node1, who;
    int max, temp;
    if ( (node1 = data_link.begin()) == data_link.end() )
        return data_link.end();

    max = (*node1)->count;
    who = node1;

    while( node1 != data_link.end() )
    {
        temp = (*node1)->count;

        if ( temp > max )
        {
            max = temp;
            who = node1;
        }

        ++node1;
    }
    return who;
}


int InitialClusterSortDim = 0;

inline bool OnlineDataClustering::ClusterPredicate(ClusterNode* lhs, ClusterNode* rhs)
{
	return lhs->data[InitialClusterSortDim] < rhs->data[InitialClusterSortDim];
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))


void OnlineDataClustering::Clustering(int n, list<ClusterNode*> & pointset, list<ClusterNode*> & clusterset, double min_distance)
{

	typedef list<ClusterNode*>::iterator iter;
	typedef pair<iter, iter> iter_pair;

	iter node1, node2, center;
	list<iter_pair>::iterator pair1, pair2, pair_temp;
    double *data1, *data2;
    int i;
    bool flag;

    if ( pointset.empty() )
        return;
	// Test if length == 1 using a safe method (since .size() could be
	// O(n)) depending on your implementation.
    else if ( ++(pointset.begin()) == pointset.end() )
    {
        node1 = pointset.begin();
        clusterset.push_front(*node1);
		pointset.erase(node1);
        return;
    }

	// A pair of iterators of lists of ClusterNode pointers
	list<iter_pair> data_pairs;

    for(node1 = pointset.begin(); node1 != pointset.end(); ++node1 )
        (*node1)->count = 0;

	int checks = 0;

	// ***********************************************************************
	// * In order to make the algorithm go from about O(n^2) to about O(n ln n)
	// * sort nodes by a single dimension and then do a double iteration
	// * using the nodes in order. That way, it is possible to greatly limit
	// * the number of candidate pairs that we have to test.
    // *
    // * This works since the distance of ALL dimensions have to be less than
	// * min. Sorting a dimension can limit how far we have to iterate after
	// * node1 because no node after the first one which is outside of the
	// * range by the sorted dimension can possibly be pairable to node1.
    // ***********************************************************************

	pointset.sort(ClusterPredicate);

	//printf("****** Done with Sorting ***** (%d)\n", pointset.size());

    node1 = pointset.begin();
    for ( node1 = pointset.begin(); node1 != pointset.end(); ++node1 )
    {
        data1 = (*node1)->data;

        node2 = node1;
		++node2;
        for (; node2 != pointset.end(); ++node2 )
        {
            data2 = (*node2)->data;
			// PAUL: cluster_org
			// Since we sorted based on a single dimension i = 0,
			// data2[0] >= data1[0]. That allows us to cut out fabs.
			if ( (data2[InitialClusterSortDim] - data1[InitialClusterSortDim]) > min_distance )
				break;	// No use searching after this point

			checks++;

            flag = true;
			// PAUL: cluster_org
            for ( i = 0; i < n; i++ )
            {
				if (i == InitialClusterSortDim)
					continue;	// This dimension was already checked

				// PAUL: lang_opt
				if ((data1[i] > data2[i] ? data1[i] - data2[i] : data2[i] - data1[i]) > min_distance)
                {
                    flag = false;
                    break;
                }
            }

            if ( flag == true )
            {
                data_pairs.push_back(iter_pair(node1, node2));
                (*node1)->count++;
                (*node2)->count++;
            }
        }
    }

	// PAUL: test
	//std::cout << "****** Done with Pairings ("<< data_pairs.size() <<") ("<<checks<<" checks were made) ******" << std::endl;

    while ( (center = WhoIsMaximum(pointset)) != pointset.end()
    		&& (*center)->count > 0 )
    {
		// This is moved up so it can become the parent of (*center)
		ClusterNode *new_clusternode = new ClusterNode();
		//ClusterNode * new_node = new ClusterNode(); //new CLin();
        (*center)->parent = new_clusternode;
		// Move from pointset to clusterset's start
		// Notice that iterators remain valid
		clusterset.splice(clusterset.begin(), pointset, center);

        double *data = new double[n];
        int entries = (*center)->entries;
        for ( i = 0; i < n; i++ )
            data[i] = ( (*center)->data[i] ) * entries;

        pair1 = data_pairs.begin();
        while ( pair1 != data_pairs.end() && (*center)->count > 0)
        {
            node1 = pointset.end();
            if ( (*pair1).first == center )
                node1 = (*pair1).second;
            else if ( (*pair1).second == center )
                node1 = (*pair1).first;

            if ( node1 != pointset.end() )
            {
                (*center)->count--;
                (*node1)->count--;

                (*node1)->parent = new_clusternode;

				clusterset.splice(clusterset.begin(), pointset, node1);

                entries += (*node1)->entries;
                for ( i = 0; i < n; i++ )
                    data[i] += ( (*node1)->data[i] ) * (*node1)->entries;
                //pair_temp = pair1++;
                //pair1 = data_pairs->GetNext(pair_temp);
                data_pairs.erase(pair1++);

                pair2 = data_pairs.begin();
                while ( pair2 != data_pairs.end() && (*node1)->count > 0)
                {
                    node2 = pointset.end();
                    if ( (*pair2).first == node1 )
                        node2 = (*pair2).second;
                    else if ( (*pair2).second == node1 )
                        node2 = (*pair2).first;
                    if ( node2 != pointset.end() )
                    {
                        (*node2)->count--;
                        (*node1)->count--;
                        pair_temp = pair2++;
                        if ( pair1 == pair_temp )
                            pair1 = pair2;
                        data_pairs.erase(pair_temp);
                    }
                    else
                        ++pair2;
                }
            }
            else
                ++pair1;
        }

        for ( i = 0; i < n; i++ )
            data[i] /= entries;

        new_clusternode->data = data;
        new_clusternode->entries = entries;
        new_clusternode->count = 0;
        new_clusternode->radius = min_distance;
        new_clusternode->parent = NULL;

        //new_node->element = new_clusternode;
        //pointset->AddNodeAtStart(new_node);
		pointset.push_front(new_clusternode);
    }

    if ( pointset.size() == 1 )
    {
        node1 = pointset.begin();
		clusterset.push_front(*node1);
        pointset.erase(node1);
    }

    return;
}

void OnlineDataClustering::WriteOutput(FILE *fp, int dim, list<ClusterNode*> & data_link)
{

	int i;
	list<ClusterNode*>::iterator node;


    int id = 0;
    node = data_link.begin();
    while ( node != data_link.end() )
    {
        (*node)->id = id;
        id++;
        ++node;
    }

    fprintf(fp, "%7d %7d \n", id, dim);

    int parent_id;
    ClusterNode *cnode;
    node = data_link.begin();
    while ( node != data_link.end() )
    {
        cnode = (*node);
        if ( cnode->parent != NULL )
            parent_id = (cnode->parent)->id;
        else
            parent_id = -1;

        fprintf(fp, "%7d %7d %7d ", cnode->id, parent_id, cnode->entries);

        for ( i = 0; i < dim; i++ )
        {
			fprintf(fp, "%14.7lf ", cnode->data[i]*cnode->entries);
        }
        fprintf(fp, "%14.7lf ", cnode->radius);
        ++node;
    }
}

bool OnlineDataClustering::OnlineClusterFileToCfFile(const char* okc_file_name, const char* cf_file_name, int times)
{
    int dim, i;

    FILE *fp1 = fopen(okc_file_name, "r");

    if ( fp1 == NULL )
        return false;

    FILE *fp2 = fopen(cf_file_name, "w");

    list<ClusterNode*> data_link1;
	bool isGood = GetInput(fp1, dim, data_link1);
    fclose(fp1);
    if ( !isGood )
    {
        fclose(fp2);
        return false;
    }

    list<ClusterNode*> data_link2;

    double *max_radiuses = new double[times];
    max_radiuses[times-1] = 1.0;
    for ( i = times-1; i > 0; i-- )
         max_radiuses[i-1] = max_radiuses[i] - 1.0 / times;

	// PAUL: cluster_org
	// Find the best cluster
	int best_dim = 0;
	float best_dim_error = (float)1e30;
	int Buckets[1000]; // 1000 (for spills)
	for (i = 0; i < dim; ++i)
	{
		for (int j = 0; j < 1000; ++j)
			Buckets[j] = 0;

		float current_error = 0.0f;

		for (list<ClusterNode*>::const_iterator k = data_link1.begin(); k != data_link1.end();)
		{
			int index = (int)((*k)->data[i] * 999.0f);
			Buckets[index]++;
			++k;
		}

		for (int j = 0; j < 1000; ++j)
			current_error += fabs(data_link1.size() / 1000.0f - Buckets[j]);

		if (current_error < best_dim_error)
		{
			best_dim = i;
			best_dim_error = current_error;
		}
	}
	InitialClusterSortDim = best_dim;

    for ( i = 0; i < times; i++ )
    {
        Clustering(dim, data_link1, data_link2, max_radiuses[i]);
    }

    WriteOutput(fp2, dim, data_link2);

    fclose(fp2);

    return true;
}


/*******************************************************************************************************/
std::vector<Cluster> nodes;

void OnlineDataClustering::GetMaxAndMin(int size, int dimension, int k)
{
	int i, j, ii;

	int num_children = nodes[k].num_children;

	if ( num_children <= 0 )
	{
		for ( i = 0; i < dimension; i++ )
		{
			nodes[k].min[i] = nodes[k].sx[i];
			nodes[k].max[i] = nodes[k].sx[i];
		}
		return;
	}
	else
	{
		std::vector<double> max, min;
		max.resize(dimension);
		min.resize(dimension);

		i = nodes[k].child[0];
		GetMaxAndMin(size, dimension, i);
		for ( j = 0; j < dimension; j++ )
		{
			max[j] = nodes[i].max[j];
			min[j] = nodes[i].min[j];
		}

		for ( ii = 1; ii < num_children; ii++ )
		{
			i = nodes[k].child[ii];
			GetMaxAndMin(size, dimension, i);
			for ( j = 0; j < dimension; j++ )
			{
				if ( max[j] < nodes[i].max[j] )
					max[j] = nodes[i].max[j];
				if ( min[j] > nodes[i].min[j] )
					min[j] = nodes[i].min[j];
			}
		}
		for ( i = 0; i < dimension; i++ )
		{
			nodes[k].max[i] = max[i];
			nodes[k].min[i] = min[i];
		}
		return;
	}
}

bool OnlineDataClustering::ReadCfFile(const char*, const char * cfFileName)
{
    FILE *fptr = fopen(cfFileName,"r");
	//int N = strlen(okcFileName);
	//char *qual_file_name;
   // qual_file_name = new char[N+1];
	//strcpy(qual_file_name, okcFileName);
    //strcpy(&qual_file_name[N-4],".qcf");


	//FILE *f1 = fopen(qual_file_name, "w");



//    FILE *fptr1 = fopen("output","w");

	if (fptr==NULL) {
//        fprintf(fptr1, "Cluster file \"%s\" does not exist. Program terminated.\n", fname);
        return false;
    }

    int num_nodes, dimension;
    if (fscanf(fptr, "%d %d\n", &num_nodes, &dimension)<=0)
    {
//        fprintf(fptr1, "Error reading the first line of the file %s\n", fname);
//        fprintf(fptr1, "Format of the first line: #nodes #dimension\n");
        return false;
    }
	//fprintf(f1, "%d %d\n", num_nodes, dimension);
	nodes.clear();
    nodes.resize(num_nodes);

    for (int i=0; i<num_nodes; i++) {
        nodes[i].sx.resize(dimension);
        nodes[i].max.resize(dimension);
        nodes[i].min.resize(dimension);
        nodes[i].num_children = 0;
        nodes[i].entries = 0;
		//nodes[i].clustqual = 0.0;
		//nodes[i].dataqual = 0.0;
		nodes[i].name="";
    }

    int k;
    for (k=0; k<num_nodes; k++ )
        for (int i=0;i<dimension; i++)
		{
            nodes[k].sx[i] = 0.0;
            nodes[k].min[i] = 0.0;
            nodes[k].max[i] = 0.0;
		}

	double radius;
	for (k=0; k<num_nodes; k++)
    {
        int id;
        fscanf(fptr, "%d %d %d", &id, &nodes[k].parent, &nodes[k].entries);

        for (int i=0; i<dimension; i++)
		{
            fscanf(fptr, "%lf ", &(nodes[k].sx[i]));
        }

        fscanf(fptr, "%lf", &radius);
	//	nodes[k].clustqual = radius;
		//nodes[k].dataqual = 1; //float (rand()%10)/10;//float (rand()%10)/10;
		//fprintf(f1, "%d %f %f\n", id, nodes[k].clustqual, nodes[k].dataqual);

        if ( nodes[k].parent >= 0 ) {
            nodes[nodes[k].parent].child.push_back(k);
            nodes[nodes[k].parent].num_children++;
        }
    }
	fclose(fptr);
	//fclose(f1);
//    info_dialog->SetProcess(30, "Processing, please wait...");

	GetMaxAndMin(num_nodes, dimension, 0);
//    info_dialog->SetProcess(60, "Processing, please wait...");

	return true;
}


bool OnlineDataClustering::WriteAsciiCgFile(const char *oldfile, const char *newfile)
{
    int i, k;

	FILE *oldfptr = fopen(oldfile,"r");
    FILE *newfptr = fopen(newfile,"w");

//    FILE *fptr1 = fopen("output","w");

	if (newfptr==NULL) {
        return false;
    }

	if (oldfptr==NULL) {
        return false;
    }

    int num_nodes, dimension;
    fscanf(oldfptr, "%d %d\n", &num_nodes, &dimension);
	fprintf(newfptr, "%d %d\n", num_nodes, dimension);

	int parent, entries, id;
	double sx, radius;
	for (k=0; k<num_nodes; k++)
    {
        fscanf(oldfptr, "%d %d %d ", &id, &parent, &entries);
        fprintf(newfptr, "%d %d %d ", id, parent, entries);

        for (i=0; i<dimension; i++)
		{
            fscanf(oldfptr, "%lf ", &sx);
            fprintf(newfptr, "%14.7lf ", sx);
        }

        fscanf(oldfptr, "%lf \n", &radius);

		fprintf(newfptr, "%14.7lf ", radius);

        for (i=0; i<dimension; i++)
		{
			fprintf(newfptr, "%14.7lf ", nodes[k].max[i]);
        }

        for (i=0; i<dimension; i++)
		{
			fprintf(newfptr, "%14.7lf ", nodes[k].min[i]);
        }
		fprintf(newfptr, "\n");
	}

	fclose(newfptr);
	fclose(oldfptr);

	return true;
}


bool OnlineDataClustering::WriteBinaryCgFile(const char *oldfile, const char *newfile)
{
    int i, k;

	FILE *oldfptr = fopen(oldfile,"r");

    if (oldfptr==NULL) {
        return false;
    }

    FILE *newfptr = fopen(newfile,"wb");

	if (newfptr==NULL) {
        fclose(oldfptr);
        return false;
    }


    unsigned int magic_code;
    magic_code = 0xcecfcecf;
    fwrite(&magic_code, sizeof(unsigned int), 1, newfptr);

    int num_nodes, dimension;
    fscanf(oldfptr, "%d %d\n", &num_nodes, &dimension);
	fwrite(&num_nodes, sizeof(int), 1, newfptr);
	fwrite(&dimension, sizeof(int), 1, newfptr);

	int parent, entries, id;
	double sx, radius;
	for (k=0; k<num_nodes; k++)
    {
        fscanf(oldfptr, "%d %d %d ", &id, &parent, &entries);
    	fwrite(&id, sizeof(int), 1, newfptr);
	    fwrite(&parent, sizeof(int), 1, newfptr);
	    fwrite(&entries, sizeof(int), 1, newfptr);

        for (i=0; i<dimension; i++)
		{
            fscanf(oldfptr, "%lf ", &sx);
    	    fwrite(&sx, sizeof(double), 1, newfptr);
        }

        fscanf(oldfptr, "%lf \n", &radius);

	    fwrite(&radius, sizeof(double), 1, newfptr);

        for (i=0; i<dimension; i++)
		{
    	    fwrite(&(nodes[k].max[i]), sizeof(double), 1, newfptr);
        }

        for (i=0; i<dimension; i++)
		{
    	    fwrite(&(nodes[k].min[i]), sizeof(double), 1, newfptr);
        }
	}

	fclose(newfptr);
	fclose(oldfptr);

	return true;

}

bool OnlineDataClustering::WriteCgFile(const char *oldfile, const char *newfile, bool binary)
{
    if ( binary )
        return WriteBinaryCgFile(oldfile, newfile);
    else
        return WriteAsciiCgFile(oldfile, newfile);

}


bool OnlineDataClustering::TransCfToCgFile(const char* okcFileName, const char * cfFileName, bool binary)
{
	bool succ = ReadCfFile(okcFileName, cfFileName);

	if ( succ )
	{
		int N = strlen(cfFileName);
		char* newFile = new char[N];
		strcpy(newFile,cfFileName);
		strcpy(&newFile[N-3],".cg");

		succ = WriteCgFile(cfFileName, newFile, binary);
    }

    return succ;
}

bool OnlineDataClustering::OnlineClusterOkcFileToCgFile(const char* okc_file_name, int repeat_times, bool binary)
{
    int N = strlen(okc_file_name);
	char *cf_file_name, *cg_file_name;
    cf_file_name = new char[N+1];
    cg_file_name = new char[N+1];
    strcpy(cf_file_name, okc_file_name);
    strcpy(cg_file_name, okc_file_name);
    strcpy(&cf_file_name[N-4],".cf");
    strcpy(&cg_file_name[N-4],".cg");

     if ( ! OnlineClusterFileToCfFile(okc_file_name, cf_file_name, repeat_times) )
         return false;
     if ( ! TransCfToCgFile(okc_file_name, cf_file_name, binary) )
         return false;

     return true;
}
