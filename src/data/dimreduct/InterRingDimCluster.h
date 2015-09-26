/*
 * InterRingDimCluster.h
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#ifndef INTERRINGDIMCLUSTER_H_
#define INTERRINGDIMCLUSTER_H_

#include "data/dimreduct/InterRingCluster.h"
#include <vector>

class InterRingDimCluster : public InterRingCluster {
	enum REORDER_METHOD{
	        PCA_REORDER,
	        OPTIMAL_REORDER,
	        RANDOM_REORDER
	};
public:
	InterRingDimCluster();
	virtual ~InterRingDimCluster();

public:
    double weight; //its weight when calculate its parent dimension. it is meanless for root node;
	int okc_id; //dim id in okc and cg file
//    double disagreement;  //disagreement of this node;

    //start_jing_PCA
    //this first PCA component of the dimension cluster.
    double *PCA1;
    double PCA1_min, PCA1_max;
    bool PCAed;
    double PCA1_contribution; //contribution of this cluster to its parent's PCA1
    //end_jing_PCA


public:

	double calculate(std::vector<double> &all_data, std::vector<double> &dim_min, std::vector<double> &dim_max);
    int calculate(std::vector<int> &all_data);
    void calculateWithDimClusterExtends(std::vector<double> all_data, double &mean, double &max, double &min);

    void resetNames();

    //void changeSelectedStatus(bool mark);

    //start_jing_PCA
    //this first PCA component of the dimension cluster.
    //double* getPCA1();
    //bool PCA();
    //end_jing_PCA

    //set reorder_para according to pca contributions
    //bool setReorderParaAccordingToPCA();

    //bool setReorderPara(REORDER_METHOD choice);
    //bool setReorderPara();

    /////////choice: 1,optimal reorder; 2,random reorder; 3,artificial ants reorder////////////////////////////////////
    static bool similarity_reorder_interring(int, int, double **, std::vector<int> &, int, int, char *, int =-1);

protected:
    /////////choice: 1,optimal reorder; 2,random reorder; 3,artificial ants reorder////////////////////////////////////
    bool setReorderParaAccordingToChoice(int choice);

    void getClusterData(int& n, int& m, double **&data);
};

#endif /* INTERRINGDIMCLUSTER_H_ */
