#ifndef __onika_valuetraits_h
#define __onika_valuetraits_h

#include <limits>
#include <cstdint>
#include <tuple>

namespace onika {


// numeric_limits doesn't provide real minimum value in case of denormalized floating point type.
// this template aleviates this caveat providing mathematical minimum representable value of a type.
template<class value_type>
struct value_traits
{
	static inline value_type min()
	{
		return ( -std::numeric_limits<value_type>::max() < std::numeric_limits<value_type>::min() ) ?
			 -std::numeric_limits<value_type>::max()
			: std::numeric_limits<value_type>::min();
	}
	static inline value_type max() { return std::numeric_limits<value_type>::max(); }
};

template<class... T>
struct value_traits< std::tuple<T...> >
{
	static inline std::tuple<T...> min() { return std::tuple<T...>( onika::value_traits<T>::min()... ); }
	static inline std::tuple<T...> max() { return std::tuple<T...>( onika::value_traits<T>::max()... ); }
};


} // end of namespace

#endif

