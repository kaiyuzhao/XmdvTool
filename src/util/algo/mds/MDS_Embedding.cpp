#include "util/algo/mds/MDS_Embedding.h"
#include "util/algo/mds/nrutil.h"



MDS_Embedding::MDS_Embedding( int N, int M ) :
  MDS_Matrix( N,M )
{}

MDS_Embedding::MDS_Embedding( const MDS_Matrix& other ) :
  MDS_Matrix( other )
{}

//
// method	: classicalScaling
// description	: Compute a classical metric MDS embedding
//		  given the _Euclidian_ distances D.


void
MDS_Embedding::classicalScaling( const MDS_Matrix& D )
{

  int N = D.length0();
  int n = length1();

  MDS_Matrix A,B,H(N,N);

  // Matrix A
  // Since we assume the D_{ik} now to be the
  // Euclidian distances, we have to square them.
  // a_{rs} = -1/2 D_{rs}*D_{rs}

  A = -0.5*D.pointMul(D);

  // Matrix H
  // This is not very effective, but
  // done for the ease of the implementation.

  H = -1./double(N);
  H.diagonal() = 1.-1./double(N);

  // Matrix B
  // Inner-product-matrix, B=XX^T

  B = H*A*H;

  // Jacobi rotation:
  // Compute eigenvalues and eigenvectors of B.

  MDS_Matrix	V(N,N);
  MDS_Vector	lambda( N );
  int		rot;

  jacobid(  B.nr(), N, lambda.nr() , V.nr(), &rot );
  eigsrtd( lambda.nr() , V.nr(), N );

  copy( V.window(0,0,N,n) );
}
