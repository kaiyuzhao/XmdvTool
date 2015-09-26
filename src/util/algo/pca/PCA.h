/*
 * PCA.h
 *
 *  Created on: Sep 3, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class PCA contains static functions to do principal component analysis.
 * The entry for this class is the function
 * principal_Components_Analysis()
 * and principal_Components_Analysis_2();
 */
#ifndef PCA_H_
#define PCA_H_

class PCA {
public:
	PCA();
	virtual ~PCA();

	static double **matrix(int n, int m);
	static void free_matrix(double** matrix, int n, int m);
	static double *vector(int n);
	static void free_vector(double *v, int n);
	//creat correlation matrix
	static void corcol(double **data, int n, int m, double **symmat);
	//householder reduction of matrix a  to tridiagonal form
	static void tred2( double **a, int n, double *d, double *e);
	// tridiagonal QL algorithm -- implicit
	static bool tqli(double *d, double *e, int n, double **z);

	// The entrance for the PCA algorithm
	// Para:
	//   n : (in) the number of data points;
	//   m : (in) the number of dimensions;
	//   data : (in) a 2d array to store the data, data[i] stores the data point i;
	//   prin_num: (in) the number of principal components

	//   prin : (out) the computation result will go to this 2d array.
	//          prin[i] is the component i
	//   prin_max : (out) the maximum value for each component
	//   prin_min : (out) the minimum value for each component
	static bool principal_Components_Analysis(int n, int m, double **data, int prin_num, double **prin, double *prin_max, double *prin_min);
	static bool principal_Components_Analysis_2(int n, int m, double **data, int prin_num, double **prin, double *prin_max, double *prin_min, double *pca1_contributions);

};

#endif /* PCA_H_ */
