#ifndef __onika_container_algorithm_h
#define __onika_container_algorithm_h

#include <limits>
#include <stdlib.h>
#include "onika/language.h"
#include "onika/valuetraits.h"
#include "onika/mathfunc.h"

namespace onika { namespace container {

// lower representable value in container C
template<typename _C>
inline auto min_value(const _C& c) 
ONIKA_AUTO_RET( value_traits<typename _C::value_type>::min() )

struct MinValueOp
{
	template<class C> inline auto operator () (const C& c) const
	ONIKA_AUTO_RET( onika::container::min_value(c) )
};

template<typename _C>
inline auto max_value(const _C& c) 
ONIKA_AUTO_RET( value_traits<typename _C::value_type>::max() )

struct MaxValueOp
{
	template<class C> inline auto operator () (const C& c) const
	ONIKA_AUTO_RET( onika::container::max_value(c) )
};

template<class C>
inline auto value_enc(const C& c, size_t i)
ONIKA_AUTO_RET( static_cast<typename C::value_type>(c[i]) )

struct ValueEncOp
{
	size_t i;
	inline ValueEncOp(size_t _i) : i(_i) { }
	template<class C> inline auto operator () (const C& c) const
	ONIKA_AUTO_RET( onika::container::value_enc(c,i) )
};


template<class C>
inline auto delta_enc(const C& c, size_t i, size_t j)
ONIKA_AUTO_RET(
	math::sub(
		static_cast<typename C::value_type>(c[j]),
		static_cast<typename C::value_type>(c[i])
		)
	)

struct DeltaEncOp
{
	size_t i;
	inline DeltaEncOp(size_t _i) : i(_i) { }
	template<class C> inline auto operator () (const C& c) const
	ONIKA_AUTO_RET( onika::container::delta_enc(c,i) )
};

template<typename _C>
inline size_t memory_bytes(const _C& c)
{
	return sizeof(_C) + sizeof(typename _C::value_type) * c.size() ;
}

struct MemoryBytesOp
{
	template<class C> inline auto operator () (const C& c) const
	ONIKA_AUTO_RET( onika::container::memory_bytes(c) )
};


// null container
struct NullContainer
{
	inline NullContainer() : s(0) {}
	inline NullContainer(size_t n) : s(n) {}
	inline size_t size() const { return s; }
	inline void resize(size_t n) { s=n; }
	inline void pop_back() { --s; }
	inline int& operator [] (size_t i) { return x; }
	inline const int& operator [] (size_t i) const { return x; }
	size_t s;
	int x;
};


} } // end of namespace



// ======================== UNIT TEST =======================
#endif // end of algorithm.h

