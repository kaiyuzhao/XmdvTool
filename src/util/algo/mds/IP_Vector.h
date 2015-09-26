
#ifndef IP_VECTOR_HH
#define IP_VECTOR_HH

#include "util/algo/mds/IP_Array.h"
#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a>b)?b:a)
#define ABS(a) ((a>=0)?a:-a)

template <class T> class IP_Matrix;

template <class T>
class IP_Vector : public IP_Array<T>
{
public:



  // ============
  // construction
  // ============

  // standard constructor
  IP_Vector( int l=0 );

  // constructor with reference (copy=false) and copy (copy=true) semantic
  IP_Vector( const IP_Vector<T>& other );
  IP_Vector( const IP_Vector<T>& other, bool copy );

  // constructor with memory allocated elsewhere in IP class
  IP_Vector( const IP_Array<T>& other, T *mem, int l, int s=1 );

  // constructor with memory allocated elsewhere in the world
  IP_Vector( T *mem, int l, int s=1, bool handleMem=false );

  // destructor
  ~IP_Vector();


  // ==============
  // element access
  // ==============

  // length (of array) and skip (offset between elements)
  int			len;
  int			skip;

  // methods to set and get array length (without affecting memory size!)
  inline int	       	length() const;
  inline int	       	length( int l );

  // access pointer to i-th element
  inline T*		elementPtr ( int i );
  inline const T*      	elementPtr ( int i ) const;

  // access i-th element
//  inline T ele (int i);
  inline T&		element ( int i );
  inline const T&   	element ( int i ) const;

  // access i-th element
  inline T&       	operator() ( int i );
  inline const T&  	operator() ( int i ) const;

  // access i-th element
  inline T&       	operator[] ( int i );
  inline const T&  	operator[] ( int i ) const;

  // return sub-vector (reference semantic)
  IP_Vector		window( int start, int length );

  // internal: computes offset between memory start and i-th element
  inline int		seek( int i ) const;


  // ================
  // copy, assignment
  // ================

  // initialize with memory allocated elsewhere
  IP_Vector&		init( T* mem, int l, int s=1, bool handleMem=false );

  // reference semantic - assign this and other
  IP_Vector&		assign ( const IP_Vector& other );

  // copy semantic - copy other to this
  IP_Vector&		copy ( const IP_Vector& other );

  // copy semantic - copy other to this
  inline IP_Vector&    	operator= ( const IP_Vector& other );

  // copy semantic - copy other to this
  // inline IP_Vector&    	operator= ( const T* other );

  // assign value to all elements
  IP_Vector&		operator =  ( T value );
  inline IP_Vector<T>&	set2( T value );

  // copy matrix into vector of length len0*len1 (row by row)
  IP_Vector&		copy( const IP_Matrix<T>& other );

  // ======
  // sizing
  // ======

  // simple resize
  IP_Vector&		setSize ( int l );

  // fit to other
  IP_Vector&		setSize ( const IP_Vector<T>& other );

  // change size without allocating new memory (ie. make it smaller)
  IP_Vector&		changeSize( int l );

  // resize with copy
  IP_Vector&		setSizeWithCopy ( int l );


  // ========================
  // methods without argument
  // ========================

  // abs all elements
  IP_Vector&		abs();

  // exp all elements and return as _new_ vector
  IP_Vector		exponential() const     ;

  // sqr all elements
  IP_Vector&		sqr();

  // pow all elements with integral exponent
  IP_Vector&		pow( int exponent );

  // pow all elements with double exponent
  IP_Vector&		pow( double exponent );

  // sqrt all elements
  IP_Vector&		sqrtize();

  // y(i) = x(i)-x(i-1)
  IP_Vector&		differentiate( T last ) ;

  // -x
  IP_Vector		operator - () const;

  // Invert all elements: y[i] = 1./(eps+x[i]);
  IP_Vector&		invert( T eps );

  // ============================
  // methods with scalar argument
  // ============================

  IP_Vector		operator *  ( T value ) const;
  IP_Vector		operator /  ( T value ) const;
  IP_Vector		operator +  ( T value ) const;
  IP_Vector		operator -  ( T value ) const;

  IP_Vector&		operator *= ( T value );
  IP_Vector&		operator /= ( T value );
  IP_Vector&		operator += ( T value );
  IP_Vector&		operator -= ( T value );

#define defop(op)				\
	\
	friend IP_Vector<T> operator op ( T value, const IP_Vector<T>& vec )\
  {						\
  IP_Vector<T> result(vec.len);		\
  \
  register int tl	= vec.len;	       	\
  register const T* tv 	= vec.v;	       	\
  register int ts    	= vec.skip;  		\
  register T* rv 	= result.v;       	\
  \
  while( tl-- )					\
  {						\
  *rv = value op *tv;				\
  tv += ts; rv++;				\
  } 						\
  return result;         			\
  }

  defop(+)
  defop(-)
  defop(*)
  defop(/)

#undef defop

//  friend IP_Vector<T> 	operator * ( T value, const IP_Vector<T>& vec );
//  friend IP_Vector<T> 	operator / ( T value, const IP_Vector<T>& vec );
//  friend IP_Vector<T> 	operator + ( T value, const IP_Vector<T>& vec );
//  friend IP_Vector<T> 	operator - ( T value, const IP_Vector<T>& vec );


  IP_Vector<T>          mul( double value ) const;
  IP_Vector<T>&         mulAssign( double value ) ;

  // set elements to random value
  IP_Vector&		random(const IP_Vector<T>& lower_bound, const IP_Vector<T>& upper_bound );
  IP_Vector&		random   ( T lower_bound, T upper_bound );
  IP_Vector&		addRandom( T lower_bound, T upper_bound );

  // ============================
  // methods with vector argument
  // ============================

  // scalar product
  T			operator * ( const IP_Vector<T>& other ) const;

  // weighted scalar product
  T			prod ( const IP_Vector<T>& v1,
			       const IP_Vector<T>& v2 ) const;

  // vector product
  IP_Matrix<T>		dotTranspose( const IP_Vector<T>& other ) const;

  // vector addition/subtraction
  IP_Vector            	operator + ( const IP_Vector<T>& other ) const;
  IP_Vector            	operator - ( const IP_Vector<T>& other ) const;

  IP_Vector&           	operator += ( const IP_Vector<T>& other );
  IP_Vector&           	operator -= ( const IP_Vector<T>& other );

  // comparison
  bool                  operator == ( const IP_Vector<T>& other ) const;
  inline bool           operator != ( const IP_Vector<T>& other ) const;

  // elementwise multiplication
  IP_Vector            pointMult ( const IP_Vector& other ) const ;
  IP_Vector&     pointMultAssign ( const IP_Vector& v1,
				   const IP_Vector& v2 );
  IP_Vector&     pointMultAssign ( const IP_Vector& other )  ;

  // elementwise division
  IP_Vector             pointDiv ( const IP_Vector& other ) const ;
  IP_Vector&     pointDivAssign  ( const IP_Vector& other )  ;

  // vector of absolute differences
  //IP_Vector<T>		absDiff ( const IP_Vector<T>& other ) const;

  // ============================
  // methods with matrix argument
  // ============================

  // vector*matrix multiplication
  IP_Vector		operator * ( const IP_Matrix<T>& m ) const;

  // =====
  // norms
  // =====

  // y = sum_i abs(x(i))
  double       	p1norm() const;

  // y = sum_i sqrt( x(i)*x(i) )
  double     	p2norm() const;

  // y = sum_i x(i)*x(i)
  double       	p2sqr() const;		// y = x(i)*x(i)

  // y = sum_i x(i)
  T		sum() const;

  // y = product_i x(i)
  T		product() const;

  // y = sum()/length
  T		mean() const;

  // y = sum_i (x(i)-mean)^2
  T		variance( T mean ) const;

  // y = p2sqr()/length
  double	meanSqr( bool=true ) const;

  // y = sqrt( p2sqr()/length )
  double       	rms() const;


  // =======
  // metrics
  // =======

  // Minkowski p1 distance
  T		p1dist( const IP_Vector<T>& other ) const;

  // _squared_ euclidian distance
  T            	p2sqr_dist( const IP_Vector<T>& other ) const;
  T				p2sqrt_dist( const IP_Vector<T>& other ) const;


  friend T dist(const IP_Vector<T>& v1, const IP_Vector<T>& v2)
  {
	  return (v1.p2sqrt_dist(v2));
  }

  // =================
  // maximum / minimum
  // =================

  // of values
  T&			max();
  T			max() const;
  T&			min();
  T			min() const;

  // of absolute values
  T			maxAbs() const;
  T			minAbs() const;

  // ===============
  // argmax / argmin
  // ===============

  // of values
  int			argMin() const;
  int			argMax() const;

  // of absolute values
  int			argMinAbs() const;
  int			argMaxAbs() const;

  // N maximal/minimal values FIXME: To be optimized!
  //IP_Vector<int4>	argMax( int N ) const;
  //IP_Vector<int4>	argMin( int N ) const;

  // ============
  // thresholding
  // ============

  IP_Vector&    	setMax( T theta );
  IP_Vector&    	setMin( T theta );


  // =========
  // utilities
  // =========

  // apply function to all elements
  IP_Vector<T>& 	apply( T (*function)(T) );
  IP_Vector<T>& 	apply( T (*function)(T,void*), void* data );

  // reverse order of elements
  IP_Vector&		reverse();
  void bubble_sort();
  void quick_sort();
  void merge_sort();
  void mg_sort(int b, int e);

  // v[j] = 1 for j==i and v[j] = 0 else
  IP_Vector&		canonic( int i );
  IP_Vector<T>		right_merge(const IP_Vector<T>& other);

  // =====================================================================
  // Numerical recipes interface:
  // The returned temporary IP_RecipesVector<T> converts itself
  // into a Numerical recipes compatible c-style vector T*.
  // =====================================================================

  ///IP_RecipesVector<T>	nr();
  T* nr();

	friend std::istream& operator >> (std::istream& is, IP_Vector<T>& vec)
	{
		for (int i=0; i<vec.len; i++)
			is>>vec.element(i);
		return is;
	}
	friend std::ostream& operator << (std::ostream& os, const IP_Vector<T>& vec)
	{
		char tbl='\t';
		for (int i=0; i<vec.len; i++)
			os<<vec.element(i)<<tbl;
		//os<<vec.element(i)<<endl;
		//os<<endl;

		return os;
	}


};


// ==================================
// implementation of inline functions
// ==================================

template <class T>
inline int
IP_Vector<T>::length() const
{  return len; }

template <class T>
inline int
IP_Vector<T>::length( int l )
{  return len=l; }

// ===========================
// assignment, copy, operator=
// ===========================

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::operator= ( const IP_Vector<T>& other )
{
  IP_Vector<T>::copy(other);
  return *this;
}

template <class T>
inline bool
IP_Vector<T>::operator != ( const IP_Vector<T>& other ) const
{
  return !(*this == other);
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::set2( T value )
{
  return IP_Vector<T>::operator=(value);
}


// access pointer to i-th element
template <class T>
inline T*
IP_Vector<T>::elementPtr( int i )
{

	return (this->v)+seek(i);
}

template <class T>
inline const T*
IP_Vector<T>::elementPtr( int i ) const
{

	return (this->v)+seek(i);
}

/*
template <class T>
inline T
IP_Vector<T>::ele( int i )
{  return (this->v)[seek(i)]; }
*/

template <class T>
inline T&
IP_Vector<T>::element( int i )
{
	return (this->v)[seek(i)]; }
//{ return *elementPtr(i); }

template <class T>
inline const T&
IP_Vector<T>::element( int i ) const
{   return (this->v)[seek(i)]; }
//{ return *elementPtr(i); }

template <class T>
inline T&
IP_Vector<T>::operator() ( int i )
{  return *elementPtr(i); }

template <class T>
inline const T&
IP_Vector<T>::operator() ( int i ) const
{  return *elementPtr(i); }

template <class T>
inline T&
IP_Vector<T>::operator[] ( int i )
{  return *elementPtr(i); }

template <class T>
inline const T&
IP_Vector<T>::operator[] ( int i ) const
{  return *elementPtr(i); }

template <class T>
inline int
IP_Vector<T>::seek( int i ) const
{
  return i*skip;
}

template <class T>
inline T
abs( const IP_Vector<T>& vec )
{  return vec.abs(); }



#include <stdlib.h>
#include <time.h>


// ========================
// construction/destruction
// ========================

template <class T>
inline IP_Vector<T>::IP_Vector( int l ) :
  IP_Array<T>( l ),
  len( l ),
  skip( 1 )
{}

template <class T>
inline IP_Vector<T>::IP_Vector( const IP_Vector<T>& other ) :
  IP_Array<T>( other ),
  len( other.len ),
  skip( other.skip )
{}

template <class T>
inline IP_Vector<T>::IP_Vector( const IP_Vector<T>& other, bool cpy ) :
  IP_Array<T>( other.len ),
  len( other.len ),
  skip( 1 )
{
  if(cpy) copy(other);
}

template <class T>
inline IP_Vector<T>::IP_Vector( const IP_Array<T>& other, T *mem, int l, int s ) :
  IP_Array<T>( other, mem ),
  len( l ),
  skip( s )
{}

template<class T>
inline IP_Vector<T>::IP_Vector(  T* mem, int l, int s, bool handleMem )
{
  init(mem,l,s,handleMem);
}

template<class T>
inline IP_Vector<T>::~IP_Vector()
{ }

template <class T>
inline IP_Vector<T>
IP_Vector<T>::window( int s, int l )
{


  int skp = (l<0 ? -skip : skip);
  IP_Vector<T> result( *this, (this->v)+seek(s), (l==0)?(len-s):ABS(l), skp );
  return result;
}

template<class T>
inline IP_Vector<T>&
IP_Vector<T>::init( T* mem, int l, int s, bool handleMem )
{
  IP_Array<T>::init(mem,handleMem);
  len = l;
  skip = s;

  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::assign( const IP_Vector<T>& other )
{
  IP_Array<T>::assign(other);
  len = other.len;
  skip = other.skip;

  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::copy( const IP_Vector<T>& other )
{

  if( !len )
  { setSize(other); }
  else
  {
	  ;
  }

  if( !((this->v) == other.v) )
    {
    if( skip==1 && other.skip==1 )
      memcpy( (this->v), other.v, len*sizeof(T) );
    else
    {
      register int tl	= len;
      register T* tv 	= (this->v);
      register int ts    	= skip;
      register T* ov 	= other.v;
      register int os   	= other.skip;
      while( tl-- )
      {
        *tv = *ov;
        tv += ts;
        ov += os;
      }
    }
  }

  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::copy( const IP_Matrix<T>& other )
{
  if( len != other.len0*other.len1 ) setSize( other.len0*other.len1 );

  register T* tv        = (this->v);
  register int ts   	= skip;

  register int ol0	= other.len0;
  register int ol1;
  register T*  ov1;
  register T*  ov0	= other.v;
  register int os1   	= other.skip1;
  register int os0   	= other.skip0;

  while( ol0-- )
  {
    ol1 = other.len1;
    ov1 = ov0;

    while( ol1-- )
    {
      *tv = *ov1;
      tv  += ts;
      ov1 += os1;
    }
    ov0 += os0;
  }
  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::setSize( int l )
{
  this->size(l);
  len = l;
  skip = 1;

  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::setSize( const IP_Vector<T>& other )
{
  this->size(other.len);
  len = other.len;
  skip = 1;

  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::changeSize( int l )
{  len=l; return *this; }

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::setSizeWithCopy( int l )
{
  IP_Array<T>::resize( len, l );
  len = l;
  skip = 1;
  return *this;
}

// ==========================
// operators without argument
// ==========================

template<class T>
inline IP_Vector<T>
IP_Vector<T>::operator - () const
{
  IP_Vector<T> result( len );

  register int tl	= len;
  register T* tv 	= (this->v);
  register T* rv 	= result.v;

  if( skip==1 )
  {
    while( tl-- )
      *rv++ = -*tv++;
  }
  else
  {
    register int ts = skip;
    while( tl-- )
    {
      *rv = -*tv;
      tv += ts;
      rv++;
    }
  }
  return result;
}

// ==============================
// operators with scalar argument
// ==============================

// =====================================================================

//#undef defop
#define defop(op,op_inside)    			\
template<class T>				\
inline IP_Vector<T>					\
IP_Vector<T>::operator op ( T value ) const  	\
{ 						\
  IP_Vector<T> result( len );   		\
  result = *this;      				\
  return result op_inside value;       		\
}

defop(*,*=)
defop(+,+=)
defop(-,-=)
defop(/,/=)

#undef defop

// =====================================================================


#define defop(op)				\
						\
template <class T>				\
inline IP_Vector<T>&					\
IP_Vector<T>::operator op ( T value )		\
{   						\
  register int tl	= len;			\
  register T* tv 	= (this->v);			\
  register int ts    	= skip;			\
  while( tl-- )					\
  {						\
    *tv op value;				\
    tv += ts;					\
  } 						\
  return *this;         			\
}

defop(=)
defop(+=)
defop(-=)

// un-commented ---ap
defop(*=)
defop(/=)

#undef defop

template<class T>
inline IP_Vector<T>
IP_Vector<T>::mul( double value ) const
{

  IP_Vector<T>	result(len);
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
  cerr << "mul was not instantiated properly for cmplx\n";
#else
  register int tl = len;
  register T* tv  = (this->v);
  register T* rv  = result.v;
  register int ts = skip;

  while( tl-- )
  {
    *rv = T(*tv * value) ;
    tv += ts;
    rv++;
  }
#endif
  return result;
}

template<class T>
inline IP_Vector<T>&
IP_Vector<T>::mulAssign( double value )
{

  register int tl	= len;
  register T* tv 	= (this->v);
  register int ts    	= skip;

#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
  T tmp(value,value);
  while( tl-- )	{
	*tv = T(*tv * tmp) ;
    tv += ts;
  }

#else
  while( tl-- )	{
    *tv = T(*tv * value) ;
    tv += ts;
  }

#endif

  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::random( T lower_bound, T upper_bound )
{
  register int tl = len;
  register T* tv  = (this->v);
  register int ts = skip;
	//srand( (unsigned)time( NULL ) );

  T width = upper_bound-lower_bound;

  while( tl-- )
  {
	  double rnum=rand();
	  double randmax=RAND_MAX;
	*tv = T( width * (rnum/randmax) + lower_bound );
    tv += ts;
  }
  return *this;
}


template <class T>
inline IP_Vector<T>&
IP_Vector<T>::random(const IP_Vector<T>& lower_bound, const IP_Vector<T>& upper_bound )
{
  register int tl = len;
  register T* tv  = (this->v);
  register int ts = skip;

  register T* tv_low  = lower_bound.v;
  register int ts_low = lower_bound.skip;

  register T* tv_upper  = upper_bound.v;
  register int ts_upper = upper_bound.skip;

	//srand( (unsigned)time( NULL ) );


  while( tl-- )
  {
	T width=*tv_upper-*tv_low;


	*tv = T( width * ((double)rand()/(double)RAND_MAX) + *tv_low);
    tv += ts;
	tv_low +=ts_low;
	tv_upper += ts_upper;
  }
  return *this;
}



template <class T>
inline IP_Vector<T>&
IP_Vector<T>::addRandom( T lower_bound, T upper_bound )
{

  register int tl       = len;
  register T* tv        = (this->v);
  register int ts       = skip;
  T width = upper_bound-lower_bound;

#ifdef _WIN32
	//srand( (unsigned)time( NULL ) );
#endif

  while( tl-- )
  {
#ifdef _WIN32
    *tv += T( width * rand()/RAND_MAX + lower_bound );
#else
	*tv += T( width * drand48() + lower_bound );
#endif
    tv += ts;
  }
//#endif
  return *this;
}

// ==============================
// operators with vector argument
// ==============================

template <class T>
bool
inline IP_Vector<T>::operator== ( const IP_Vector<T>& other ) const
{
  if ( len != other.len ) return false;

  register int tl	= len;
  register T* tv 	= (this->v);
  register T* ov 	= other.v;

  if( skip==1 && other.skip==1 )
  {
    while( tl-- )
      if( *tv++ != *ov++ ) return false;
  }
  else
  {
    register int ts = skip;
    register int os = other.skip;
    while( tl-- )
    {
      if( *tv != *ov ) return false;
      tv += ts;
      ov += os;
    }
  }
  return true;
}

// =====================================================================

// inner product

template <class T>
T
inline IP_Vector<T>::operator* ( const IP_Vector<T>& other ) const
{

  T result       	= 0;
  register int tl	= len;
  register T* tv 	= (this->v);
  register T* ov 	= other.v;

  if( skip==1 && other.skip==1 )
  {
    while( tl-- )
      result += *tv++ * *ov++;
  }
  else
  {
    register int ts = skip;
    register int os = other.skip;
    while( tl-- )
    {
	  result += *tv * *ov;
	  tv += ts;
	  ov += os;
    }
  }
  return result;
}

// weighted inner product

template <class T>
T
inline IP_Vector<T>::prod( const IP_Vector<T>& v1, const IP_Vector<T>& v2 ) const
{

  T result       	= 0;
  register int tl	= len;

  register T* tv 	= (this->v);
  register T* v1v 	= v1.v;
  register T* v2v 	= v2.v;

  register int ts	= skip;
  register int vs1	= v1.skip;
  register int vs2	= v2.skip;

  while( tl-- )
  {
    result += (*tv) * (*v1v) * (*v2v);
    tv  += ts;
    v1v += vs1;
    v2v += vs2;
  }

  return result;
}

// =====================================================================
/*
template <class T>
inline IP_Vector<T>
IP_Vector<T>::operator* ( const IP_Matrix<T>& m ) const

{

  IP_Vector<T> 			result( m.len1 );
  IP_Matrix<T>::VectorIterator	col(m,0);

  for( col.first(); col.valid(); col.next() )
    result(col.index()) = *col * *this;

  return result;
}
*/

// =====================================================================

template <class T>
inline IP_Matrix<T>
IP_Vector<T>::dotTranspose( const IP_Vector<T>& other ) const
{
  IP_Matrix<T> result ( len, other.len );

  register T *ov;
  register T *vv	= (this->v);
  register T *rv 	= result.v;
  register int l0 	= len;
  register int l1 	= other.len;
  register int os  	= other.skip;
  register int vs   	= skip;

  while( l0-- )
  {
    l1 = other.len;
    ov = other.v;
    while( l1-- )
    {
      *rv = *vv * *ov;
      rv++;
      ov += os;
    }
    vv += vs;
  }
  return result;
}

// =====================================================================

#define defop(op) 			       				\
									\
template <class T>                                                  	\
inline IP_Vector<T>& IP_Vector<T>::operator op ( const IP_Vector<T>& other )	\
{            								\
                                                                	\
  register int tl 	  = len;         					\
  register T* tv  = (this->v);							\
  register T* ov  = other.v;						\
  register int ts = skip;						\
  register int os = other.skip;						\
									\
  while( tl-- )							\
  {								\
    *tv op *ov;							\
    tv += ts;							\
    ov += os;							\
  }								\
  return *this;							\
}

defop(+=)
defop(-=)

#undef defop // (op)

// =====================================================================
#define defop(op) 							\
									\
template <class T>                                                   	\
inline IP_Vector<T> 								\
IP_Vector<T>::operator op ( const IP_Vector<T>& other ) const    	\
{           								\
                                                                        \
  IP_Vector<T> result( len );	       					\
  register int tl 	  = len;         				       	\
  register T* tv  = (this->v);							\
  register T* ov  = other.v;						\
  register T* rv  = result.v;						\
  register int ts = skip;						\
  register int os = other.skip;						\
									\
  while( tl-- )								\
  {									\
    *rv++ = *tv op *ov;							\
    tv += ts;								\
    ov += os;								\
  }									\
  return result;							\
}

defop(+)
defop(-)

#undef defop // (op)


template <class T>
inline IP_Vector<T>&
IP_Vector<T>::abs()
{
  register int           tl  = len      ;
  register T         *tv  = this->v        ;
  register int        ts  = skip     ;

  while( tl-- )
  {
    if ( *tv <= T(0) )
      *tv = -*tv;
    tv += ts;
  }
  return (*this) ;
}

template <class T>
inline IP_Vector<T> IP_Vector<T>::
exponential() const
{
	IP_Vector<T>        result( len )  ;

#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)

 cerr << "Instatiation of exp didn't take place because exp(complex&) does not exist\n";
 return  result;
#else

  register int           tl  = len      ;
  register T         *tv  = (this->v)        ;
  register T         *rv  = result.v ;
  register int ts = skip;

  while( tl-- )
  {
    *rv++ = (T) exp(*tv);
    tv += ts;
  }
  return result;
#endif
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::sqrtize()
{
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
	cerr << "sqrt is not instantiated properly for cmplx data type\n";
#else
  register int   tl  = len      ;
  register T  *tv = (this->v)        ;
  register int ts = skip     ;

  while( tl-- )
  {
    *tv = (T) ::sqrt(*tv);
    tv += ts;
  }
#endif
  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::sqr()
{
  register int   tl  = len      ;
  register T  *tv = (this->v)        ;
  register int ts = skip     ;

  while( tl-- )
  {
    *tv *= (*tv) ;
    tv += ts;
  }
  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::pow( int exponent )
{
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
	cerr << "pow is not instantiated properly for cmplx daa type\n";
#else
  register int   tl  = len      ;
  register T  *tv = (this->v)        ;
  register int ts = skip     ;

  while( tl-- )
  {
    //*tv *= (T) ::pow( *tv, (CMPLX_FLOAT_TYPE)exponent );
    *tv *= (T) ::pow( *tv, exponent );
    tv += ts;
  }
#endif
  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::pow( double exponent )
{
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
	cerr << "pow is not instantiated properly for cmplx daa type\n";
#else
  register  int  tl  = len      ;
  register T  *tv = (this->v)        ;
  register int ts = skip     ;

  while( tl-- )
  {
    *tv =  (T) ::pow( (*tv), (float)exponent );
    tv += ts;
  }
#endif
  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::invert( T eps )
{
#if defined(_WIN32) && defined(CMPLX_INSTANTIATION)
	cerr << "sqrt is not instantiated properly for cmplx data type\n";
#else
  register int   tl  = len      ;
  register T  *tv = (this->v)        ;
  register int ts = skip     ;

  if( eps == T(0) )
  {
    while( tl-- )
    {
      *tv = T(1. / *tv);
      tv += ts;
    }
  }
  else
  {
    while( tl-- )
    {
      *tv = T(1. / (*tv + eps));
      tv += ts;
    }
  }
#endif
  return *this;
}

// =====================================================================

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::differentiate( T last )
{
  register T tmp;
  register int tl	= len;
  register T* tv	= (this->v);
  register int ts	= skip;

  while( tl-- )
  {
    tmp  = *tv - last;
    last = *tv;
    *tv  = tmp;
    tv  += ts;
  }
  return *this;
}

// =====================================================================

template <class T>
inline IP_Vector<T> IP_Vector<T>::
pointMult( const IP_Vector<T>& other ) const
{
  IP_Vector<T> result ( len )  ;
  result.pointMultAssign(*this, other);
  return result;
}

// =====================================================================

template <class T>
inline IP_Vector<T>& IP_Vector<T>::
pointMultAssign( const IP_Vector<T>& v1, const IP_Vector<T>& v2 )
{

  register T  *vv  = (this->v);
  register T  *ov1 = v1.v;
  register T  *ov2 = v2.v;

  register int l0  = len;

  register int vs  = skip;
  register int os1 = v1.skip;
  register int os2 = v2.skip;

  while( l0-- )
  {
    *vv  = (*ov1) * (*ov2);
    vv  += vs;
    ov1 += os1;
    ov2 += os2;
  }
  return *this;
}

// =====================================================================

template <class T>
inline IP_Vector<T>& IP_Vector<T>::
pointMultAssign( const IP_Vector<T>& other )
{
  register T        *vv	= (this->v)         ;
  register T        *ov = other.v   ;

  register int l0 	= len       ;

  register int vs   	= skip       ;
  register int os  	= other.skip ;

  while( l0-- )
  {
    *vv = *vv * *ov;
    ov += os;
    vv += vs;
  }
  return *this;
}

// =====================================================================

template <class T>
inline IP_Vector<T> IP_Vector<T>::
pointDiv( const IP_Vector<T>& other ) const
{

  IP_Vector<T> result ( len )  ;

  register T        *vv	= (this->v)         ;
  register T        *ov = other.v   ;
  register T        *rv = result.v  ;

  register int l0 	= len       ;

  register int vs   	= skip       ;
  register int os  	= other.skip ;

  while( l0-- )
  {
    *rv = *vv / *ov;
    rv++;
    ov += os;
    vv += vs;
  }
  return result;
}

// =====================================================================

template <class T>
inline IP_Vector<T>& IP_Vector<T>::
pointDivAssign( const IP_Vector<T>& other )
{

  IP_Vector<T> result ( len )  ;

  register T        *vv	= (this->v)         ;
  register T        *ov = other.v   ;

  register int l0 	= len       ;

  register int vs   	= skip       ;
  register int os  	= other.skip ;

  while( l0-- )
  {
    *vv = *vv / *ov;
    ov += os;
    vv += vs;
  }
  return *this;
}

// =====
// norms
// =====

// =====================================================================

template <class T>
inline double
IP_Vector<T>::p1norm() const
{
  double result = 0;

  register int tl	= len;
  register T* tv	= (this->v);
  register int ts	= skip;

  while( tl-- )
  {
    result += (double) IP_abs(*tv);
    tv += ts;
  }
  return result;
}

// =====================================================================

template <class T>
inline double
IP_Vector<T>::p2norm() const
{
  return ::sqrt( (double) p2sqr() );
}

// =====================================================================

template <class T>
inline double
IP_Vector<T>::p2sqr() const
{
  double result = 0;

  register int tl	= len;
  register T* tv	= (this->v);
  register int ts	= skip;

  while( tl-- )
  {
    result += *tv * *tv;
    tv += ts;
  }
  return result;
}

// =====================================================================

template <class T>
inline T
IP_Vector<T>::sum() const
{
  T result = 0;

  register int tl	= len;
  register T* tv	= (this->v);
  register int ts	= skip;

  while( tl-- )
  {
    result += *tv;
    tv += ts;
  }
  return result;
}

template <class T>
inline T
IP_Vector<T>::product() const
{
  T result = 1;

  register int tl	= len;
  register T* tv	= (this->v);
  register int ts	= skip;

  while( tl-- )
  {
    result *= *tv;
    tv += ts;
  }
  return result;
}

// =====================================================================

template <class T>
inline T
IP_Vector<T>::mean() const
{
  return sum()/(T)len;
}

template <class T>
inline T
IP_Vector<T>::variance( T mean ) const
{
  return (T)(((*this)-mean).p2sqr()/double(len));
}

// =====================================================================

template <class T>
inline double
IP_Vector<T>::meanSqr(bool global) const
{
  double result;

  if(global)
    result=p2sqr()/double(len);
  else
  {
    result=0;
    double c=len;

    register int tl	= len;
    register T* tv	= (this->v);
    register int ts	= skip;

    while( tl-- )
    {
      result += (*tv * *tv)/c;
      tv += ts;
    }
  }
  return result;
}

// =====================================================================

template <class T>
inline double
IP_Vector<T>::rms() const
{
  return ::sqrt( p2sqr()/double(len));
}

// =====================================================================

template <class T>
inline T
IP_Vector<T>::p2sqr_dist( const IP_Vector<T>& other ) const
{

  T result = 0;
  T tmp;
  register int tl 		= len;
  register T* tv	= (this->v);
  register T* ov   	= other.v;
  register int ts 	= skip;
  register int os 	= other.skip;

  while( tl-- )
  {
    tmp = *tv - *ov;
    result += tmp * tmp;
    tv += ts;
    ov += os;
  }
  return result;
}


template <class T>
inline T
IP_Vector<T>::p2sqrt_dist( const IP_Vector<T>& other ) const
{

  T result = 0.0;

  T tmp;
  register int tl 		= len;
  register T* tv	= (this->v);
  register T* ov   	= other.v;
  register int ts 	= skip;
  register int os 	= other.skip;

  while( tl-- )
  {
    tmp = *tv - *ov;
    result += tmp * tmp;
    tv += ts;
    ov += os;
  }

  return sqrt(result);
}


#ifndef CMPLX_INSTANTIATION
template <class T>
inline T
IP_Vector<T>::p1dist( const IP_Vector<T>& other ) const

{

  T tmp, result=0;

  return result;
}

#endif

// =====================================================================
// maximum
// =====================================================================

template <class T>
inline T& IP_Vector<T>::max()
{
  return (this->v)[seek(argMax())];
}
template <class T>
inline T IP_Vector<T>::max() const
{
  return (this->v)[seek(argMax())];
}

template <class T>
inline int IP_Vector<T>::argMax() const
{
  int result = 0 ;

  T maximum = *(this->v);

  register T *tv 	= &(this->v)[seek(len-1)];
  register int ts 	= skip ;
  register int tl0 	= len ;

  while( tl0-- )
  {
    if( maximum<*tv )
      {
	result = tl0  ;
	maximum = *tv ;
      }
    tv -= ts;
  }
  return result;
}



// =====================================================================
// minimum
// =====================================================================

template <class T>
inline T& IP_Vector<T>::min()
{
  return (this->v)[seek(argMin())];
}
template <class T>
inline T
IP_Vector<T>::min() const
{
  return (this->v)[seek(argMin())];
}

template <class T>
inline int IP_Vector<T>::argMin() const
{
  int result = 0 ;

  T minimum = *(this->v);


  register T *tv 	= &(this->v)[seek(len-1)];
  register int ts 	= skip ;
  register int tl0 	= len ;

  while( tl0-- )
  {
    if( minimum>*tv )
      {
	result = tl0  ;
	minimum = *tv ;
      }
    tv -= ts;
  }

  return result;
}


// =====================================================================
// maximum of absolute
// =====================================================================

template <class T>
inline T IP_Vector<T>::maxAbs() const
{
  T max=(this->v)[argMaxAbs()*skip];
  return MAX(max,-max);
}

template <class T>
inline int IP_Vector<T>::argMaxAbs() const
{
  int result = 0 ;

  T maximum = *(this->v);

  register T  *tv 	= &(this->v)[seek(len-1)];
  register T   tmp ;
  register int ts 	= skip ;
  register int tl0 	= len ;

  while( tl0-- )
  {
    tmp = MAX( *tv, -(*tv) );
    if( maximum < tmp )
      {
	result = tl0  ;
	maximum = tmp ;
      }
    tv -= ts;
  }
  return result;
}

// =====================================================================
// minimum of absolute
// =====================================================================

template <class T>
inline T IP_Vector<T>::minAbs() const
{
  T min=(this->v)[argMinAbs()*skip];
  return MAX(min,-min);
}

template <class T>
inline int IP_Vector<T>::argMinAbs() const
{
  int result = 0 ;

  T minimum = *(this->v);

  register T  *tv 	= &(this->v)[seek(len-1)];
  register T   tmp ;
  register int ts 	= skip ;
  register int tl0 	= len ;

  while( tl0-- )
  {
    tmp = MAX( *tv, -(*tv) );
    if( tmp < minimum )
      {
	result  = tl0 ;
	minimum = tmp ;
      }
    tv -= ts;
  }

  return result;
}

/*/ =====================================================================\
// thresholding
// =====================================================================\
*/

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::setMax( T theta )
{
  register int tl	= len;
  register T* tv 	= (this->v);
  register int ts	= skip;
  while( tl-- )
  {
    *tv = ( *tv<theta ) ? *tv : theta;
    tv += ts;
  }
  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::setMin( T theta )
{
  register int tl	= len;
  register T* tv 	= (this->v);
  register int ts	= skip;
  while( tl-- )
  {
    *tv = ( *tv>theta ) ? *tv : theta;
    tv += ts;
  }
  return *this;
}

// =========
// utilities
// =========

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::apply( T (*function)(T) )
{
  register int tl	= len;
  register T* tv	= (this->v);
  register int ts	= skip;

  while( tl-- )
  {
    *tv = (*function)(*tv);
    tv += ts;
  }
  return *this;
}

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::apply( T (*function)(T,void*), void* data )
{
  register int tl	= len;
  register T* tv	= (this->v);
  register int ts	= skip;

  while( tl-- )
  {
    *tv = (*function)(*tv,data);
    tv += ts;
  }
  return *this;
}

// =====================================================================

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::canonic( int i )
{

  if(i<0) i=len+i;
  *this=T(0);
  (this->v)[seek(i)]=1;

  return *this;
}

// =====================================================================

template <class T>
inline IP_Vector<T>&
IP_Vector<T>::reverse()
{
  register T		tmp;
  register int tl	= len/2;
  register T* lv 	= (this->v);
  register T* rv 	= elementPtr(len-1);
  register int ts	= skip;

  while( tl-- )
  {
    tmp = *lv;
    *lv = *rv;
    *rv = tmp;
    lv += ts;
    rv -= ts;
  }
  return *this;
}


template <class T>
inline void
IP_Vector<T>::bubble_sort()
{
	int flag=1;
	T* t=(this->v);
	while (flag) {
		(this->v)=t;
		flag=0;
		for (int i=0; i<len-1; i++) {
			if (*(this->v)>*((this->v)+1)) {
				flag=1;
				T tmp=*(this->v);
				*(this->v)=*((this->v)+1);
				*((this->v)+1)=tmp;
			}
			(this->v)++;
		}
	}
	(this->v)=t;
}


template <class T>
inline void
IP_Vector<T>::quick_sort()
{
	;
}


template <class T>
inline void
IP_Vector<T>::merge_sort()
{
	mg_sort(0, len);

}


template <class T>
inline void
IP_Vector<T>::mg_sort(int b, int e)
{
	int i, j, k, m, t;
	T* tem = new T [len];

	if (e > b) {
		m = (b+e)/2;
		t = m+e+1;
		mg_sort(b, m);
		mg_sort(m+1, e);

		for (i=m; i>=b; i--) tem[i]=*((this->v)+i);
		i++;
		for (j=m+1; j<=e; j++) tem[t-j]=*((this->v)+j);
		j--;
		for (k=b; k<=e; k++) *((this->v)+k) = (tem[i]<tem[j]) ? tem[i++]: tem[j--];
	}
	delete tem;
}


template <class T>
inline IP_Vector<T>
IP_Vector<T>::right_merge(const IP_Vector<T>& other)
{
	IP_Vector<T> result(len+other.len);

    register T* tv 	= result.v;
    register int ts    	= result.skip;

	register int tl	= len;
    register T* ov 	= (this->v);
    register int os   	= skip;
    while( tl-- ) {
        *tv = *ov;
        tv += ts;
        ov += os;
	}

	tl	=other.len;
	ov 	= other.v;
    os   	= other.skip;
    while( tl-- ) {
        *tv = *ov;
        tv += ts;
        ov += os;
	}

	//cout<<result<<endl;
	return result;
}
// ===========
// other stuff
// ===========

template <class T>
inline T*
IP_Vector<T>::nr()
{
  return (this->v)-1;
}

#endif // IP_VECTOR_HH
