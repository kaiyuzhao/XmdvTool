/*
 * ClusterTree.cpp
 *
 *  Created on: Jun 4, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class ClusterTree represents a hierarchical tree
 * which is the result of a hierarchical clustering algorithm
 */

#include "data/cluster/ClusterTree.h"

#include <assert.h>
#include <cmath>

#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/cluster/OkcDataModifierClusterNodeMax.h"
#include "data/cluster/OkcDataModifierClusterNodeMin.h"
#include "data/cluster/OkcDataModifierClusterColor.h"
#include "data/cluster/OkcDataModifierClusterHighlight.h"
#include "data/cluster/OkcDataModifierClusterEntries.h"
#include "data/cluster/Cluster.h"
#include "data/cluster/HierHist.h"
#include "data/cluster/TreeContour.h"
#include "util/exception/ReadCfCgException.h"
#include "operator/sbb/HierDisplayInformation.h"
#include "util/algo/cluster/OnlineDataClustering.h"

ClusterTree::ClusterTree() :
	m_brushSemantic(SBBSEMANTIC_ALL), dimension(0)
{}

ClusterTree::~ClusterTree() {
	int i;
	for (i=0; i<(int)nodes.size(); ++i) {
		nodes[i].child.clear();
		nodes[i].mean.clear();
		nodes[i].max.clear();
		nodes[i].min.clear();
		nodes[i].all_mean.clear();
		nodes[i].sx.clear();
	}
	nodes.clear();
	min_extents.clear();
	max_extents.clear();
}

bool ClusterTree::ReadClusterTreeFile(char* okcFileName, char* cgFileName, OkcData* dataset) {
    assert(dataset);

    //test if file exists. If not, online generate one.
    FILE *fptr = fopen(cgFileName,"rb");
    if (fptr==NULL) {
    	OnlineDataClustering::OnlineClusterOkcFileToCgFile(okcFileName, 100, true);
        fptr = fopen(cgFileName,"rb");
        if (fptr==NULL)
            return false;
    }

    //test if binary file.
    unsigned int magic_code;
    fread(&magic_code, sizeof(unsigned int), 1, fptr);

    fclose(fptr);

    if ( magic_code == CGFILE_MAGIC_CODE )  {
        return ReadBinaryClusterTreeFile(cgFileName, dataset);
    } else {
        return ReadTextClusterTreeFile(cgFileName, dataset);
    }
}

bool ClusterTree::ReadBinaryClusterTreeFile(const char* fname, OkcData* dataset) {
	char str_temp[500];
    int k, i, d;
    assert(dataset);

    FILE *fptr = fopen(fname,"rb");
    if (fptr==NULL)
        return false;

    // skip the magic code at the beginning of the file
    //fseek(fptr, SEEK_SET, sizeof(unsigned int));
	unsigned int magic_code;
    fread(&magic_code, sizeof(unsigned int), 1, fptr);

    // Get file size
    int num_nodes;
    if ( fread(&num_nodes, sizeof(int), 1, fptr) != 1 || fread(&dimension, sizeof(int), 1, fptr) != 1 )   {
		sprintf(str_temp, "Format of the first line: #nodes #dimension\n");
		throw ReadCfCgException(str_temp);
		return false;
    }
    assert(dimension == dataset->getDimSize());

    // Allocate space for all nodes
    //
    nodes.resize(num_nodes);

    for (i=0; i<num_nodes; i++) {
        nodes[i].sx.resize(dimension);
        nodes[i].mean.resize(dimension);
        nodes[i].max.resize(dimension);
        nodes[i].min.resize(dimension);
        nodes[i].num_children = 0;
        nodes[i].entries = 0;
    }

    for (k=0; k<num_nodes; k++ )
        for (i=0;i<dimension; i++)
            nodes[k].sx[i] = 0.0;

// SID
    double *buffer;
    buffer = new double[dimension];
    // cg_dim_max, cg_dim_min are the exact max and min values for dimensions.
    std::vector<double> cg_dim_max, cg_dim_min;
    dataset->getCgDimMinArr(cg_dim_min);
    dataset->getCgDimMaxArr(cg_dim_max);
    for (k=0; k<num_nodes; k++) {
        int id;
        fread(&id, sizeof(int), 1, fptr);
        nodes[k].nodeid = id;
        fread(&nodes[k].parent, sizeof(int), 1, fptr);
        fread(&nodes[k].entries, sizeof(int), 1, fptr);

        fread(buffer, sizeof(double), dimension, fptr);
        for (i=0; i<dimension; i++) {
              nodes[k].sx[i] = buffer[i];

            // Compute the mean
            if ( nodes[k].entries > 0) {
                double sum_dim = buffer[i] * (cg_dim_max[i]-cg_dim_min[i]) +
                                 (nodes[k].entries * cg_dim_min[i]);

                nodes[k].mean[i] = sum_dim/nodes[k].entries;
            }
        }

        fread(&(nodes[k].sqrt_radius), sizeof(double), 1, fptr);

        fread(buffer, sizeof(double), dimension, fptr);
        for (i=0; i<dimension; i++)	{
			nodes[k].max[i] = buffer[i] * (cg_dim_max[i]-cg_dim_min[i]) + cg_dim_min[i];
		}

        fread(buffer, sizeof(double), dimension, fptr);
        for (i=0; i<dimension; i++) {
			nodes[k].min[i] = buffer[i] * (cg_dim_max[i]-cg_dim_min[i]) + cg_dim_min[i];
		}

        if ( nodes[k].parent >= 0 ) {
            nodes[nodes[k].parent].child.push_back(k);
            nodes[nodes[k].parent].num_children++;
        }

    }

    fclose(fptr);
    delete [] buffer;

    // Init the min/max extents
    //
    min_extents.resize(dimension);
    max_extents.resize(dimension);

    for (d=0; d<dimension; d++) {
        min_extents[d] = HUGE_VAL;
        max_extents[d] = -HUGE_VAL;
    }

    treemap_limit_number = (int) (nodes[0].entries * TREEMAP_LIMIT);
    if ( treemap_limit_number < 1 )
        treemap_limit_number = 1;

    return true;
}

bool ClusterTree::ReadTextClusterTreeFile(const char* fname, OkcData* dataset) {
	char str_temp[500];
	int i, k;
    FILE* fptr = fopen(fname,"r");

    // Get file size
    int num_nodes;
    if (fscanf(fptr, "%d %d\n", &num_nodes, &dimension)<=0) {
		sprintf(str_temp, "Error reading the first line of the file %s\n", fname);
		throw ReadCfCgException(str_temp);
		return false;
    }
    assert(dimension == dataset->getDimSize());

    // Allocate space for all nodes
    //
    nodes.resize(num_nodes);

    for (i=0; i<num_nodes; i++) {
        nodes[i].sx.resize(dimension);
        nodes[i].mean.resize(dimension);
        nodes[i].max.resize(dimension);
        nodes[i].min.resize(dimension);
        nodes[i].num_children = 0;
        nodes[i].entries = 0;
	}

    for (k=0; k<num_nodes; k++ )
        for (i=0;i<dimension; i++)
            nodes[k].sx[i] = 0.0;

    double max, min;
    std::vector<double> cg_dim_max, cg_dim_min;
    dataset->getCgDimMinArr(cg_dim_min);
    dataset->getCgDimMaxArr(cg_dim_max);
    for (k=0; k<num_nodes; k++) {
        int id;
        fscanf(fptr, "%d %d %d", &id, &nodes[k].parent, &nodes[k].entries);
        nodes[k].nodeid = id;

        for (i=0; i<dimension; i++) {
			//Read the actual value of the dimension cluster and store in the node
            fscanf(fptr, "%lf ", &(nodes[k].sx[i]));

            // Compute the mean
            if ( nodes[k].entries > 0) {
                double sum_dim = nodes[k].sx[i] * (cg_dim_max[i]-cg_dim_min[i]) +
                                 (nodes[k].entries * cg_dim_min[i]);
                nodes[k].mean[i] = sum_dim/nodes[k].entries;
            }
        }

        fscanf(fptr, "%lf ", &nodes[k].sqrt_radius);

        for (i=0; i<dimension; i++) {
            fscanf(fptr, "%lf ", &max);
			nodes[k].max[i] = max * (cg_dim_max[i]-cg_dim_min[i]) + cg_dim_min[i];
		}

        for (i=0; i<dimension; i++)	{
            fscanf(fptr, "%lf ", &min);
			nodes[k].min[i] = min * (cg_dim_max[i]-cg_dim_min[i]) + cg_dim_min[i];
		}

        fscanf(fptr, "\n");

        if ( nodes[k].parent >= 0 ) {
            nodes[nodes[k].parent].child.push_back(k);
            nodes[nodes[k].parent].num_children++;
        }
    }

    fclose(fptr);

    // Init the min/max extents
    //
    min_extents.resize(dimension);
    max_extents.resize(dimension);

    for (int d=0; d<dimension; d++) {
        min_extents[d] = HUGE_VAL;
        max_extents[d] = -HUGE_VAL;
    }

    treemap_limit_number = (int) (nodes[0].entries * TREEMAP_LIMIT);
    if ( treemap_limit_number < 1 )
        treemap_limit_number = 1;
    return true;

}

ClusterTree* ClusterTree::hierInitClusterTree(char* okcFileName, char* cgFileName, OkcData* dataset,
		HierDisplayInformation* hier_info, TreeContour* tree_contour) {
    ClusterTree *btree;
    btree = new ClusterTree();

	if (!btree->ReadClusterTreeFile(okcFileName, cgFileName, dataset))
	{
		delete btree;
		return 0;
	}

    ClusterTree::hierInitClusterColors(btree, &btree->nodes[0], 0, 1);
    hierClearNodeFlags(btree);

	HierHist::cfHistoInit(btree);
    tree_contour->cfNavInit(btree, hier_info);

    double root_radius = btree->nodes[0].sqrt_radius;
    ClusterTree::hierMarkContourNodes( btree, root_radius*hier_info->cluster_radius, IS_CONTOUR0 );
    ClusterTree::hierMarkContourNodes( btree, root_radius*hier_info->brushed_radius, IS_CONTOUR1 );
   	hier_info->root_radius = btree->nodes[0].sqrt_radius;

   	ClusterTree::SetupBrushParameters(btree, hier_info);

    return btree;
}

void ClusterTree::hierInitClusterColors(ClusterTree* btree, Cluster* node, double L, double R)
{
    node->color = .5*(L+R);

    if (!node->num_children) return;

    // The color is evenly distributed into the range  [L,R]
    double k = (R-L)/(double)(node->num_children);

    for (int i=0; i<node->num_children; i++) {
    	// Recursively assign color to its children node.
    	ClusterTree::hierInitClusterColors(btree, &btree->nodes[node->child[i]], L+i*k, L+(i+1)*k);
    }
}

void ClusterTree::hierClearNodeFlags(ClusterTree* btree, int bits)
{
	int i;
    for (i=0;i<(int)(btree->nodes.size());i++) {
        Cluster &c = btree->nodes[i];
        BITCLEAR(c.flags,bits);
    }
}

void ClusterTree::hierMarkContourNodes(ClusterTree* btree, double contour_radius, int FLAG)
{
    // A node c is on contour if:
    // c.parent.radius > contour_radius AND
    //  (a)  c.radius <= contour_radius, OR
    //  (b)  c.radius > contour_radius AND c is a leaf
    //
	int i;
    for (i=0;i<(int)btree->nodes.size();i++)
    {
        Cluster &c = btree->nodes[i];

        BITCLEAR(c.flags,FLAG);

        double par_radius = HUGE_VAL;
        if (c.parent>=0) par_radius = btree->nodes[c.parent].sqrt_radius;

        if (par_radius > contour_radius)
        {
            if (c.sqrt_radius <= contour_radius || c.num_children==0)
            {
                assert(c.color<=1.0 && c.color>=0);
                BITSET(c.flags,FLAG);
            }
        }
    }
}


void ClusterTree::SetupBrushParameters(ClusterTree* btree,
		HierDisplayInformation* hier_info)
{
	ClusterTree::hierInitExtents(btree);
	ClusterTree::hierClearNodeFlags(btree, IS_BRUSHED);
	ClusterTree::hierClearNodeFlags(btree, IS_BRUSHED_ANY);
	ClusterTree::hierMarkBrushedClustersByColor(btree, &(btree->nodes[0]),
			hier_info->tree_brush[0], hier_info->tree_brush[1]);
}

void ClusterTree::hierInitExtents(ClusterTree* btree)
{
    for (int d=0; d<btree->dimension; d++) {
        btree->min_extents[d] = HUGE_VAL;
        btree->max_extents[d] = -HUGE_VAL;
    }
}

void ClusterTree::hierMarkBrushedClustersByColor(ClusterTree* btree, Cluster* node, double L, double R)
{
    BITCLEAR(node->flags,IS_BRUSHED);
    BITCLEAR(node->flags,IS_BRUSHED_ANY);

    int i;
    for (i=0; i<node->num_children; i++) {
    	ClusterTree::hierMarkBrushedClustersByColor(btree, &(btree->nodes[node->child[i]]), L, R);
    }

    // Reached the leaf node?
    //
    if (node->num_children == 0)
    {
        if (node->color>=L && node->color<=R)
        {
            BITSET(node->flags,IS_BRUSHED);
            BITSET(node->flags,IS_BRUSHED_ANY);

            // Setup min/max extents
            //
            int d;
            for (d=0; d<btree->dimension; d++)
			{
                if (btree->min_extents[d] > node->mean[d])
					btree->min_extents[d] = node->mean[d];

                if (btree->max_extents[d] < node->mean[d])
					btree->max_extents[d] = node->mean[d];
            }
        }

        return;
    }

    // Propagate markers up to parents
    //
    int brushed=0;
    bool brushed_any = false;
    for (int j=0; j<node->num_children; j++) {
        if (BITTEST(btree->nodes[node->child[j]].flags,IS_BRUSHED)) {
            brushed++;
        }
        if (BITTEST(btree->nodes[node->child[j]].flags,IS_BRUSHED_ANY)) {
            brushed_any = true;
        }
    }

	if ( btree->m_brushSemantic == SBBSEMANTIC_ALL) /* ALL */
	{
		// All my children are marked, mark myself too
		//
		if (brushed == node->num_children)
		{
			BITSET(node->flags,IS_BRUSHED);
		}
	}
	else if ( btree->m_brushSemantic == SBBSEMANTIC_ANY ) /* ANY */
	{
		// Any my children is marked, mark myself too
		//
		if ( brushed_any == true )
		{
			BITSET(node->flags,IS_BRUSHED);
		}
	}

	if ( brushed_any == true ) {
		BITSET(node->flags,IS_BRUSHED_ANY);
	}
}

void ClusterTree::generateSBBResult(OkcData* okcdata) {
	// Copy the dimension attributes from the
	// input of the cluster tree
	okcdata->copyFrom(this->m_inputOkcData);

	okcdata->data_size = 0;
	okcdata->data_buf = new std::vector<double>();

	OkcDataModifierManager* mm = okcdata->getOkcDataModifierManager();

	// Add two modifiers to represent the max and min values
	// for each cluster node
	OkcDataModifierClusterNodeMax* modifierMax = new OkcDataModifierClusterNodeMax(okcdata);
	modifierMax->initClusterMaxArr();
	mm->addOkcDataModifier(XmdvTool::MODIFIER_CLUSTERNODEMAX, modifierMax);

	OkcDataModifierClusterNodeMin* modifierMin = new OkcDataModifierClusterNodeMin(okcdata);
	modifierMin->initClusterMinArr();
	mm->addOkcDataModifier(XmdvTool::MODIFIER_CLUSTERNODEMIN, modifierMin);

	// Add one modifier to represent cluster color
	OkcDataModifierClusterColor* modifierColor = new OkcDataModifierClusterColor(okcdata);
	modifierColor->initColorArr();
	mm->addOkcDataModifier(XmdvTool::MODIFIER_CLUSTERCOLOR, modifierColor);

	// Add one modifier to represent cluster color
	OkcDataModifierClusterHighlight* modifierCH = new OkcDataModifierClusterHighlight(okcdata);

	modifierCH->initHighlightArr();
	mm->addOkcDataModifier(XmdvTool::MODIFIER_CLUSTERHIGHLIGHT, modifierCH);

	// Add one modifier to represent cluster entries
	OkcDataModifierClusterEntries* modifierCE = new OkcDataModifierClusterEntries(okcdata);
	modifierCE->initEntriesArr();
	mm->addOkcDataModifier(XmdvTool::MODIFIER_CLUSTERENTRIES, modifierCE);


	TraceGetUnbrushedClusters(okcdata, &this->nodes[0]);
	TraceGetBrushedClusters(okcdata, &this->nodes[0]);
	//printf("okcdata1.size = %d\n", okcdata->getDataSize());
}


void ClusterTree::TraceGetUnbrushedClusters( OkcData* okcdata, Cluster* node )
{
	if ( this->m_brushSemantic == SBBSEMANTIC_ALL ) //ALL
	{
		if ( BITTEST(node->flags,IS_BRUSHED) )
			return;
	}

	if ( BITTEST(node->flags,IS_CONTOUR0) )
	{
		if ( ! BITTEST(node->flags,IS_BRUSHED) )
			node->addIntoOkcDataAsTuple(okcdata, false);
		else
			return;
	}
	else
	{
		int j;
		for (j=0; j<node->num_children; j++)
			TraceGetUnbrushedClusters(okcdata, &this->nodes[node->child[j]]);
	}
}

void ClusterTree::TraceGetBrushedClusters( OkcData* okcdata, Cluster* node )
{
	if ( ! BITTEST(node->flags,IS_BRUSHED_ANY) )
		return;

	if ( BITTEST(node->flags,IS_CONTOUR1) )
	{
		if ( BITTEST(node->flags,IS_BRUSHED) )
			node->addIntoOkcDataAsTuple(okcdata, true);
		else
			return;
	}
	else
	{
		int j;
		for (j=0; j<node->num_children; j++)
			TraceGetBrushedClusters(okcdata, &this->nodes[node->child[j]]);
	}
}
