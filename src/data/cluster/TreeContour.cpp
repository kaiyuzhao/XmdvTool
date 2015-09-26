/*
 * TreeContour.cpp
 *
 *  Created on: Aug 16, 2009
 *      Ported by: Zaixian Xie
 */

#include "data/cluster/TreeContour.h"

#include <vector>
#include <assert.h>

#include "main/XmdvTool.h"
#include "data/cluster/ClusterTree.h"
#include "data/cluster/Cluster.h"
#include "data/cluster/HierHist.h"
#include "operator/sbb/HierDisplayInformation.h"

TreeContour::TreeContour() {
}

TreeContour::~TreeContour() {
}

// The main objective of this function is to find the tree contour
void TreeContour::cfNavInit(ClusterTree* btree, HierDisplayInformation* hier_info)
{
	int i,j;

	setClusterTree(btree);

	//Using this for testing purposes only
	hier_info->tree_height = 0;
	hier_info->root_radius = FindRootRadius();
	FindRadiusDistribution(this->contour_radius);

    // Clear contour array
    for (i=0;i<(int)this->tree_contour.size();i++) {
        this->tree_contour[i].clear();
    }

    this->tree_contour.clear();

    // Compute the contours using the average radii
    //
	this->tree_contour.resize(this->contour_radius.size());
	this->tree_color.resize(this->contour_radius.size());

    for (i=0;i<(int)this->tree_contour.size();i++) {
        this->tree_contour[i].resize(SBB_CONTOUR_RES);
        this->tree_color[i].resize(SBB_CONTOUR_RES);
        for (j=0;j<SBB_CONTOUR_RES;j++) {
            this->tree_contour[i][j] = 0.0;
            this->tree_color[i][j] = -1.0;
        }
	}


	FindTreeContour(this->tree_contour,
		this->tree_color,
		this->contour_radius);

		//Printing for debugging purposes.
		/*for(int i = 0;i<Cthis->contour_radius.size();i++)
		{
				debugArr(Cthis->tree_contour[i]);
				std::cout<<std::endl;
		}*/
		//This finds the tree height and stores it in heir info
		hier_info->tree_height = FindTreeHeight();

    //
    // Find the maximum depth of the contours, for display
    // purposes. The maximum depth may not be the maximum
    // height of the tree, because the contours are approximated.
    //
    this->max_display_height = 0;
    for (j=0;j<SBB_CONTOUR_RES;j++) {
        if (this->tree_contour[this->tree_contour.size()-1][j] > this->max_display_height)
            this->max_display_height = (int) ceil(this->tree_contour[this->tree_contour.size()-1][j]);
    }

}

void TreeContour::setClusterTree(ClusterTree* btree) {
	m_btree = btree;
}

///
/// A function that finds the number of levels in the cluster tree
///
/// @return height of the tree
///
int TreeContour ::  FindTreeHeight()
{
	int heightOfTheTree = 0;

	this->FindTreeHeight(&m_btree->nodes[0],0,&heightOfTheTree);

	return (heightOfTheTree);
}

///
/// A function that evaluates the level values and populates the array
/// contour radius
///
/// @param countour_radius
///
void TreeContour :: FindRadiusDistribution(std::vector<double> &contour_radius)
{
	assert(HierHist::histo.size()!=0);

    int MIN_GAP = 5;

    double R = m_btree->nodes[0].sqrt_radius;

    contour_radius.resize(0);

    int i, previ;
    for (i=HierHist::histo.size()-1, previ=1000000;i>=0;i--)
    {
        if (HierHist::histo[i]==0) continue;
        if ((previ-i)<MIN_GAP) continue;

        contour_radius.push_back( R*i/double(HierHist::histo.size()-1) );
        previ = i;
    }
    contour_radius.push_back(0.0);
}

///
/// A function that finds the tree height. This is a recursive function
/// that traverses the cluster_tree to the deepest level to find the
/// height of the cluster tree.
///
void TreeContour :: FindTreeContour(std::vector< std::vector<double> > &tree_contour,
										 std::vector< std::vector<double> > &tree_color,
										 std::vector<double> contour_radius)
{
	int i;
    for (i=0;i<(int)tree_contour.size();i++) {
        FindTreeContour(&m_btree->nodes[0],
						0,
						contour_radius[i],
						tree_contour[i],
						tree_color[i],
						0,
						1);
    }

}

///
/// A function that returns the and evaluates the tree contour. The left
/// and the right extent indicates what elements of the array are to be set
/// by the current call
///
/// @param node, a pointer to the root node
/// @param curr_ht, gives the current level of depth
/// @param rad, gives the level value that we are looking for using recursion
/// @param contour, is one dimensional array that stores the value of the evaluated contour
/// @param L, is the left extent
/// @param R extent Right extent
///
void TreeContour :: FindTreeContour(Cluster *node,
					 int curr_ht,
					 double rad,
					 std::vector<double> &contour,
					 std::vector<double> &color,
					 double L,
					 double R)
{
    if (node->parent != -1)
    {
        Cluster *parent = &m_btree->nodes[node->parent];

        //
        // If it's a clustered node and a leaf
        //
        if (node->num_children==0 && node->sqrt_radius >= rad) {
            rad = node->sqrt_radius+ 0.01; // (1.0/(double)SBB_CONTOUR_RES);
        }

        if (node->sqrt_radius < rad)
        {
            //
            // Iso-radius contour cuts between node and parent.
            // "interp" is the intersection ratio between node and parent.
            // "y" is the intersection height of the contour.
            //

            double interp;

            if (parent->sqrt_radius - node->sqrt_radius < 0.0000001) interp = 0;
            else
                interp = (rad - node->sqrt_radius) /
                           (parent->sqrt_radius - node->sqrt_radius);

            double y = curr_ht - interp;

            //
            // Find the contour segment(s) this node covers: [x0,x1]
            //
            double x0 = L*SBB_CONTOUR_RES;
            double x1 = R*SBB_CONTOUR_RES;

            int xi0 = (int) ceil(x0+.000000001);
            int xi1 = (int) floor(x1-.00000001);

            for (int i=xi0;i<xi1;i++) {
                // Coverage that fills whole segments
                assert(contour[i] == 0.0);
                contour[i] = y;
                color[i] = (L+R)*.5;
            }

            if (xi0>0 && x1>xi0) {
                // Left end of the coverage
                double coverage = (xi0-x0);
                contour[xi0-1] += coverage*y;
                if (coverage>.9) color[xi0-1] = (L+R)*.5;
            }
            if (xi1<SBB_CONTOUR_RES && x0<xi1) {
                // Right end of the coverage
                double coverage = (x1-xi1);
                contour[xi1] += coverage*y;
                if (coverage>.9) color[xi1] = (L+R)*.5;
            }
            if (xi0>=x1 && xi1<=x0) {
                // Partial coverage
                double coverage = (x1-x0);
                contour[xi1] += coverage*y;
                if (coverage>.9) color[xi1] = (L+R)*.5;
            }

            return;
        }
    }

    double k = (R-L)/(double)(node->num_children);

    for (int i=0; i<node->num_children; i++) {
        FindTreeContour(&m_btree->nodes[node->child[i]],
						curr_ht+1,rad,contour, color, L+i*k, L+(i+1)*k);
    } // for (int i=0; i<node->num_children; i++)
}

///
/// A function that finds the tree height
/// This is a recursive function that traverses the cluster_tree to
/// to the deepest level to find the height of the cluster tree.
///
void TreeContour :: FindTreeHeight(Cluster* node,
										int curr_ht,
										int *max_ht)
{
    if (curr_ht>*max_ht) *max_ht = curr_ht;

    for (int i=0; i<node->num_children; i++)
        FindTreeHeight(&m_btree->nodes[node->child[i]], curr_ht+1, max_ht);
}

/*
 *	Returns the radius of the root node.
 */
double TreeContour::FindRootRadius()
{
	return(m_btree->nodes[0].sqrt_radius);
}

