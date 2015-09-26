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

  //cout<<endl<<B<<endl<<endl;

  // Jacobi rotation:
  // Compute eigenvalues and eigenvectors of B.

  MDS_Matrix	V(N,N);
  MDS_Vector	lambda( N );
  int		rot;

  jacobid(  B.nr(), N, lambda.nr() , V.nr(), &rot );
  eigsrtd( lambda.nr() , V.nr(), N );

  // Take only the largest n eigenvalues
  // Weight the corresponding colums (coordinates
  // of the solution X) by lambda[i], i=1,...,n
  /*
  cout<<lambda<<endl;
  ofstream of;
  of.open ("embeddinglogfile.txt");
  if (of==NULL) {
	  cout<<"cann not open embedding log file"<<endl;
	  return ;
  }
  of<<lambda<<endl;
  of.close ();
  */
  //cout<<V<<endl;

  //cout<<V.window(0,0,N,n)<<endl;

  copy( V.window(0,0,N,n) );

  /*
  for( int c=0; c<n; c++ )
  {
    if( lambda[c]<0 )
      fatal( "Classical scaling: Eigenvector lambda[%d] = %g < 0.", c, lambda[c] );

    col(c) *= sqrt(lambda[c]);
  }

  if(verbose)
    info( "Finished classical scaling with sstress=%g.", D.stress(distEuclid()) );
*/
}

/*
void
MDS_Embedding::center()
{
  VectorIterator	X_column( *this, 0 );

  forall_iterator_items( X_column )
    *X_column -= X_column->mean();
}

//
// method	: jacobi()
// description	: Perform rotation into eigensystem of
//		  the covariance matrix.
// note		: Embedding has to be centered by center() before.
//

void
MDS_Embedding::jacobi()
{
  int d=M();

  int	       	rot;
  MDS_Matrix	V(d,d);
  MDS_Vector	lambda(d);
  MDS_Matrix	C(cov());

  jacobid( C.nr(), d, lambda.nr(), V.nr(), &rot );

  // rotate Embedding
  this->copy( (*this)*V );
}

MDS_Vector
MDS_Embedding::mean()
{
  MDS_Vector		result( len1 );

  VectorIterator	col( *this, 0 );

  forall_iterator_items( col )
    result[col.index()] = col->mean();

  return result;
}

MDS_Matrix
MDS_Embedding::cov( int i )
{
  MDS_Vector		m( mean() );

  VectorIterator	row( *this, 1 );
  MDS_Matrix		result( len1, len1 );

  result = 0.;
  forall_iterator_items( row )
  {
    if( row.index() != i )
    {
      MDS_Vector r( *row-m );
      result += r.dotTranspose(r);
    }
  }
  return result/len0;
}


MDS_Matrix
MDS_Embedding::distEuclid() const
{
  return row_p2norm_dist();
}

MDS_Matrix
MDS_Embedding::distSqrEucl() const
{
  return row_p2sqr_dist();
}
*/
