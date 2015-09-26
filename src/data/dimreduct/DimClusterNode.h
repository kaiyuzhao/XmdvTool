/*
 * DimClusterNode.h
 *
 *  Created on: Sep 27, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class DimClusterNode represents a node in the dimension cluster tree
 * when we online generate dimension cluster tree using the following function:
 * DimReducer::BuildDimClusterTree().
 * We use another class InterRingDimCluster to represent a node in the
 * dimension cluster tree after the dimension cluster tree has been created.
 * The latter class is used much more often than the former.
 */
#ifndef DIMCLUSTERNODE_H_
#define DIMCLUSTERNODE_H_

class LinkListNode;

class DimClusterNode {
public:
	DimClusterNode();
	virtual ~DimClusterNode();

public:
    int id;
    int count;
    int entries;
    int okc_id;
	double *data;
    double radius;
	char *name;
	LinkListNode *parent;
};

#endif /* DIMCLUSTERNODE_H_ */
