#ifndef IP_MATRIX_HH
#define IP_MATRIX_HH

#include <iostream>
#include "util/algo/mds/IP_Vector.h"

#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a>b)?b:a)
#define ABS(a) ((a>=0)?a:-a)


struct IP_MatrixIndex
{
  int row;
  int col;
  friend std::ostream& operator <<(std::ostream& os, const IP_MatrixIndex& im)
	{  os<<im.row<<im.col<<std::endl;
		return os;
  }
  int indexDistSqr(int n, int m)
  {
	  return (row-n)*(row-n)+(col-m)*(col-m);
  }
};

template <class T>
class IP_Matrix : public IP_Array<T>
{
public:
  // dimension 0 indexes rows, dimension 1 indexes columns
  // => length of column == length of dim 0
  // => length of row    == length of dim 1

  // ========================
  // construction/destruction
  // ========================

  // standard constructor (with optional skips)
  IP_Matrix( int l0=0, int l1=0, int s0=0, int s1=1 );

  // constructor with reference (copy=false) and copy (copy=true) semantic
  IP_Matrix( const IP_Matrix<T>& other );
  IP_Matrix( const IP_Matrix<T>& other, bool copy );

  // constructor with memory allocated elsewhere in IP class
  IP_Matrix( const IP_Array<T>& parent, T* mem, int l0, int l1, int s0=0, int s1=1 );

  // constructor with memory allocated elsewhere in the world
  IP_Matrix( T* mem, int l0, int l1, int s0=0, int s1=1, bool handleMem=false );

  // destructor
  ~IP_Matrix();


  // ===========
  // matrix size
  // ===========

  // length (of array) and skip (offset between elements)
  int			len0, len1;
  int			skip0, skip1;

  // methods to set and get array length (without affecting memory size!)
  inline int	       	length0() const;
  inline int	       	length0( int l0 );

  inline int	       	length1() const;
  inline int	       	length1( int l1 );

  // overall length
  inline int		length() const;

  // get length of dimension d
  inline int	       	length ( int d ) const;

  // get skip of dimension d
  inline int	       	skip ( int d ) const;


  // ==============
  // element access
  // ==============

  // access pointer to (i,j)-th element
  inline T*		elementPtr( int i, int j );
  inline const T*	elementPtr( int i, int j ) const;

  // access (i,j)-th element
  inline T&		element( int i, int j );
  inline const T&	element( int i, int j ) const;

  // access (i,j)-th element
  inline T&          	operator() ( int i, int j );
  inline const T&  	operator() ( int i, int j ) const;

  // access first element i-th row
  inline T&		operator() ( int i );

  // internal: compute offset of element (i,j)
  inline int		seek( int i, int j ) const;

  // ================
  // access utilities
  // ================

  // note: expensive - avoid usage in inner loops

  // returns the offset-th diagonal of the matrix as a vector
  IP_Vector<T>		diagonal( int offset=0 );
  const IP_Vector<T>   	diagonal( int offset=0 ) const;

  // returns the i-th row as a vector; 0<=i<len0
  IP_Vector<T>         	row( int i );
  const IP_Vector<T>   	row( int i ) const ;

  // returns the j-th column as a vector; 0<=j<len1()
  IP_Vector<T>         	col( int j );
  const IP_Vector<T>   	col( int j ) const ;

  // returns sub-matrix
  // l0==0 || l1==0  =>  automatic calculation of resp. length
  // l0<0  || l1<0   =>  reflection of window orientation (FIXME)
  IP_Matrix		window    ( int b0, int b1, int l0, int l1 );


  // ================
  // copy, assignment
  // ================

  // initialize with memory allocated elsewhere
  IP_Matrix&		init ( T* mem, int l0,   int l1,
			       int s0=0, int s1=1, bool handleMem=false );

  // reference semantic - assign this and other
  IP_Matrix&	       	assign ( const IP_Matrix<T>& other );

  // copy semantic - copy other to this
  IP_Matrix&	       	copy ( const IP_Matrix<T>& other );

  // copy semantic - copy other to this
  inline IP_Matrix&     operator= ( const IP_Matrix<T>& other );

  // set all elements to value
  IP_Matrix<T>&	       	set2( T value );
  IP_Matrix<T>&       	operator=  ( T value );

  // ======
  // sizing
  // ======

  // simple resize
  IP_Matrix&		setSize   ( int l0, int l1 );

  // fit to other
  IP_Matrix&		setSize   ( const IP_Matrix& other );

  // change size without allocating new memory (make it smaller)
  IP_Matrix&		changeSize( int l0, int l1 );

  // setSize with copy of old matrix contents
  IP_Matrix<T>&		setSizeWithCopy( int l0, int l1, T init_value );

  // get and set array format
//  IP_ArrayFormat	format() const;
 // IP_Matrix&		setFormat ( IP_ArrayFormat format );


  // ========================
  // methods without argument
  // ========================

  // abs all elements
  IP_Matrix&		abs () ;

  // trace == sum of diagonal elements
  T			trace () const;

  // upper triangle = lower triangle
  IP_Matrix<T>&	       	mirror();

  // return transposed matrix pointing to same memory
  // no copy involved - exchanges changes skip and length
  IP_Matrix<T>	       	transposed();
  const IP_Matrix<T>  	transposed() const ;

  // Returns square matrix of inter-row-vector distances
  // ( D^2_{ik} = || row[i] - row[k] ||^2 ).
  IP_Matrix<T>  	row_p2sqr_dist() const;
  //IP_Matrix<T>  	row_p2norm_dist() const;

  // Set to unit matrix.
  //IP_Matrix<T>&	       	unit();

  // index pair of minimum/maximum
  IP_MatrixIndex      	argMax () const;
  IP_MatrixIndex      	argMin () const;

  // index pair of minimum distance
  IP_MatrixIndex		minDist(const T& var) const;


  // maximum/minimum element
  T&			max ();
  T			max () const;
  T&			min ();
  T			min () const;

  // Returns the matrix of sign-inverted elements.
  IP_Matrix		operator - () const;
  IP_Matrix<T>	row_center () const;
  IP_Matrix<T>	col_center () const;

  // ============================
  // methods with scalar argument
  // ============================

  IP_Matrix<T>&        	operator *= ( T value );
  IP_Matrix<T>&       	operator /= ( T value );
  IP_Matrix<T>&       	operator += ( T value );
  IP_Matrix<T>&        	operator -= ( T value );

  friend IP_Matrix<T>   operator * ( T value, const IP_Matrix<T>& mat )
  {
	  IP_Matrix<T>	result( mat.len0, mat.len1 );

	  register const T *tv0	= mat.v;
	  register T *rv0	= result.v;

	  if( mat.skip0==mat.len1 && result.skip0==result.len1 )
	  {
		  register int tl 	= mat.len0*mat.len1;
		  while( tl-- )
		  {
			  *rv0=value* *tv0;
			  rv0++; tv0++;
		  }
	  }
	  else
	  {
		  register const T *tv1;
		  register T *rv1;

		  register int tl1 = mat.len1;
		  register int ts1 = mat.skip1;
		  register int rs1 = result.skip1;

		  register int tl0 = mat.len0;
		  register int ts0 = mat.skip0;
		  register int rs0 = result.skip0;

		  while( tl0-- )
		  {
			  tv1 = tv0;
			  rv1 = rv0;
			  tl1 = mat.len1;
			  while( tl1-- )
			  {
				  *rv1=value* *tv1;
				  tv1 += ts1;
				  rv1 += rs1;
			  }
			  tv0 += ts0;
			  rv0 += rs0;
		  }
	  }
	  return result;
  }

  //friend IP_Matrix<T>   operator + ( T value, const IP_Matrix<T>& mat );
  //friend IP_Matrix<T>   operator - ( T value, const IP_Matrix<T>& mat );

  IP_Matrix<T>       	operator * ( T value );
  IP_Matrix<T>       	operator / ( T value );
  IP_Matrix<T>       	operator + ( T value );
  IP_Matrix<T>       	operator - ( T value );

  IP_Matrix<T>		mul( double value ) const;
  IP_Matrix<T>&         mulAssign( double value ) ;

  // Fill with elements drawn from identical distribution
  // on interval [lower_bound,upper_bound].
  //IP_Matrix<T>&        	random   ( T lower_bound, T upper_bound );
  //IP_Matrix<T>&        	addRandom( T lower_bound, T upper_bound );


  // ==============================
  // operators with vector argument
  // ==============================

//  IP_Vector<T>	       	operator* ( const IP_Vector<T>&this->v ) const;


  // ==============================
  // operators with matrix argument
  // ==============================

  IP_Matrix<T>&         multAssign( const IP_Matrix<T>& other,
				    IP_Matrix<T>& result ) const;
  IP_Matrix<T>	       	operator* ( const IP_Matrix<T>& other ) const;
  IP_Matrix<T>	       	operator/ ( const IP_Matrix<T>& other ) const;

  IP_Matrix<T>	       	operator+ ( const IP_Matrix<T>& other ) const;
  IP_Matrix<T>	       	operator- ( const IP_Matrix<T>& other ) const;

  IP_Matrix<T>&	       	operator/=( const IP_Matrix<T>& other );
  IP_Matrix<T>&	       	operator+=( const IP_Matrix<T>& other );
  IP_Matrix<T>&	       	operator-=( const IP_Matrix<T>& other );

  // scalar product
//  T		       	dot( const IP_Matrix<T>& other ) const;

  // element-wise multiplication of this and other
  // element-wise multiplication of this and other and assignment to result
  IP_Matrix<T>	       	pointMul( const IP_Matrix<T>& other ) const;
  IP_Matrix<T>&	       	pointMulAssign( const IP_Matrix<T>& other,
				        IP_Matrix<T>& result );
  // element-wise division of this by other
  // element-wise division of this by other and assignment to result
  IP_Matrix<T>	       	pointDiv( const IP_Matrix<T>& other );
  IP_Matrix<T>&	       	pointDivAssign( const IP_Matrix<T>& other,
				        IP_Matrix<T>& result );

  // compute determinant (only for 2x2 matrices)
  inline T              det();
  // compute inverse     (only for 2x2 matrices)
  inline void           inv(IP_Matrix<T>& result);

  // =====
  // norms
  // =====

  // y = abs(x(i,j))
  double		p1norm() const;

  // y = sqrt(x(i,j)*x(i,j))
  double		p2norm() const;

  // y = x(i,j)*x(i,j)
  double		p2sqr() const;

  // y = x(i,j)
  T			sum() const;

  // y = sum()/(len0*len1())
  T			mean() const;

  // y = p2sqr()/(len0*len1())
//  double		meanSqr(bool=true) const;

  // y = sqrt( p2sqr()/(len0*len1()) )
  double		rms() const;

  // =======
  // metrics
  // =======

  // sum of abs of element differences
  //double       		p1dist    ( const IP_Matrix<T>& other ) const;

  // _squared_ Euclidian distance of matrix elements
  //double       		p2sqr_dist( const IP_Matrix<T>& other ) const;

  // ===============================================================
  // Numerical recipes interface - nr() returns
  // an IP_RecipesMatrix<T> matrix that
  // converts itself into a NR compatible T**
  // (array of arrays each starting at position 1 instead of 0).
  // ===============================================================
  //IP_RecipesMatrix<T>   nr();
  T** nr();



//distance matrix functions, all retured matrix are square
	//Euculidian distance D^2{n, n}=x(i,j)*x(i,k)
	//IP_Matrix<T>		euculidian() const;
	//IP_Matrix<T>		dominance() const;
	//IP_Matrix<T>		cityBlock() const;
	//IP_Matrix<T>		minkowski() const;


//matrix variant
	//IP_Matrix<T>		square() const;

//minimum/maximum values for the rows
  IP_Vector<T> minrow() const;
  IP_Vector<T> maxrow() const;


//iostream overloading
	friend std::istream& operator >> (std::istream& is, IP_Matrix<T>& mat)
	{
		for (int i=0; i<mat.len0; i++)
			for (int j=0; j<mat.len1; j++)
				is>>mat.element(i,j);
		return is;
	}
	friend std::ostream& operator << (std::ostream& os, const IP_Matrix<T>& mat)
	{
		char tbl='\t';
		for (int i=0; i<mat.len0; i++) {
			//for (int j=0; j<mat.len1; j++)
			//os<<mat.element(i,j)<<" ";
			//os<<endl;
			os<<mat.row(i)<<std::endl;
		}

		return os;
	}

};



template <class FROM,class TO>
void convert( const IP_Matrix<FROM>& from, IP_Matrix<TO>& to );

template <class T, class T1>
IP_Matrix<T> right_merge(const IP_Matrix<T>& mtx, IP_Matrix<T1>& other);


template <class FROM,class TO>
inline
void convert( const IP_Matrix<FROM>& from, IP_Matrix<TO>& to )
{
	;
}


template <class T, class T1>
inline
IP_Matrix<T> right_merge(const IP_Matrix<T>& mtx, IP_Matrix<T1>& other)
{
	IP_Matrix<T> result(mtx.len0, mtx.len1+other.len1);

	register int tl1;
    register T* tv1;
    register int ts1   	 = mtx.skip1;
    register T* ov1;
    register T1* ov2;
    register int os1   	 = other.skip1;

    register int tl0	 = result.len0;
    register T* tv0		 = result.v;
    register int ts0   	 = result.skip0;
	register T* mv0		 = mtx.v;
    register T1* ov0	 = other.v;
	register int ms0	 = mtx.skip0;
    register int os0   	 = other.skip0;

    while( tl0-- )
    {
	  tv1 = tv0;

      tl1 = mtx.len1;
      ov1 = mv0;
      while( tl1-- )
      {
		*tv1 = *ov1;
		tv1 += ts1;
		ov1 += ts1;
      }

	  tl1 = other.len1;
      ov2 = ov0;
      while( tl1-- )
      {
		*tv1 =(T) *ov2;
		tv1 += ts1;
		ov2 += os1;
      }


      tv0 += ts0;
	  mv0 += ms0;
      ov0 += os0;
	}

	return result;
}



// ==================================
// implementation of inline functions
// ==================================

template <class T>
inline int
IP_Matrix<T>::length0() const
{ return len0; }

template <class T>
inline int
IP_Matrix<T>::length1() const
{ return len1; }

template <class T>
inline int
IP_Matrix<T>::length0( int l0 )
{ return len0=l0; }

template <class T>
inline int
IP_Matrix<T>::length1( int l1 )
{ return len1=l1; }

template <class T>
inline int
IP_Matrix<T>::length() const
{ return len0*len1; }

template <class T>
inline int
IP_Matrix<T>::length( int dim ) const
{ return (&len0)[dim]; }

template <class T>
inline int
IP_Matrix<T>::skip( int dim ) const
{ return (&skip0)[dim]; }

template <class T>
inline int
IP_Matrix<T>::seek( int i, int j ) const
{

  return i*skip0 + j*skip1;
}

template <class T>
inline T*
IP_Matrix<T>::elementPtr( int i, int j )
{
return this->v +seek(i,j); }

template <class T>
inline const T*
IP_Matrix<T>::elementPtr( int i, int j ) const
{ return this->v +seek(i,j); }

template <class T>
inline T&
IP_Matrix<T>::element( int i, int j )
{ return (this->v)[seek(i,j)]; }

template <class T>
inline const T&
IP_Matrix<T>::element( int i, int j ) const
{return (this->v)[seek(i,j)]; }

template <class T>
inline T&
IP_Matrix<T>::operator() ( int i )
{ return (this->v)[i]; }

template <class T>
inline T&
IP_Matrix<T>::operator() ( int i, int j )
{ return (this->v)[seek(i,j)]; }

template <class T>
inline const T&
IP_Matrix<T>::operator() ( int i, int j ) const
{ return (this->v)[seek(i,j)]; }

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::operator=( const IP_Matrix<T>& other )
{ return copy(other); }


// doesnt really work (or make much sense) for T != real, float
template <class T>
inline T
IP_Matrix<T>::det()
{

  T result = element(0,0)*element(1,1) - element(1,0)*element(0,1);
  return result;
}

// doesnt really work (or make much sense) for T != real, float
template <class T>
inline void
IP_Matrix<T>::inv(IP_Matrix<T>& result)
{


  T determ = det();

  if (determ == (T)(0.))
    {
       //warning("IP_Matrix::inv(): det == 0");
       //cout << "IP_Matrix::inv(): det == 0";
       // HACK
       determ = (T)(1.e-10);
    }

  result(0,0) = (T)( element(1,1) / determ);
  result(0,1) = (T)(-element(0,1) / determ);
  result(1,0) = (T)(-element(1,0) / determ);
  result(1,1) = (T)( element(0,0) / determ);
}



//#include <iostream.h>
//#include <ctype.h>


template <class T>
inline IP_Matrix<T>::IP_Matrix( int l0, int l1, int s0, int s1 ) :
  IP_Array<T>( l0*l1 ),
  len0( l0 ), len1( l1 ),
  skip0( (s0>0) ? s0 : l1 ),
  skip1( (s1>0) ? s1 :  1 )
{}

template <class T>
inline IP_Matrix<T>::IP_Matrix( const IP_Matrix<T>& other ) :
  IP_Array<T>( other ),
  len0( other.len0 ), len1( other.len1 ),
  skip0( other.skip0 ), skip1( other.skip1 )
{}

template <class T>
inline IP_Matrix<T>::IP_Matrix( const IP_Matrix<T>& other, bool copy_other ) :
  IP_Array<T>( other.length() ),
  len0( other.len0 ), len1( other.len1 ),
  skip0( len1 ), skip1( 1 )
{
  if(copy_other) copy(other);
}

template <class T>
inline IP_Matrix<T>::IP_Matrix( const IP_Array<T>& parent, T* mem,
			 int l0, int l1, int s0, int s1 ) :
  IP_Array<T>(parent,mem),
  len0( l0 ),
  len1( l1 ),
  skip0( (s0!=0) ? s0 : l1 ),
  skip1( (s1!=0) ? s1 : 1  )
{}

template <class T>
inline IP_Matrix<T>::IP_Matrix( T* mem, int l0, int l1, int s0, int s1,
			 bool handleMem )
{
  init(mem, l0, l1, s0, s1, handleMem);
}

template <class T>
inline IP_Matrix<T>::~IP_Matrix()
{ }

template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::transposed()
{
  IP_Matrix<T> result( *this, this->v, len1, len0, skip1, skip0 );
  return result;
}

template <class T>
inline const IP_Matrix<T>
IP_Matrix<T>::transposed() const
{
  const IP_Matrix<T> result( *this, this->v, len1, len0, skip1, skip0 );
  return result;
}

template <class T>
inline IP_Vector<T>
IP_Matrix<T>::row( int i )
{



  IP_Vector<T> result( *this, this->v+i*skip0, len1, skip1 );
  return result;
}

template <class T>
inline const IP_Vector<T>
IP_Matrix<T>::row( int i ) const
{



  const IP_Vector<T> result( *this, this->v+i*skip0, len1, skip1 );
  return result;
}

template <class T>
inline IP_Vector<T>
IP_Matrix<T>::col( int j )
{


  IP_Vector<T> result( *this, this->v+j*skip1, len0, skip0 );
  return result;
}

template <class T>
inline const IP_Vector<T>
IP_Matrix<T>::col( int j ) const
{


  const IP_Vector<T> result( *this, this->v+j*skip1, len0, skip0 );
  return result;
}

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::set2( T value )
{
  operator=(value);
  return *this;
}

template <class T>
inline IP_Vector<T>
IP_Matrix<T>::diagonal( int offset )
{
  IP_Vector<T> result( *this, this->v+seek(MAX(0,-offset),MAX(0,offset)),
		       (offset>0) ? MIN(len0,len1-offset) : MIN(len1,len0+offset),
                       skip0+skip1 );



  return result;
}

template <class T>
inline const IP_Vector<T>
IP_Matrix<T>::diagonal( int offset ) const
{
  IP_Vector<T> result( *this, this->v+seek(MAX(0,-offset),MAX(0,offset)),
		       (offset>0) ? MIN(len0,len1-offset) : MIN(len1,len0+offset),
                       skip0+skip1 );



  return result;
}


// ================
// copy, assignment
// ================

template<class T>
inline IP_Matrix<T>&
IP_Matrix<T>::init( T* mem, int l0, int l1, int s0, int s1, bool handleMem )
{
  IP_Array<T>::init( mem, handleMem );

  len0 = l0;
  len1 = l1;

  skip0 = s0 ? s0 : l1;
  skip1 = s1 ? s1 : 1;

  return *this;
}


template<class T>
inline IP_Matrix<T>&
IP_Matrix<T>::assign( const IP_Matrix<T>& other )
{
  IP_Array<T>::assign(other);

  len0 = other.len0;
  len1 = other.len1;
  skip0 = other.skip0;
  skip1 = other.skip1;

  return *this;
}

template<class T>
inline IP_Matrix<T>&
IP_Matrix<T>::copy( const IP_Matrix<T>& other )
{
//  if( !len0 && !len1 )
	if( !len0 || !len1 )
  { setSize( other ); }
  else
  {

	; }

  if( this->v == other.v ) return *this;


  if( skip0==other.skip0 && skip1==other.skip1 && (skip0==len1||skip1==len0) )
    memcpy( this->v, other.v, len0*len1*sizeof(T) );
  else

  {
    register int tl1;
    register T* tv1;
    register int ts1   	 = skip1;
    register T* ov1;
    register int os1   	 = other.skip1;

    register int tl0	 = len0;
    register T* tv0 = this->v;
    register int ts0   	 = skip0;
    register T* ov0 = other.v;
    register int os0   	 = other.skip0;

    while( tl0-- )
    {
      tl1 = len1;
      tv1 = tv0;
      ov1 = ov0;
      while( tl1-- )
      {
	*tv1 = *ov1;
	tv1 += ts1;
	ov1 += os1;
      }
      tv0 += ts0;
      ov0 += os0;
    }
  }
  return *this;
}

// ======
// sizing
// ======

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::setSize( int l0, int l1 )
{
  this->size( l0*l1 );

  len0 	= l0;
  len1 	= skip0 = l1;
  skip1 = 1;

  return *this;
}

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::setSize( const IP_Matrix<T>& other )
{
  size( (len0=other.len0)*(len1=other.len1) );

  skip0 = len1;
  skip1 = 1;

  return *this;
}

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::changeSize( int l0, int l1 )
{
  len0 	= l0;
  len1 	= l1;
  return *this;
}

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::setSizeWithCopy( int newLen0, int newLen1, T init )
{
  IP_ArrayHandle<T>* nref = new IP_ArrayHandle<T>( newLen0*newLen1 );
  T* nv = nref->mem;


  register int ml0 = ( len0 < newLen0 ) ? len0 : newLen0;
  register int ml1 = ( len1 < newLen1 ) ? len1 : newLen1;

  register int i,j;
  for( i=0; i<ml0; i++ )
    for( j=0; j<ml1; j++ )
      nv[i*newLen1+j] = this->v[seek(i,j)];

  for( i=ml0; i<newLen0; i++ )
    for( j=ml1; j<newLen1; j++ )
      nv[i*newLen1+j] = init;

  if( --(this->ref)->count == 0 ) delete (this->ref);
  this->ref   = nref;
  this->v 	= nv;
  len0 	= newLen0;
  len1 	= skip0 = newLen1;
  skip1 = 1;

  return *this;
}



// ========================
// methods without argument
// ========================

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::abs()
{
  register T* tv0	= this->v;

  if( skip0==len1 )
  {
    register int tl	= len0*len1;
    while( tl-- ) {
      if (*tv0 <= T(0))
	*tv0 = -*tv0;
      tv0++;
    }
  }
  else
  {
    register T *tv1;
    register T *tv0 = this->v;
    register int ts0 	 = skip0;
    register int ts1 	 = skip1;
    register int tl0 	 = len0;
    register int tl1;

    while( tl0-- ) {
      tv1 = tv0;
      tl1 = len1;
      while( tl1-- ) {
	if (*tv1 <= T(0))
	  *tv1 = -*tv1;
	tv1 += ts1;
      }
      tv0 += ts0;
    }
  }
  return *this;
}


template <class T>
inline IP_MatrixIndex IP_Matrix<T>::minDist(const T& var) const
{
  IP_MatrixIndex result = {0,0};

  double min_dist = dist(*(this->v),var);


  register T *tv1;
  register T *tv0 	= &(this->v)[seek(len0-1,len1-1)];
  register int ts0 	= skip0;
  register int ts1 	= skip1;
  register int tl0 	= len0;
  register int tl1;

  while( tl0-- )
  {
    tv1 = tv0;
    tl1 = len1;
    while( tl1-- )
    {

      if( (dist(*tv1,var))<min_dist )
      {
		result.row = tl0;
		result.col = tl1;
		min_dist = dist(*tv1,var);
      }
      tv1 -= ts1;
    }
    tv0 -= ts0;
  }
  return result;
}



template <class T>
inline IP_MatrixIndex IP_Matrix<T>::argMax() const
{
  IP_MatrixIndex result = {0,0};

  T maximum = *(this->v);

  register T *tv1;
  register T *tv0 	= &(this->v)[seek(len0-1,len1-1)];
  register int ts0 	= skip0;
  register int ts1 	= skip1;
  register int tl0 	= len0;
  register int tl1;

  while( tl0-- )
  {
    tv1 = tv0;
    tl1 = len1;
    while( tl1-- )
    {
      if( maximum<*tv1 )
      {
	result.row = tl0;
	result.col = tl1;
	maximum = *tv1;
      }
      tv1 -= ts1;
    }
    tv0 -= ts0;
  }
  return result;
}

template <class T>
inline T&
IP_Matrix<T>::max()
{
  IP_MatrixIndex mi=argMax();
  return (this->v)[seek(mi.row,mi.col)];
}
template <class T>
inline T
IP_Matrix<T>::max() const
{
  IP_MatrixIndex mi=argMax();
  return (this->v)[seek(mi.row,mi.col)];
}

template <class T>
inline IP_MatrixIndex IP_Matrix<T>::argMin() const
{
  IP_MatrixIndex result = {0,0};

  T minimum = *(this->v);

  register T *tv1;
  register T *tv0 	= &(this->v)[seek(len0-1,len1-1)];
  register int ts0 	= skip0;
  register int ts1 	= skip1;
  register int tl0 	= len0;
  register int tl1;

  while( tl0-- )
  {
    tv1 = tv0;
    tl1 = len1;
    while( tl1-- )
    {
      if( minimum>*tv1 )
      {
	result.row = tl0;
	result.col = tl1;
	minimum = *tv1;
      }
      tv1 -= ts1;
    }
    tv0 -= ts0;
  }
  return result;
}

template <class T>
inline T& IP_Matrix<T>::min()
{
  IP_MatrixIndex mi=argMin();
  return (this->v)[seek(mi.row,mi.col)];
}
template <class T>
inline T IP_Matrix<T>::min() const
{
  IP_MatrixIndex mi=argMin();
  return (this->v)[seek(mi.row,mi.col)];
}


template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator - () const
{
  IP_Matrix<T>	result(len0,len1);

  register T *tv0	= (this->v);
  register T *rv0	= result.v;

  register T *tv1, *rv1;

  register int tl1 = len1;
  register int ts1 = skip1;
  register int rs1 = result.skip1;

  register int tl0 = len0;
  register int ts0 = skip0;
  register int rs0 = result.skip0;

  while( tl0-- )
  {
    tv1 = tv0;
    rv1 = rv0;
    tl1 = len1;
    while( tl1-- )
    {
      *rv1 = -*tv1;
      tv1 += ts1;
      rv1 += rs1;
    }
    tv0 += ts0;
    rv0 += rs0;
  }
  return *this;
}


template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::row_center () const
{
	IP_Matrix<T> result(len0,len1);
	for (int i=0; i<len0; i++)
		for (int j=0; j<len1; j++)
			result.element(i,j)=element(i,j)-row(i).mean();
	return result;
}

template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::col_center () const
{
	IP_Matrix<T> result(len0,len1);
	for (int i=0; i<len0; i++)
		for (int j=0; j<len1; j++)
			result.element(i,j)=element(i,j)-col(j).mean();
	return result;
}



template<class T>
inline IP_Matrix<T>&
IP_Matrix<T>::multAssign( const IP_Matrix<T>& other,
			  IP_Matrix<T>& result ) const
{

  register T *tv1  = this->v;
  register T *ov0  = other.v;
  register T *rv 	= result.v;
  register int ts1	= skip1;
  register int os0	= other.skip0;
  register int tl1;

  register T *ov1  = other.v;
  register int os1	= other.skip1;
  register int ol1;

  register T *tv0 	=this->v;
  register int ts0 	= skip0;
  register int tl0	= len0;

  while( tl0-- ) // all rows of this
  {
    ol1 = other.len1;
    while( ol1-- ) // all cols of other
    {
      tl1 = len1;
      ov0 = ov1;
      tv1 = tv0;
      *rv = 0;
      while( tl1-- ) // all items in *this.row and other.col
      {
	*rv += *tv1 * *ov0;
	tv1 += ts1;
	ov0 += os0;
      }
      rv++;
      ov1 += os1;
    }
    ov1 = other.v;
    tv0 += ts0;
  }

  return result;
}


template<class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator * ( const IP_Matrix<T>& other ) const
{
  IP_Matrix<T> result( len0, other.len1 );
  multAssign( other, result );
  return result;
}


template<class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator / ( const IP_Matrix<T>& other ) const
{
  IP_Matrix<T> result( len0, other.len1 );
  result = *this;
  result /= other;
  return result;
}


// matrix addition and subtraction

template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator+ ( const IP_Matrix<T>& other ) const
{
  IP_Matrix<T> result( len0, len1 );
  result = *this;
  result += other;

  return result;
}

template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator- ( const IP_Matrix<T>& other ) const
{
  IP_Matrix<T> result( len0, len1 );
  result = *this;
  result -= other;

  return result;
}


#define defop(op) 			       				\
									\
template <class T>							\
IP_Matrix<T>& IP_Matrix<T>::operator op ( const IP_Matrix<T>& other ) 	\
{         								\
  		\
                                                                	\
  register T *tv0	= this->v;					\
  register T *ov0	= other.v;				\
                                                                \
  if( skip0==len1 && other.skip0==other.len1 )	       		\
  {								\
    register int tl 	= len0*len1;       		        \
    while( tl-- )			     			\
      *tv0++ op *ov0++;						\
  }								\
  else								\
  {								\
    register T *tv1, *ov1;					\
                                                                \
    register int tl1 = len1;					\
    register int ts1 = skip1;					\
    register int os1 = other.skip1;				\
                                                                \
    register int tl0 = len0;					\
    register int ts0 = skip0;					\
    register int os0 = other.skip0;				\
                                                                \
    while( tl0-- )						\
    {								\
      tv1 = tv0;						\
      ov1 = ov0;						\
      tl1 = len1;						\
      while( tl1-- )						\
      {								\
	*tv1 op *ov1;						\
	tv1 += ts1;						\
	ov1 += os1;						\
      }								\
      tv0 += ts0;						\
      ov0 += os0;						\
    }								\
  }								\
  return *this;							\
}

defop(+=)
defop(-=)

#undef defop // (op)


template <class T>
IP_Matrix<T>& IP_Matrix<T>::operator /= ( const IP_Matrix<T>& other )
{


  register T *tv0	=this->v;
  register T *ov0	= other.v;

  if( skip0==len1 && other.skip0==other.len1 )
  {
    register int tl 	= len0*len1;
    while( tl-- )
		if (*ov0!=0.0)    *tv0++ /= *ov0++;
		else {
			*tv0++ =1.0;
			ov0++;
		}
  }
  else
  {
    register T *tv1, *ov1;

    register int tl1 = len1;
    register int ts1 = skip1;
    register int os1 = other.skip1;

    register int tl0 = len0;
    register int ts0 = skip0;
    register int os0 = other.skip0;

    while( tl0-- )
    {
      tv1 = tv0;
      ov1 = ov0;
      tl1 = len1;
      while( tl1-- )
      {
		if(*ov1!=0.0)	*tv1 /= *ov1;
		else			*tv1 = 1.0;
		tv1 += ts1;
		ov1 += os1;
      }
      tv0 += ts0;
      ov0 += os0;
    }
  }
  return *this;
}



// ====================
// pointwise operations
// ====================

template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::pointMul( const IP_Matrix<T>& other ) const
{
  IP_Matrix<T>	result(other.len0,other.len1);

  // FIXME
  ((IP_Matrix<T>*)this)->pointMulAssign(other,result);
  return result;
}

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::pointMulAssign (const IP_Matrix<T>& other,
			      IP_Matrix<T>& result )
{


  register T *tv1, *ov1, *rv1;

  register int tl1 = len1;
  register int ts1 = skip1;
  register int os1 = other.skip1;
  register int rs1 = result.skip1;

  register int tl0 = len0;
  register int ts0 = skip0;
  register int os0 = other.skip0;
  register int rs0 = result.skip0;

  register T *tv0	=this->v;
  register T *ov0	= other.v;
  register T *rv0	= result.v;

  while( tl0-- )
  {
    rv1 = rv0;
    tv1 = tv0;
    ov1 = ov0;

    tl1 = len1;
    while( tl1-- )
    {
      *rv1 = *tv1 * *ov1;
      rv1 += rs1;
      tv1 += ts1;
      ov1 += os1;
    }
    rv0 += rs0;
    tv0 += ts0;
    ov0 += os0;
  }
  return *this;
}

template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::pointDiv( const IP_Matrix<T>& other )
{
  IP_Matrix<T>	result(other.len0,other.len1);

  pointDivAssign(other,result);
  return result;
}

template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::pointDivAssign (const IP_Matrix<T>& other,
			      IP_Matrix<T>& result )
{


  register T *tv1, *ov1, *rv1;

  register int tl1 = len1;
  register int ts1 = skip1;
  register int os1 = other.skip1;
  register int rs1 = result.skip1;

  register int tl0 = len0;
  register int ts0 = skip0;
  register int os0 = other.skip0;
  register int rs0 = result.skip0;

  register T *tv0	=this->v;
  register T *ov0	= other.v;
  register T *rv0	= result.v;

  while( tl0-- )
  {
    rv1 = rv0;
    tv1 = tv0;
    ov1 = ov0;

    tl1 = len1;
    while( tl1-- )
    {
      *rv1 = *tv1 / *ov1;

      rv1 += rs1;
      tv1 += ts1;
      ov1 += os1;
    }
    rv0 += rs0;
    tv0 += ts0;
    ov0 += os0;
  }
  return *this;
}



template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::row_p2sqr_dist() const
{
	IP_Matrix<T>	result(len0, len0);
	for (int i=0; i<len0; i++) {
		for (int j=0; j<i; j++) {
			double dist=0.0;
			for (int k=0; k<len1; k++) {
				double diff= element(i,k)-element(j,k);
				dist +=diff*diff;
			}
			result(i,j)=result(j,i)=sqrt(dist);
		}
	}

	result.diagonal().set2(0);
	return result;
}





template<class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator + ( T value )
{
  IP_Matrix<T> result( len0, len1 );
  result = *this;
  return result += value;
}


template<class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator - ( T value )
{
  IP_Matrix<T> result( len0, len1 );
  result = *this;
  return result -= value;
}


template<class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator * ( T value )
{
  IP_Matrix<T> result( len0, len1 );
  result = *this;
  return result *= value;
}

template<class T>
inline IP_Matrix<T>
IP_Matrix<T>::operator / ( T value )
{
  IP_Matrix<T> result( len0, len1 );
  result = *this;
  return result /= value;
}

template<class T>
inline IP_Matrix<T> operator+ ( T value, const IP_Matrix<T>& mat )
{
  IP_Matrix<T>	result( mat.len0, mat.len1 );

  register const T *tv0	= mat.v;
  register T *rv0	= result.v;

  if( mat.skip0==mat.len1 && result.skip0==result.len1 )
  {
    register int tl 	= mat.len0*mat.len1;
    while( tl-- )
    {
	  *rv0=value+ *tv0;
      rv0++; tv0++;
    }
  }
  else
  {
    register const T *tv1;
    register T *rv1;

    register int tl1 = mat.len1;
    register int ts1 = mat.skip1;
    register int rs1 = result.skip1;

    register int tl0 = mat.len0;
    register int ts0 = mat.skip0;
    register int rs0 = result.skip0;

    while( tl0-- )
    {
      tv1 = tv0;
      rv1 = rv0;
      tl1 = mat.len1;
      while( tl1-- )
      {
	*rv1=value+ *tv1;
	tv1 += ts1;
	rv1 += rs1;
      }
      tv0 += ts0;
      rv0 += rs0;
    }
  }
  return result;
}


template<class T>
inline IP_Matrix<T> operator- ( T value, const IP_Matrix<T>& mat )
{
  IP_Matrix<T>	result( mat.len0, mat.len1 );

  register const T *tv0	= mat.v;
  register T *rv0	= result.v;

  if( mat.skip0==mat.len1 && result.skip0==result.len1 )
  {
    register int tl 	= mat.len0*mat.len1;
    while( tl-- )
    {
	  *rv0=value- *tv0;
      rv0++; tv0++;
    }
  }
  else
  {
    register const T *tv1;
    register T *rv1;

    register int tl1 = mat.len1;
    register int ts1 = mat.skip1;
    register int rs1 = result.skip1;

    register int tl0 = mat.len0;
    register int ts0 = mat.skip0;
    register int rs0 = result.skip0;

    while( tl0-- )
    {
      tv1 = tv0;
      rv1 = rv0;
      tl1 = mat.len1;
      while( tl1-- )
      {
	*rv1=value- *tv1;
	tv1 += ts1;
	rv1 += rs1;
      }
      tv0 += ts0;
      rv0 += rs0;
    }
  }
  return result;
}



// ====================================
// Multiplication with double (scaling)
// ====================================

template<class T>
inline IP_Matrix<T>
IP_Matrix<T>::mul( double value ) const
{
  IP_Matrix<T>	result( len0, len1 );
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
  cerr << "mul was not instantiated properly for cmplx\n";
#else

  register T *tv0	= this->v;
  register T *rv0	= result.v;

  if( skip0==len1 && result.skip0==result.len1 )
  {
    register int tl 	= len0*len1;
    while( tl-- )
    {
      *rv0 = (T)(value * *tv0);
      rv0++; tv0++;
    }
  }
  else
  {
    register T *tv1, *rv1;

    register int tl1;
    register int ts1 = skip1;
    register int rs1 = result.skip1;

    register int tl0 = len0;
    register int ts0 = skip0;
    register int rs0 = result.skip0;

    while( tl0-- )
    {
      tv1 = tv0;
      rv1 = rv0;
      tl1 = len1;
      while( tl1-- )
      {
	*rv1 = (T)(value * *tv1);
	tv1 += ts1;
	rv1 += rs1;
      }
      tv0 += ts0;
      rv0 += rs0;
    }

  }
#endif
  return result;
}

#define defop(op) 							\
template <class T>                                                   	\
IP_Matrix<T>& IP_Matrix<T>::operator op ( T value )			\
{          								\
  register T* tv0 =this->v;							\
                                                                        \
  if( skip0==len1 && skip1 == 1 )      	       			       	\
  {									\
    register int tl	= len0*len1;        		       		\
    while( tl-- )			      			       	\
      *tv0++ op value;							\
  }									\
  else									\
  { 									\
    register T *tv1;							\
    register int ts0 	 = skip0;      					\
    register int ts1 	 = skip1;      					\
    register int tl0 	 = len0;       					\
    register int tl1;							\
                                                                        \
    while( tl0-- )							\
    {									\
      tv1 = tv0;							\
      tl1 = len1;							\
      while( tl1-- )							\
      {									\
    	*tv1 op value;							\
	tv1 += ts1;							\
      }									\
      tv0 += ts0;						        \
    }									\
  }									\
  return *this;								\
}

defop(=)
defop(*=)
defop(/=)
defop(+=)
defop(-=)

#undef defop


template <class T>
inline IP_Matrix<T>&
IP_Matrix<T>::mulAssign( double value )
{
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
  T tmp(value,value);
#endif

  register T* tv0	=this->v;

  if( skip0==len1 )
  {
    register int tl	= len0*len1;
    while( tl-- )
    {
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
    *tv0 = (T)(tmp * *tv0);
#else
      *tv0 = (T)(value * *tv0);
#endif
      tv0++;
    }
  }
  else
  {
    register T *tv1;
    register T *tv0 =this->v;
    register int ts0 	 = skip0;
    register int ts1 	 = skip1;
    register int tl0 	 = len0;
    register int tl1;

    while( tl0-- )
    {
      tv1 = tv0;
      tl1 = len1;
      while( tl1-- )
      {
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
    *tv1 = (T)(tmp * *tv1);
#else
	*tv1 = (T)(value * *tv1);
#endif

	tv1 += ts1;
      }
      tv0 += ts0;
    }
  }

  return *this;
}



template <class T>
inline T
IP_Matrix<T>::trace() const
{
  //ASSERT( len0==len1, "Can't compute trace of a non-rectangular matrix." );

  T trace         = 0;
  register T* tv  =this->v;
  register int tl = len0;
  register int ts = skip0+skip1;

  while( tl-- )
  {
    trace += *tv;
    tv += ts;
  }
  return trace;
}



// //////////////////////////////////////////////////////////////////////////////////

// =====
// norms
// =====

template <class T>
inline double
IP_Matrix<T>::p1norm() const
{
  double result = 0;

  register T *tv1;
  register T *tv0 =this->v;
  register int ts0 	 = skip0;
  register int ts1 	 = skip1;
  register int tl0 	 = len0;
  register int tl1;

  while( tl0-- )
  {
    tv1 = tv0;
    tl1 = len1;
    while( tl1-- )
    {
      result += IP_abs(*tv1);
      tv1 += ts1;
    }
    tv0 += ts0;
  }
  return result;
}

template <class T>
inline T
IP_Matrix<T>::sum() const
{
  T result = 0;
  register T* tv0 =this->v;

  if( skip0==len1 )
  {
    register int tl	= len0*len1;
    while( tl-- )
      result += *tv0++;
  }
  else
  {
    register T *tv1;
    register T *tv0 =this->v;
    register int ts0 	 = skip0;
    register int ts1 	 = skip1;
    register int tl0 	 = len0;
    register int tl1;

    while( tl0-- )
    {
      tv1 = tv0;
      tl1 = len1;
      while( tl1-- )
      {
    	result += *tv1;
	tv1 += ts1;
      }
      tv0 += ts0;
    }
  }
  return result;
}

template <class T>
inline double
IP_Matrix<T>::p2sqr() const
{
  double result = 0;
  register T* tv0 =this->v;

  if( skip0==len1 )
  {
    register int tl	= len0*len1;
    while( tl-- )
    {
      result += *tv0 * *tv0;
      tv0++;
    }
  }
  else
  {
    register T *tv1;
    register T *tv0 =this->v;
    register int ts0 	 = skip0;
    register int ts1 	 = skip1;
    register int tl0 	 = len0;
    register int tl1;

    while( tl0-- )
    {
      tv1 = tv0;
      tl1 = len1;
      while( tl1-- )
      {
    	result += *tv1 * *tv1;
	tv1 += ts1;
      }
      tv0 += ts0;
    }
  }
  return result;
}

template <class T>
inline double
IP_Matrix<T>::p2norm() const
{
  return sqrt(p2sqr());
}

template <class T>
inline T
IP_Matrix<T>::mean() const
{
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
	T a;
  cerr << "mean was not instantiated properly for cmplx\n";
  return a;
#else

  return  (T) (sum()/(double)(len0*len1));

#endif
}



template <class T>
inline double
IP_Matrix<T>::rms() const
{
  return sqrt( p2sqr()/double(len0*len1) );
}

// =========
// utilities
// =========

template<class T>
inline IP_Matrix<T>&
IP_Matrix<T>::mirror()
{

  // PLEASE OPTIMIZE!
  for( int i=1; i<len0; i++ )
    for( int j=0; j<i; j++ )
      element(j,i) = element(i,j);

  return (*this);
}




template <class T>
inline T**
IP_Matrix<T>::nr()
{
	T** mrcps = new T* [len0];

	register int ml0    	= len0;
	register int ml1    	= len1;
	register double** rc	= mrcps;      	// array of pointers
	register double*  vv  	=this->v-1;		// Hack!

	while( ml0-- ){
		*rc = vv;
		rc++;
		vv+=ml1;
	}
	mrcps--;

	return mrcps;
}

template <class T>
inline IP_Matrix<T>
IP_Matrix<T>::window( int b0, int b1, int l0, int l1 )
{
  int skp0 = (l0<0 ? -skip0 : skip0);
  int skp1 = (l1<0 ? -skip1 : skip1);

  IP_Matrix<T> result( *this, this->v+seek(b0,b1),
		      (l0==0)?(len0-b0):ABS(l0),
		      (l1==0)?(len1-b1):ABS(l1),
		      skp0, skp1 );



  return result;
}

template <class T>
inline IP_Vector<T>
IP_Matrix<T>::minrow() const
{
	IP_Vector<T> result(len1);
	for (int i=0; i<len1; i++)
		result(i)=col(i).min();
	return result;
}


template <class T>
inline IP_Vector<T>
IP_Matrix<T>::maxrow() const
{
	IP_Vector<T> result(len1);
	for (int i=0; i<len1; i++)
		result(i)=col(i).max();
	return result;
}

#endif // IP_MATRIXH
