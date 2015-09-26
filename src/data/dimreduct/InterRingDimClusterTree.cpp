/*
 * InterRingDimClusterTree.cpp
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#include "data/dimreduct/InterRingDimClusterTree.h"

#include <string.h>
#include <vector>
#include "datatype/LinkList.h"
#include "data/multidim/OkcData.h"
#include "data/dimreduct/InterRingDimCluster.h"
#include "data/dimreduct/InterRingClusterTree.h"
#include "data/dimreduct/InterRingCluster.h"
#include "util/algo/dimreduct/DimReducer.h"

InterRingDimClusterTree::InterRingDimClusterTree() : InterRingClusterTree()
{
}


InterRingDimClusterTree::~InterRingDimClusterTree()
{

}

//build the dim cluster tree according to a file;
//fname: name of the file;
bool InterRingDimClusterTree::buildInterRingDimClusterTreeFromFile(OkcData* okcdata, const char* dcfilename)
{
    int i;
    FILE* fpTest;

    // First try whether this file exists
    if ( ( fpTest = fopen(dcfilename, "r" ) ) != NULL )  {
        fclose(fpTest);
    } else {
    	// If this file does not exist, generate one dc file via dimension reduction algorithm
    	DimReducer dimr;
    	dimr.BuildDimClusterTree(okcdata, dcfilename);
    }

    //read the nodes into arrays from the file;
    FILE *fptr = fopen(dcfilename,"r");
    if (fptr==NULL)
        return false;

    // Get file size
    int num_nodes;
    if (fscanf(fptr, "%d\n", &num_nodes)<=0)
    {
        printf("Error reading the first line of the file %s\n", dcfilename);
        printf("Format of the first line: #nodes\n");
        return false;
    }
    // Allocate space for all node pointers and parent_ids;
    //
    std::vector<InterRingCluster *> nodes;
	std::vector<int> parent_ids;

	nodes.resize(num_nodes);
    parent_ids.resize(num_nodes);

	for ( i = 0; i < num_nodes; i++ )
	{
		nodes[i] = NULL;
		parent_ids[i] = -2;
	}

    for(i=0; i<num_nodes; i++)
	{
		nodes[i] = NULL;
    }

    int id, okc_id;
	char temp_name[200];
	for(i = 0; i < num_nodes; i++)
	{
		if ( fscanf(fptr, "%d", &id) != 1 )
            return false;
		nodes[id] = new InterRingDimCluster();


		fscanf(fptr, "%d %d %lf %d %s", &parent_ids[id], &(nodes[id]->entries), &(nodes[id]->radius), &okc_id, temp_name);
        ((InterRingDimCluster *)nodes[id])->okc_id = okc_id;
        //testing

		((InterRingDimCluster *)nodes[id])->name = new char[50]; //temp_name)+1];
		strcpy(((InterRingDimCluster *)nodes[id])->name, temp_name);
	}
	fclose(fptr);


	//construct the dim cluster tree according to the arrays;
	if ( constructInterRingClusterTree(num_nodes, nodes, parent_ids) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void InterRingDimClusterTree::generateDimReductResult(OkcData* okcdata, LinkList* current_list ) {
    // the data size of the new dataset is equal to that of input dataset
    int data_size = m_inputOkcData->getDataSize();
    // the dimension size is the lenght of current dimension list
    int dim_size = current_list->GetLength();

    // Set the data and dimension size for the new okcdata
    okcdata->data_size = data_size;
    okcdata->dims = dim_size;
    // Since this an intermediate result, its filename is null
    okcdata->filepath[0] = 0;
    // initialize all arrays
    okcdata->names.resize(dim_size);
    okcdata->dim_min.resize(dim_size);
    okcdata->dim_max.resize(dim_size);
    okcdata->cg_dim_min.resize(dim_size);
    okcdata->cg_dim_max.resize(dim_size);
    okcdata->cardinality.resize(dim_size);
    okcdata->data_buf->resize(dim_size * data_size);
    // The data buffer is owned by this okcdata itself
    okcdata->m_isBaseOkcData = true;

    // Get the min, max values and cardinality for each dimension
	std::vector<double> dim_min, dim_max, cg_dim_min, cg_dim_max;
	std::vector<int> cardinality;
	m_inputOkcData->getDimMinArr(dim_min);
	m_inputOkcData->getDimMaxArr(dim_max);
	m_inputOkcData->getCgDimMinArr(cg_dim_min);
	m_inputOkcData->getCgDimMaxArr(cg_dim_max);
	m_inputOkcData->getCardinality(cardinality);

	// calculate min, max, cardinality of each selected dimension in
	// the current dimension list
	// Convert the current dimension list to an array
	void** dim_arr_voidp = current_list->TransToArray();
	InterRingDimCluster** dim_arr = new InterRingDimCluster*[dim_size];
	int i, d;
	for (d=0; d<dim_size; d++) {
		dim_arr[d] = (InterRingDimCluster*)dim_arr_voidp[d];
    	// Get the name from dimension cluster node
		int namelen = strlen(dim_arr[d]->name);
		okcdata->names[d] = new char[namelen+1];
    	strcpy(okcdata->names[d], dim_arr[d]->name);
    	// calculate the dim_min
    	okcdata->dim_min[d] = dim_arr[d]->calculate(dim_min, dim_min, dim_max);
    	// calculate the dim_max
    	okcdata->dim_max[d] = dim_arr[d]->calculate(dim_max, dim_min, dim_max);
    	// calculate the cg_dim_min
    	okcdata->cg_dim_min[d] = dim_arr[d]->calculate(cg_dim_min, dim_min, dim_max);
    	// calculate the cg_dim_max
    	okcdata->cg_dim_max[d] = dim_arr[d]->calculate(cg_dim_max, dim_min, dim_max);
    	// calculte the cardinality
    	okcdata->cardinality[d] = dim_arr[d]->calculate(cardinality);
    }

    // Calculate each datapoint
    std::vector<double> data_buf;
    for (i=0; i<data_size; i++) {
    	m_inputOkcData->getData(data_buf, i);
    	for (d=0; d<dim_size; d++) {
    		(*(okcdata->data_buf))[i*dim_size+d] = dim_arr[d]->calculate(data_buf, dim_min, dim_max);
    	}
    }

    delete []dim_arr_voidp;
    delete []dim_arr;
}

void InterRingDimClusterTree::resetNames()
{
	((InterRingDimCluster *)root)->resetNames();
}


void InterRingDimClusterTree::resetSelection()
{
//    if ( g_globals.dim_reducer != NULL )
//    {
//        g_globals.dim_reducer->SetStatus("normal");
//        g_globals.dim_reducer->ResetRepresentativeDimList();
//    }
}

/*
void InterRingDimClusterTree::PCA()
{
	((InterRingDimCluster *)root)->PCA();
}
*/
