/*
 * PCA.cpp
 *
 *  Created on: Sep 3, 2009
 *      Ported by Zaixian Xie
 */

#include "util/algo/pca/PCA.h"

#include <math.h>

PCA::PCA() {
}

PCA::~PCA() {
}



/*************************************************************************************************/
//Principal component analysis -- derived

#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )

double **PCA::matrix(int n, int m)
{
	int i;
	double **mat;

	mat = new double *[n];

	for ( i = 0; i < n; i++ )
	{
		mat[i] = new double[m];
	}
	return mat;
}

void PCA::free_matrix(double** matrix, int n, int)
{
	for ( int i = 0; i < n; i++ )
	{
		delete [] matrix[i];
	}
	delete [] matrix;
}

double *PCA::vector(int n)
{
	double* v;
	v = new double[n];
	return v;
}

void PCA::free_vector(double *v, int)
{
	delete [] v;
}

//creat correlation matrix
void PCA::corcol(double **data, int n, int m, double **symmat)
{
	double eps = 0.005;
	double x, *mean, *stddev;
	int i, j, j1, j2;

	mean = vector(m);
	stddev = vector(m);

	// determine mean of column vectors of input data matrix
	for ( j = 0; j < m; j++ )
	{
		mean[j] = 0.0;
		for ( i = 0; i < n; i++ )
		{
			mean[j] += data[i][j];
		}
		mean[j] /= n;
	}

	//determine standard deviations of colunm vectors of data matrix
	for ( j = 0; j < m; j++ )
	{
		stddev[j] = 0.0;
		for ( i = 0; i < n; i++ )
		{
			stddev[j] += ( ( data[i][j] - mean[j] ) *
				           ( data[i][j] - mean[j] ) );
		}
		stddev[j] /= n;
		stddev[j] = sqrt(stddev[j]);
		/* the following is an inelegant but usual way to handle near-zero std. dev. values, which
		below would cause a zero-divide */
		if ( stddev[j] <= eps )
			stddev[j] = 1.0;
	}

	//center and reduce the column vectors
	for ( i = 0; i < n; i++ )
	{
		for ( j = 0; j < m; j++ )
		{
			data[i][j] -= mean[j];
			x = sqrt((double)n);
			x *= stddev[j];
			data[i][j] /= x;
		}
	}

	//calculate the m*m correlation matrix
	for ( j1 = 0; j1 < m-1; j1++ )
	{
		symmat[j1][j1] = 1.0;
		for ( j2 = j1+1; j2 < m; j2++ )
		{
			symmat[j1][j2] = 0.0;
			for ( i = 0; i < n; i++ )
			{
				symmat[j1][j2] += ( data[i][j1] * data[i][j2] );
			}
			symmat[j2][j1] = symmat[j1][j2];
		}
	}
	symmat[m-1][m-1] = 1.0;
	return;
}

//householder reduction of matrix a  to tridiagonal form
void PCA::tred2( double **a, int n, double *d, double *e)
{
	int  l, k, j, i;
	double scale, hh, h, g, f;

	for ( i = n-1; i >= 1; i-- )
	{
		l = i - 1;
		h = scale = 0.0;
		if ( l > 0 )
		{
			for ( k = 0; k <= l; k++ )
				scale += fabs(a[i][k]);
			if ( fabs(scale) < 1e-8 )
				e[i] = a[i][l];
			else
			{
				for ( k = 0; k <= l; k++ )
				{
					a[i][k] /= scale;
					h += a[i][k] * a[i][k];
				}
				f = a[i][l];
				g = f>0 ? -sqrt(h) : sqrt(h);
				e[i] = scale * g;
				h -= f * g;
				a[i][l] = f - g;
				f = 0.0;
				for ( j = 0; j <= l; j++ )
				{
					a[j][i] = a[i][j] / h;
					g = 0.0;
					for ( k = 0; k <= j; k++ )
						g += a[j][k] * a[i][k];
					for ( k = j+1; k <= l; k++ )
						g += a[k][j] * a[i][k];
					e[j] = g / h;
					f += e[j] * a[i][j];
				}
				hh = f / ( h + h );
				for ( j = 0; j <= l; j++ )
				{
					f = a[i][j];
					e[j] = g = e[j] - hh * f;
					for ( k = 0; k <= j; k++ )
						a[j][k] -= ( f * e[k] + g * a[i][k] );
				}
			}
		}
		else
			e[i] = a[i][l];
		d[i] = h;
	}

	d[0] = 0.0;
	e[0] = 0.0;
	for ( i = 0; i < n; i ++ )
	{
		l = i - 1;
		if ( d[i] )
		{
			for ( j = 0; j <= l; j++ )
			{
				g = 0.0;
				for ( k = 0; k <= l; k++ )
					g += a[i][k]  * a[k][j];
				for ( k = 0; k <= l; k++ )
					a[k][j] -= g * a[k][i];
			}
		}
		d[i] = a[i][i];
		a[i][i] = 1.0;
		for ( j = 0; j <= l; j++ )
			a[j][i] = a[i][j] = 0.0;
	}
}

// tridiagonal QL algorithm -- implicit
bool PCA::tqli(double *d, double *e, int n, double **z)
{
	int m, l, iter, i, k;
	double s, r, p, g, f, dd, c, b;

	for ( i = 1; i < n; i++ )
		e[i-1] = e[i];
	e[n-1] = 0.0;
	for ( l = 0; l < n; l++ )
	{
		iter = 0;
		do
		{
			for ( m = l; m < n-1; m++ )
			{
				dd = fabs(d[m]) + fabs(d[m+1]);
				if ( fabs(e[m]) < 1e-8 )
					break;
			}
			if (m != l )
			{
				if ( iter++ == 30 )
					return false;
				g = ( d[l+1] - d[l] ) / ( 2.0 * e[l] );
				r = sqrt( (g * g) + 1.0 );
				g = d[m] - d[l] + e[l] / ( g + SIGN(r, g) );
				s = c = 1.0;
				p = 0.0;
				for ( i = m-1; i >= l; i-- )
				{
					f = s * e[i];
					b = c * e[i];
					if ( fabs(f) >= fabs(g) )
					{
						c = g / f;
						r = sqrt( ( c * c ) + 1.0 );
						e[i+1] = f * r;
						c *= ( s = 1.0/r );
					}
					else
					{
						s = f / g;
						r = sqrt( (s * s) + 1.0 );
						e[i+1] = g * r;
						s *= ( c = 1.0/r );
					}
					g = d[i+1] - p;
					r = ( d[i] - g ) * s + 2.0 * c * b;
					p = s * r;
					d[i+1] = g + p;
					g = c * r - b;
					for ( k = 0; k < n; k++ )
					{
						f = z[k][i+1];
						z[k][i+1] = s * z[k][i] + c * f;
						z[k][i] = c * z[k][i] - s * f;
					}
				}
				d[l] = d[l] - p;
				e[l] = g;
				e[m] = 0.0;
			}
		}   while ( m != l );
	}
	return true;
}

bool PCA::principal_Components_Analysis(int n, int m, double **data, int prin_num, double **prin, double *prin_max, double *prin_min)
{
	int i, j, k, k2;
	double **symmat, **symmat2, *evals, *interm;

	symmat = matrix(m, m); //symmat: correlation matrix
	corcol(data, n, m, symmat);	 //creat correlation matrix

/**** eigen-reduction ***********/

	evals = vector(m);   //evals: vector of eigenvalues
	interm = vector(m);   //interm: vector of intermediate
	symmat2 = matrix(m, m);  //duplication of correlation matrix
	for ( i = 0; i < m; i++ )
		for ( j = 0; j < m; j++ )
			symmat2[i][j] = symmat[i][j];

	tred2(symmat, m, evals, interm); //triangular decomposition

	if ( ! tqli(evals, interm, m, symmat) )
	{
		free_matrix(symmat, m, m);
		free_matrix(symmat2, m, m);
		free_vector(evals, m);
		free_vector(interm, m);
		return false; //reduction of sym. trid. matrix
	}
/* evals now contains the eigenvalues, columns of symmat now contain the associated eigenvectors*/

	// Form projections of row-points on first three prin. components
	for ( i = 0; i < n; i++ )
	{
		for ( k = 0; k < prin_num; k++ )
		{
			prin[k][i] = 0.0;
			for ( k2 = 0; k2 < m; k2++ )
				prin[k][i] += data[i][k2] * symmat[k2][m-k-1];
		}
	}

	for ( i = 0; i < prin_num; i++ )
		prin_max[i] = prin_min[i] = prin[i][0];

	for ( i = 0; i < n; i++ )
	{
		for ( j = 0; j < prin_num; j++ )
		{
			if ( prin[j][i] > prin_max[j] )
				prin_max[j] = prin[j][i];
			if ( prin[j][i] < prin_min[j] )
				prin_min[j] = prin[j][i];
		}
	}

	free_matrix(symmat, m, m);
	free_matrix(symmat2, m, m);
	free_vector(evals, m);
	free_vector(interm, m);
	return true;
}

bool PCA::principal_Components_Analysis_2(int n, int m, double **data, int prin_num, double **prin, double *prin_max, double *prin_min, double *pca1_contributions)
{
	int i, j, k, k2;
	double **symmat, **symmat2, *evals, *interm;

	symmat = matrix(m, m); //symmat: correlation matrix
	corcol(data, n, m, symmat);	 //creat correlation matrix

/**** eigen-reduction ***********/

	evals = vector(m);   //evals: vector of eigenvalues
	interm = vector(m);   //interm: vector of intermediate
	symmat2 = matrix(m, m);  //duplication of correlation matrix
	for ( i = 0; i < m; i++ )
		for ( j = 0; j < m; j++ )
			symmat2[i][j] = symmat[i][j];

	tred2(symmat, m, evals, interm); //triangular decomposition

	if ( ! tqli(evals, interm, m, symmat) )
	{
		free_matrix(symmat, m, m);
		free_matrix(symmat2, m, m);
		free_vector(evals, m);
		free_vector(interm, m);
		return false; //reduction of sym. trid. matrix
	}
/* evals now contains the eigenvalues, columns of symmat now contain the associated eigenvectors*/

	// Form projections of row-points on first three prin. components
	for ( i = 0; i < n; i++ )
	{
		for ( k = 0; k < prin_num; k++ )
		{
			prin[k][i] = 0.0;
			for ( k2 = 0; k2 < m; k2++ )
				prin[k][i] += data[i][k2] * symmat[k2][m-k-1];
		}
	}

	for ( i = 0; i < prin_num; i++ )
		prin_max[i] = prin_min[i] = prin[i][0];

	for ( i = 0; i < n; i++ )
	{
		for ( j = 0; j < prin_num; j++ )
		{
			if ( prin[j][i] > prin_max[j] )
				prin_max[j] = prin[j][i];
			if ( prin[j][i] < prin_min[j] )
				prin_min[j] = prin[j][i];
		}
	}

	for ( k2 = 0; k2 < m; k2++ )
		pca1_contributions[k2] = symmat[k2][m-1];

    free_matrix(symmat, m, m);
	free_matrix(symmat2, m, m);
	free_vector(evals, m);
	free_vector(interm, m);
	return true;
}

