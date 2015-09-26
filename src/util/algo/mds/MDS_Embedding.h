#ifndef MDS_Embedding_HH
#define MDS_Embedding_HH

#include "util/algo/mds/MDS_misc.h"



class MDS_Embedding : public MDS_Matrix
{
public:

	MDS_Matrix Dissim;

	/* shiping, default dimension number is set to 2 */
  MDS_Embedding( int N=0, int M=2 );
  MDS_Embedding( const MDS_Matrix& other );

  //set dissimilarity matrix
  inline void assignDissim (const MDS_Matrix& DD) { Dissim=DD;}

  //set number of dimensions
  inline void setDimensionNumber (int dim_num) { setSize(0, dim_num); }

  // classical metric multidimensional scaling
  // given the _squared_ Euclidian distances DD
  void			classicalScaling( const MDS_Matrix& DD );

/*
  // center embedding
  void			center( void );

  // rotate into eigensystem
  void			jacobi( void );

  // mean of all vectors
  MDS_Vector		mean( void );

  // cov matrix
  MDS_Matrix		cov( int i=-1 );

  // number of objects
  inline unsigned int   N() const { return (unsigned int) length0(); }

  // embedding dimension
  inline unsigned int	M() const { return (unsigned int) length1(); }

  // matrix of squared Euclidian distances
  MDS_Matrix		distEuclid() const;

  // matrix of squared Euclidian distances
  MDS_Matrix		distSqrEucl() const;
*/
};

#endif

