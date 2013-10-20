#ifndef __onika_container_tuplevec_h
#define __onika_container_tuplevec_h

#include <iterator>
#include "onika/container/iterator.h"
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


template<class... Types>
inline auto zip_vectors( Types&... t ) -> TupleVec< Types... >
{
	return TupleVec< Types... > ( t... );
}


} } // end of namespace onika::container




// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#ifdef onika_container_tuplevec_TEST

#include <iostream>
#include <vector>
#include <tuple>
#include <utility>

template<class... T>
inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
{
	onika::tuple::print( out, t );
	return out;
}

template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ConstElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}

template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}

int main()
{
	std::vector<int> xvalues(10);
	std::vector<double> yvalues(10);
	std::vector<float> zvalues(10);

	std::vector<std::string> avalues(10);
	std::vector<char> bvalues(10);
	const char* txt[] = { "Un", "Deux", "Trois", "Quatre", "Cinq", "Six", "Sept", "Huit", "Neuf", "Dix" };

	for(int i=0;i<10;i++)
	{
		xvalues[i] = i;
		yvalues[i] = 1.45676546546*i;
		zvalues[i] = 1.625*i;
		avalues[i] = txt[i];
		bvalues[i] = 'A'+i;
	}

	auto c1 = onika::container::zip_vectors( xvalues, yvalues, zvalues );
	c1.resize( 9 );
	c1.push_back( std::make_tuple(9, 3.14159, 0.5) );
	c1.resize( 12 , std::make_tuple(12, 1.11, 0.1) );
	for( auto x : c1 ) { std::cout<< x <<"\n"; }
		
	auto c2 = onika::container::zip_vectors( avalues, bvalues );
	c2.resize( 15, std::make_tuple("Default",'Z') );
	for( auto x : c2 ) { std::cout<< x <<"\n"; }

	return 0;
}
#endif // end of unit test


#endif // end of file tuplevec.h


