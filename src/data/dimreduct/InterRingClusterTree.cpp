/*
 * InterRingClusterTree.cpp
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#include "data/dimreduct/InterRingClusterTree.h"
#include "data/dimreduct/InterRingCluster.h"

InterRingClusterTree::InterRingClusterTree()
{
//	dim_cluster_file = NULL;
//	CMAP = new colormap("InterRing");
//	current_list = NULL;
    root = NULL;
}


InterRingClusterTree::~InterRingClusterTree()
{
	//delete CMAP;
    if ( root != NULL )
        delete root;
}

//construct the cluster tree for interRing display according to the arrays;
//parameters: int num_nodes-number of nodes.
//parameters: array nodes-nodes stored in an array according to their ids. nodes[0] must be the root node.
//parameters: array parent_ids-corresponding nodes' parent' ids. nodes[0](the root node)'s parent id is
//-1. Other parent ids should be larger than -1.
bool InterRingClusterTree::constructInterRingClusterTree(int num_nodes, std::vector<InterRingCluster *> nodes, std::vector<int> parent_ids)
{
	//if can't find the root node, return false
	if ( parent_ids[0] != -1 || nodes[0] == NULL)
		return false;

	//link the clusters into a tree
	root = nodes[0];
	for ( int i = 1; i < num_nodes; i++ )
	{
		if ( parent_ids[i] < 0 || parent_ids[i] >= num_nodes )
			return false;
		if ( nodes[parent_ids[i]] == NULL )
			return false;
		nodes[parent_ids[i]]->addChild(nodes[i]);
	}

	resetLevels();
	resetAngles();
	resetColors();
	return true;
}

//set level for every node in the tree, find the max_level of the tree
void InterRingClusterTree::resetLevels()
{
	max_level = 1;
	root->resetLevels(1, max_level);
}

void InterRingClusterTree::resetEntries()
{
    root->resetEntries();
}


//reset color of all nodes of the tree. color ~ mid angle of the node
void InterRingClusterTree::resetColors()
{
	root->resetColors();
}


//reorder
void InterRingClusterTree::reorder()
{
    if ( root->setReorderPara() )
	    root->reorder();
}


//set start_angles and sweep_angles for all the nodes
void InterRingClusterTree::resetAngles()
{
	unit_angle = 360. / root->entries;
	mini_angle = unit_angle / 25;
	root->resetAngles(0, unit_angle);
}


InterRingCluster* InterRingClusterTree::findNodeAccordingLevelAndAngle(int level, double angle)
{
	if ( level < root->level )
		return NULL;

	while ( angle < 0 )
		angle += 360;
	while ( angle > 360 )
		angle -= 360;

	return root->findNodeAccordingLevelAndAngle(level, angle);
}


InterRingCluster* InterRingClusterTree::findNodeAccordingLevelAndAngle(int level, double angle, bool &start, bool &end, bool &in)
{
	if ( level < root->level )
		return NULL;

	while ( angle < 0 )
		angle += 360;
	while ( angle > 360 )
		angle -= 360;

	start = false;
	end = false;
	in = false;

	return root->findNodeAccordingLevelAndAngle(level, angle, mini_angle/2, start, end, in);
}

/*
void InterRingClusterTree::resetCurrentList()
{
	if ( current_list != NULL )
		delete current_list;

	current_list = new CLinkList();
	root->ResetCurrentList(current_list);
}
*/
void InterRingClusterTree::resetSelection()
{
}


void InterRingClusterTree::rotate(double degree)
{
	root->rotate(degree);
}

//return double from 0 to 1;
double InterRingClusterTree::getDistance(InterRingCluster *node_1, InterRingCluster *node_2)
{
    InterRingCluster *node1, *node2;

    if ( node_1->level >= node_2->level )
    {
        node1 = node_1;
        node2 = node_2;
    }
    else
    {
        node1 = node_2;
        node2 = node_1;
    }

//
    double distort_para1 = node1->sweep_angle / node1->entries / unit_angle;
    double distort_para2 = node2->sweep_angle / node2->entries / unit_angle;

    double distort_para = distort_para1 > distort_para2 ? distort_para1 : distort_para2;
//

    int dis = node1->level - node2->level;

    for ( int i = 0; i < dis; i++ )
    {
        node1 = node1->parent;
    }

    while ( node1 != node2 )
    {
        node1 = node1->parent;
        node2 = node2->parent;
        dis++;
    }
    double distance = node1->radius * distort_para;
//    double distance = ((double)dis)/(max_level-1);
    return distance;
}

