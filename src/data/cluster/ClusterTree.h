/*
 * ClusterTree.h
 *
 *  Created on: Jun 4, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class ClusterTree denotes a cluster tree.
 * the data member nodes is an array containing
 * all nodes in this tree
 */
#ifndef CLUSTERTREE_H_
#define CLUSTERTREE_H_

#include <vector>
#include "data/Data.h"
#include "data/cluster/Cluster.h"


class OkcData;
class Cluster;
class HierDisplayInformation;
class TreeContour;

class ClusterTree : public Data {
public:
    ClusterTree();
    virtual ~ClusterTree();

private:
	int m_brushSemantic;

public:
	// The pointer to the OkcData to generate this cluster tree
	OkcData* m_inputOkcData;

    std::vector<Cluster> nodes;
    int dimension;
    int treemap_limit_number;

    std::vector<double> min_extents;
    std::vector<double> max_extents;

    // Read the cf or cg file and construct the cluster tree
    // It will check the first four bytes and decide the file format (cg or cf)
    // and then call function ReadTextClusterTreeFile()
    // or ReadTextClusterTreeFile() based on the file format.
    bool ReadClusterTreeFile(char* okcFileName, char* cgFileName, OkcData* dataset);

    // Read the cf file and construct the cluster tree
    bool ReadTextClusterTreeFile(const char* fname, OkcData* dataset);
    // Read the cg file and construct the cluster tree
    bool ReadBinaryClusterTreeFile(const char* fname, OkcData* dataset);

	void writeClusterTree(char * fileName);
	void writeClusterTreeBroad(char * fileName);


// static functions for convenience reason
//	static void hierDeleteClusterTree();
	// Initialize a cluster tree and return a pointer to this tree.
	// It will first try to read file and load cluster tree,
	// if failed, it will online generate one
	// Read the cluster tree from the cg or cf file
	// and initialize the cluster node
	// Parameters:
	//   okcFileName: the filename for input okcdata
	//   cgFileName: the filename for cg file which contains clustering result
	//   dataset: the input dataset
	//   hier_info: an pointer to a class HierDisplayInformation to describe hierarchical display parameters.
	//   tree_contour (OUT): This argument will include tree contour information after this function returns.
	static ClusterTree* hierInitClusterTree(char* okcFileName, char* cgFileName, OkcData* dataset,
			HierDisplayInformation* hier_info,
			TreeContour* tree_contour);
	// Initialize the cluster node color
	// This is a recursive function
	static void hierInitClusterColors(ClusterTree* btree, Cluster* node, double L, double R);
	static void hierClearNodeFlags(ClusterTree* btree, int bits=0xffffff);
	static void hierMarkContourNodes(ClusterTree* btree, double contour_radius, int FLAG);
	static void SetupBrushParameters(ClusterTree* btree, HierDisplayInformation* hier_info);
	static void hierInitExtents(ClusterTree* btree);
	static void hierMarkBrushedClustersByColor(ClusterTree* btree, Cluster* node, double L, double R);

	// Traverse the cluster tree and add the node on specified level into the okcdata
	void TraceGetUnbrushedClusters( OkcData* okcdata, Cluster* node );
	void TraceGetBrushedClusters( OkcData* okcdata, Cluster* node );

public:
	// Add the nodes at a specific level in this cluster tree into the OkcData
	void generateSBBResult(OkcData* okcdata);
};

#endif /* CLUSTERTREE_H_ */
