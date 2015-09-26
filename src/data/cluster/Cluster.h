/*
 * Cluster.h
 *
 *  Created on: Jun 4, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class Cluster denotes a node in the cluster tree.
 * This cluster is ported from old XmdvTool
 */
#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <vector>

class OkcData;

class Cluster {
public:
	Cluster();
	virtual ~Cluster();

public:
    int nodeid;
    int parent;//Reference to the parent in the cluster tree
    int num_children;//stores the number of children
    std::vector<int> child;//Stores the index in the cluster tree for the child

    std::vector<double> mean;//Stores the mean of each of the dimensions
//jing
    std::vector<double> max;//Stores the max for each dimension
    std::vector<double> min;//Stores the min of each dimension
//jing
    std::vector<double> all_mean;
	char *name; //name of the cluster


    //okcdata::mark_type mark;

    // cfentry of a cluster
    int entries;
    std::vector<double> sx; //Stores the actual values of dimensions of the cluster
    double sxx;
    double sqrt_radius; //Stores the radius
    double sqrt_diameter;

    double color;

    // The dimensions for the rect in tree-map
    double xmin;
    double xmax;
    double ymin;
    double ymax;

    int flags;
    //jing_flat_sb_start
    //bool flat_brushed;
    //bool contain_flat_brushed;
    //jing_flat_sb_end

public:
	// add the node into the OkcData as a new tuple
	// This is used to generate the OkcData from
	// the node at a specific level-of-detail in the cluster tree
    void addIntoOkcDataAsTuple(OkcData* okcdata, bool brushed);

};

#endif /* CLUSTER_H_ */
