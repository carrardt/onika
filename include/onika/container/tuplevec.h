#ifndef __onika_container_tuplevec_h
#define __onika_container_tuplevec_h

#include <iterator>
#include "onika/container/iterator.h"
#include "onika/container/algorithm.h"
#include "onika/language.h"
#include "onika/tuple.h"

namespace onika { namespace container {

struct ContainerIthElementOp
{
	inline ContainerIthElementOp(size_t i) : m_idx(i) {}
	template<class C> inline typename C::value_type operator () ( const C & c ) const
	{
		return static_cast< typename C::value_type >( c[m_idx] );
	}
	size_t m_idx;
};

// TupleVec Helper class
template<unsigned int N>
struct TupleVecHelper
{
	template<class ContainerTuple, class ValueTuple>
	static inline void set( ContainerTuple& tv, const size_t i, const ValueTuple& x )
	{
		TupleVecHelper<N-1>::set( tv, i, x );
		std::get<N-1>( tv ) [ i ] = std::get<N-1>( x );
	}
	
	template<class ContainerTuple, class ValueTuple>
	static inline void resize( ContainerTuple& tv, const size_t n, const ValueTuple& x )
	{
		TupleVecHelper<N-1>::resize( tv, n, x );
		std::get<N-1>( tv ).resize( n, std::get<N-1>( x ) );
	}
	
	template<class ContainerTuple>
	static inline void resize( ContainerTuple& tv, const size_t n )
	{
		TupleVecHelper<N-1>::resize( tv, n );
		std::get<N-1>( tv ).resize( n );
	}
	
	template<class ContainerTuple>
	static inline size_t size( const ContainerTuple& tv )
	{
		return std::min( TupleVecHelper<N-1>::size( tv ) , std::get<N-1>( tv ).size() );
	}

};
template<>
struct TupleVecHelper<1>
{
	template<class ContainerTuple, class ValueTuple>
	static inline void set( ContainerTuple& tv, const size_t i, const ValueTuple& x )
	{
		std::get<0>( tv ) [ i ] = std::get<0>( x );
	}
	
	template<class ContainerTuple, class ValueTuple>
	static inline void resize( ContainerTuple& tv, const size_t n, const ValueTuple& x )
	{
		std::get<0>( tv ).resize( n, std::get<0>( x ) );
	}
	
	template<class ContainerTuple>
	static inline void resize( ContainerTuple& tv, const size_t n )
	{
		std::get<0>( tv ).resize( n );
	}
	
	template<class ContainerTuple>
	static inline size_t size( const ContainerTuple& tv )
	{
		return std::get<0>( tv ).size();
	}
};



template<class... Types>
struct TupleVec
{
	// tuple of references to containers
	typedef std::tuple<Types&...> VecTuple;

	// number of containers
	static constexpr unsigned int NComp = sizeof...(Types);

	// value returned for each element is a tuple of Ti::value_type for each Ti in Types, e.g. a tuple of types returned by each container
	typedef std::tuple< typename Types::value_type ... > ElementTuple;

	// constructors
	inline TupleVec( Types&... refsToContainers ) : m_vecTuple( refsToContainers... )
	{
		// dbg_assert( all_equal_size );
	}

	// generic container API
	ONIKA_CONTAINER_ACCESS_DEF(TupleVec,ElementTuple);
	ONIKA_CONTAINER_PUSH_BACK_DEF(TupleVec,ElementTuple);

	inline void resize(size_t n)
	{
		TupleVecHelper<NComp>::resize(m_vecTuple,n); 
	}

	inline void resize(size_t n, const value_type& val )
	{
		TupleVecHelper<NComp>::resize(m_vecTuple,n,val);
	}

	inline size_t size() const
	{
		return TupleVecHelper<NComp>::size(m_vecTuple);
	}

	inline void set(size_t i, const value_type& val)
	{
		TupleVecHelper<NComp>::set( m_vecTuple , i , val );
	}

	inline value_type get(size_t i) const
	{
		return tuple::map( m_vecTuple , ContainerIthElementOp(i) );
	}

	// references to containers stored as a tuple of references
	VecTuple m_vecTuple;
};

// variant where zipped containers are copied and not referenced
template<class... Types>
struct TupleVecCpy
{
	// tuple of references to containers
	typedef std::tuple<Types...> VecTuple;

	// number of containers
	static constexpr unsigned int NComp = sizeof...(Types);

	// value returned for each element is a tuple of Ti::value_type for each Ti in Types, e.g. a tuple of types returned by each container
	typedef std::tuple< typename Types::value_type ... > ElementTuple;

	// constructors
	inline TupleVecCpy() { }
	inline TupleVecCpy( const Types&... constRefsToContainers ) : m_vecTuple( constRefsToContainers... )
	{
		// dbg_assert( all_equal_size );
	}

	// generic container API
	ONIKA_CONTAINER_ACCESS_DEF(TupleVecCpy,ElementTuple);
	ONIKA_CONTAINER_PUSH_BACK_DEF(TupleVecCpy,ElementTuple);

	inline void resize(size_t n)
	{
		TupleVecHelper<NComp>::resize(m_vecTuple,n);
	}

	inline void resize(size_t n, const value_type& val )
	{
		TupleVecHelper<NComp>::resize(m_vecTuple,n,val);
	}

	inline size_t size() const
	{
		return TupleVecHelper<NComp>::size(m_vecTuple);
	}

	inline void set(size_t i, const value_type& val)
	{
		TupleVecHelper<NComp>::set( m_vecTuple , i , val );
	}

	inline value_type get(size_t i) const
	{
		return tuple::map( m_vecTuple , ContainerIthElementOp(i) );
	}

	// references to containers stored as a tuple of references
	VecTuple m_vecTuple;
};


template<class... Types>
inline auto zip_vectors( Types&... t ) -> TupleVec< Types... >
{
	return TupleVec< Types... > ( t... );
}

template<class... Types>
inline auto zip_vectors_cpy( const Types&... t ) -> TupleVecCpy< Types... >
{
	return TupleVecCpy< Types... > ( t... );
}


// ==========================================================
// ==== algorithm specialization                          ===
// ==========================================================
#include "onika/container/algorithm.h"
// min value
template<class... Types>
inline auto min_value(const TupleVecCpy<Types...>& c) 
ONIKA_AUTO_RET( onika::tuple::map( c.m_vecTuple, MinValueOp() ) ) 
template<class... Types>
inline auto min_value(const TupleVec<Types...>& c) 
ONIKA_AUTO_RET( onika::tuple::map( c.m_vecTuple, MinValueOp() ) ) 

// max value
template<class... Types>
inline auto max_value(const TupleVecCpy<Types...>& c) 
ONIKA_AUTO_RET( onika::tuple::map( c.m_vecTuple, MaxValueOp() ) ) 
template<class... Types>
inline auto max_value(const TupleVec<Types...>& c) 
ONIKA_AUTO_RET( onika::tuple::map( c.m_vecTuple, MaxValueOp() ) ) 

// single element value encoding
template<class... Types>
inline auto value_enc(const TupleVecCpy<Types...>& c, size_t i)
ONIKA_AUTO_RET( onika::tuple::map( c.m_vecTuple, ValueEncOp(i) ) )
template<class... Types>
inline auto value_enc(const TupleVec<Types...>& c, size_t i)
ONIKA_AUTO_RET( onika::tuple::map( c.m_vecTuple, ValueEncOp(i) ) )

// element pair difference encoding
template<class... Types>
inline auto delta_enc(const TupleVecCpy<Types...>& c, size_t i, size_t j)
ONIKA_AUTO_RET( onika::tuple::map( c.m_vecTuple, DeltaEncOp(i,j) ) )
template<class... Types>
inline auto delta_enc(const TupleVec<Types...>& c, size_t i, size_t j)
ONIKA_AUTO_RET( onika::tuple::map( c.m_vecTuple, DeltaEncOp(i,j) ) )

// total size of container
template<class... Types>
inline auto memory_bytes(const TupleVecCpy<Types...>& c) 
ONIKA_AUTO_RET(
	onika::tuple::reduce( 
		onika::tuple::map(c.m_vecTuple,MemoryBytesOp()),
		onika::math::add_op()
		)
	)

template<class... Types>
inline auto memory_bytes(const TupleVec<Types...>& c) 
ONIKA_AUTO_RET( sizeof(c) + /* add reference vector to total size */
	onika::tuple::reduce( 
		onika::tuple::map(c.m_vecTuple,MemoryBytesOp()),
		onika::math::add_op()
		)
	)

} } // end of namespace onika::container



#endif // end of file tuplevec.h


