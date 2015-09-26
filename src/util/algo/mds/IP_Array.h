//
// classes	: IP_ArrayHandle
//		  IP_Array
//
// description	: IP_ArrayHandle is a small struct actually
//		  holding the array memory, while IP_Array is the constitutes
//		  the base of all array classes and provides all methods for
//		  memory management.
//
#ifndef IP_ARRAY_HH
#define IP_ARRAY_HH

#ifdef __GNUC__
#pragma interface
#endif

#ifdef SGI
#include <stdlib.h>
#endif

#include <string.h>
#include <math.h>

template <class T>
struct IP_ArrayHandle
{
  inline IP_ArrayHandle( int len );
  inline IP_ArrayHandle( const IP_ArrayHandle<T>& other );
  inline IP_ArrayHandle( T* m );
  inline ~IP_ArrayHandle();

  T  	*mem;
  int	count;
};

template <class T>
class IP_Array
{
public:
  // construction/destruction
  IP_Array( int l=0 );
  IP_Array( const IP_Array<T>& parent, T* mem=NULL );
  virtual ~IP_Array();


  // init array with memory allocated elsewhere
  IP_Array&     	init	( T* mem, bool handleMem=false );

  // assign array to memory of other
  IP_Array&     	assign	( const IP_Array<T>& other );

  // resize without copy
  IP_Array&     	size	( int l );

  // resize with memcpy
  IP_Array&     	resize	( int l_old, int l_new );

  // returns pointer to internal array
  inline T*	       	memory();
  inline const T*      	memory() const;

  // Hack: assign internal c array given memory
  T*			memory( T* m );

  // Hack: release internal memory from automatic memory handling
  T*			release();

  // handle holding actual memory
  IP_ArrayHandle<T> 	*ref;

  // pointer to v(ector) start (not necessarily identical with ref->mem)
  T			*v;

};

template <class T>
inline IP_ArrayHandle<T>::IP_ArrayHandle( int length ) :
  mem( (T*) (length ? new T [length] : NULL )), count(1)
{
  if ( !(length==0 || mem) )
    {
//    fatal( "IP_ArrayHandle - out of memory\n" );
//    exit(1);
	  ;
    }
}

template <class T>
inline IP_ArrayHandle<T>::IP_ArrayHandle( const IP_ArrayHandle<T>& other ) :
  mem(other.mem), count(1)
{}


template <class T>
inline IP_ArrayHandle<T>::IP_ArrayHandle( T* m ) :
  mem(m), count(1)
{}

template <class T>
inline IP_ArrayHandle<T>::~IP_ArrayHandle()
{
  if(mem) delete (T*) mem;
}

template <class T>
inline T*
IP_Array<T>::memory()
{ return v; }

template <class T>
inline const T*
IP_Array<T>::memory() const
{ return v; }




#include <memory.h>

template <class T>
inline IP_Array<T>::IP_Array( int l ) :
  ref( new IP_ArrayHandle<T>(l) ),
  v( ref->mem )
{}

template <class T>
inline IP_Array<T>::IP_Array( const IP_Array<T>& parent, T* mem ) :
  ref( parent.ref ),
  v( mem ? mem : parent.v )
{ ref->count++; }

template <class T>
inline IP_Array<T>::~IP_Array()
{
  if( --ref->count == 0 ) delete ref;
}

template<class T>
inline IP_Array<T>&
IP_Array<T>::init( T* mem, bool handleMem )
{
  if( ref->count>1 || ref->mem!=NULL )
  {
    if( --ref->count == 0 ) delete ref;
    ref = new IP_ArrayHandle<T>( handleMem ? mem : ((T *) NULL) );
  }
  v = mem;
  return *this;
}

template <class T>
inline IP_Array<T>&
IP_Array<T>::assign( const IP_Array<T>& other )
{
  if( ref != other.ref )
  {
    if( --ref->count == 0 ) delete ref;
    ref = other.ref;
    ref->count++;
  }
  v = other.v;

  return *this;
}

template <class T>
inline IP_Array<T>&
IP_Array<T>::size( int l )
{
  if( --ref->count == 0 ) delete ref;

  ref = new IP_ArrayHandle<T>( l );

  v   = ref->mem;

  return *this;
}

template <class T>
inline IP_Array<T>&
IP_Array<T>::resize( int l_old, int l_new )
{
  IP_ArrayHandle<T> *nref = new IP_ArrayHandle<T>( l_new );
  REQUIRE1( nref, "IP_Array<%s>::resize() - out of memory", "elementType()");

  memcpy( nref->mem, v, ( l_old<l_new ? l_old:l_new )*sizeof(T) );
  if( l_new>l_old ) memset( &(nref->mem[l_old]), '\0', (l_new-l_old)*sizeof(T) );

  if( --ref->count == 0 ) delete ref;

  ref = nref;
  v   = nref->mem;

  return *this;
}

template <class T>
inline T*
IP_Array<T>::memory( T* newmem )
{
  if( ref->count != 1 || ref->mem != NULL )
  {
    //
    // need to allocate a new array Handle
    //
    if( --ref->count == 0 ) delete ref;
    ref = new IP_ArrayHandle<T>( NULL );
  }
  return v = newmem;
}

// HACK!

template <class T>
inline T*
IP_Array<T>::release()
{ ref->count++; return v; }




#endif // IP_ARRAY_HH






