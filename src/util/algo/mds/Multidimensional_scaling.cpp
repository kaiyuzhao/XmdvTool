/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#include "util/algo/mds/Multidimensional_scaling.h"
#include "util/algo/mds/MDS_misc.h"
#include "util/algo/mds/MDS_Embedding.h"
#include "util/algo/mds/MDS_Kruskal.h"



bool multidimensional_scaling(int n,
							  int lower_dimensionality,
							  double **distance_matrix,
							  double** positions)
{
	MDS_Matrix dissim(n,n);
	for (int i=0; i<n; i++)
		for (int j=0; j<n; j++)
			dissim(i, j) = distance_matrix[i][j];


	MDS_Kruskal emd;
	if (lower_dimensionality>2) emd.setDimensionNumber(lower_dimensionality);
	emd.assignDissim (dissim);
	emd.classicalScaling (dissim);


	// Multidimensional scaling
	emd.init ();
	emd.iterateOnce ();

	//std::cout<<"Mulitdimensional scaling is in the progress..."<<std::endl;
	for (int i=0; i<100;i++) {
		double ss2=emd.ss2();

		//cout<<endl<<"ith time:"<<i<<endl;
		//cout<<"ss2="<<ss2<<endl;

		emd.iterateOnce ();
		if(fabs(ss2-emd.ss2())<0.001) break;
		//emd.printStress();
	}
	//std::cout<<"MDS done!"<<std::endl;


	for (int j=0; j<lower_dimensionality; j++) {

		double col_max =  emd.Xik.col(j).max();
		double col_min =  emd.Xik.col(j).min();
		double coeff =  1.0/(col_max-col_min);

		for (int i=0; i<n; i++)
			positions[j][i] = (emd.Xik.element(i, j) - col_min)*coeff;
	}

	return true;

}

/*
#ifdef __cplusplus
}
#endif
*/
