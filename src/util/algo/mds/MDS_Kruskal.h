#ifndef MDS_KRUSKAL
#define MDS_KRUSKAL

#include "util/algo/mds/MDS_misc.h"
#include "util/algo/mds/MDS_Embedding.h"
//#include <gu_link.h>


class Kruskal_node
{
public:
	int i, j;
	double dissim, disparity;	//estimate delta

	Kruskal_node();
	int operator > (const Kruskal_node &kn);
	int operator < (const Kruskal_node &kn);
	Kruskal_node& operator = (const Kruskal_node& );


	friend std::istream& operator>> (std::istream&, Kruskal_node& );
	friend std::ostream& operator<< (std::ostream&, Kruskal_node& );
};


class Kruskal_Array:public IP_Vector<Kruskal_node>
{
public:
	void sort_disparity();
};

class MDS_Kruskal:public MDS_Embedding
{
public:
	//Xik--estimated coordinates
	//Dij--distance
	//Dissim--orginal dissimilarity (defined in MDS_Embedding)
	//Disparity--disparity

	MDS_Matrix Dij, Xik, Disparity;
	double stress;
	Kruskal_Array krusk;

	MDS_Kruskal();
	void init();


	//Sum(i,j)dij^2=1.00
	//Xik=c*Xik
	void standardize();

	//convert to  a link list so that to sorting
	void initKruskList();

	//adjusting a estimated delta (i,j)
	void nonmetric();

	//convert from adjusted link list
	void fromKruskList();

	//compute Xik and Dik
	void metric();

	double s1();	//stress
	double s2();
	double ss1();	//s-stress
	double ss2();

	void printStress();

	//one iteration
	void iterateOnce();

	//multiple iteration until the desired stress is satisfied
	void kruskal_scaling(double str);
};


#endif
