#ifndef __onika_container_containertraits_h
#define __onika_container_containertraits_h

#include "onika/codec/types.h"
#include "onika/valuetraits.h"
#include "onika/codec/encoding.h"
#include "onika/language.h"
#include <algorithm>

#warning DEPRECATED, use container/algorithm.h instead

namespace onika { namespace container {


#define DECL_AUTO_RET(expr) -> decltype(expr) { return expr; }

// default traits for external containers
template<typename _Container>
struct container_traits
{
	typedef _Container Container;
	typedef typename Container::value_type value_type;
	typedef typename language::MakeSigned<value_type>::type signed_value_type;

	static inline value_type min_element(const Container& c) { return * std::min_element(c.begin(),c.end()); }
	static inline value_type max_element(const Container& c) { return * std::max_element(c.begin(),c.end()); }
	static inline value_type min_value(const Container& c) { return value_traits<value_type>::min(); }
	static inline value_type max_value(const Container& c) { return value_traits<value_type>::max(); }
	static inline size_t size(const Container& c) { return c.size(); }
	static inline size_t memory_bytes(const Container& c)
	{
		return sizeof(Container) + sizeof(value_type) * size(c);
	}
	static inline codec::BoundedValue<int> meta_data(const Container& c)
	{
		return codec::bounded_value(0,0,0); // no information needed
	}
	static inline codec::BoundedValue<signed_value_type> wavelet_enc(Container& c,size_t a,size_t b)
	{
			value_type Min = min_element(c);
			value_type Max = max_element(c);
			value_type va = c[a];
			value_type vb = c[b];
			value_type avg;
			signed_value_type delta;
			codec::wavelet_enc(va,vb,avg,delta);
			c[a] = avg;
			return codec::bounded_delta(avg,delta,Min,Max);
	}
	static inline codec::BoundedValue<value_type> value_enc(const Container& c,size_t i)
	{
			value_type Min = min_element(c);
			value_type Max = max_element(c);
			return bounded_value_enc(c,i,Min,Max);
	}
	static inline codec::BoundedValue<value_type> bounded_value_enc(const Container& c,size_t i,value_type Min,value_type Max)
	{
			value_type v = c[i];
			return codec::bounded_value(v,Min,Max);
	}
	static inline codec::BoundedValue<signed_value_type> delta_enc(const Container& c,size_t a, size_t b)
	{
			value_type Min = min_element(c);
			value_type Max = max_element(c);
			return bounded_delta_enc(c,a,b,Min,Max);
	}
	static inline codec::BoundedValue<signed_value_type>
	bounded_delta_enc(const Container& c,size_t a, size_t b,value_type Min,value_type Max) 
	{
			value_type va = c[a];
			value_type vb = c[b];
			value_type delta = vb - va;
			return codec::bounded_delta(va,delta,Min,Max);
	}
};


// traits for home grown containers
template<typename _Container>
struct container_traits_base
{
	typedef _Container Container;
	typedef typename Container::value_type value_type;
	typedef typename value_traits<value_type>::signed_type signed_value_type;

	static inline auto min_element(const Container& c) DECL_AUTO_RET( c.min_element() )
	static inline auto max_element(const Container& c) DECL_AUTO_RET( c.max_element() )
	static inline auto min_value(const Container& c) DECL_AUTO_RET( c.min_value() )
	static inline auto max_value(const Container& c) DECL_AUTO_RET( c.max_value() )
	static inline auto memory_bytes(const Container& c) DECL_AUTO_RET( c.memory_bytes() )
	static inline auto meta_data(const Container& c) DECL_AUTO_RET( c.meta_data() )
	static inline auto value_enc(const Container& c,size_t i) DECL_AUTO_RET( c.value_enc(i) )
	static inline auto bounded_value_enc(const Container& c,size_t i,value_type Min,value_type Max) DECL_AUTO_RET( c.bounded_value_enc(i,Min,Max) )
	static inline auto delta_enc(const Container& c,size_t a, size_t b) DECL_AUTO_RET( c.delta_enc(a,b) )
	static inline auto bounded_delta_enc(const Container& c,size_t a, size_t b,value_type Min,value_type Max)
	DECL_AUTO_RET( c.bounded_delta_enc(a,b,Min,Max) )

	static inline auto wavelet_enc(Container& c,size_t a,size_t b) DECL_AUTO_RET( c.wavelet_enc(a,b) )
};

template<typename C>
static inline auto min_element(const C& c)
DECL_AUTO_RET(container_traits<C>::min_element(c))

template<typename C>
static inline auto max_element(const C& c) 
DECL_AUTO_RET(container_traits<C>::max_element(c))

template<typename C>
static inline auto min_value(const C& c) 
DECL_AUTO_RET(container_traits<C>::min_value(c))

template<typename C>
static inline auto max_value(const C& c) 
DECL_AUTO_RET(container_traits<C>::max_value(c))

template<typename C>
static inline auto memory_bytes(const C& c) 
DECL_AUTO_RET(container_traits<C>::memory_bytes(c))

template<typename C>
static inline auto meta_data(const C& c) 
DECL_AUTO_RET(container_traits<C>::meta_data(c))

template<typename C>
static inline auto value_enc(const C& c,size_t i) 
DECL_AUTO_RET(container_traits<C>::value_enc(c,i))

template<typename C, typename value_type>
static inline auto bounded_value_enc(const C& c,size_t i,value_type Min,value_type Max)
DECL_AUTO_RET(container_traits<C>::bounded_value_enc(c,i,Min,Max))

template<typename C> 
static inline auto delta_enc(const C& c,size_t a, size_t b) 
DECL_AUTO_RET(container_traits<C>::delta_enc(c,a,b))

template<typename C, typename value_type> 
static inline auto bounded_delta_enc(const C& c,size_t a, size_t b,value_type Min,value_type Max)
DECL_AUTO_RET(container_traits<C>::bounded_delta_enc(c,a,b,Min,Max))

template<typename C>
static inline auto wavelet_enc(C& c,size_t a,size_t b)
DECL_AUTO_RET(container_traits<C>::wavelet_enc(c,a,b))


#undef DECL_AUTO_RET


} } // end of namespace

#endif


