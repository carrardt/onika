#ifndef __onika_container_zipvec_h
#define __onika_container_zipvec_h

#include <cstdint>
#include <stdlib.h>

#include "onika/debug/dbgassert.h"
#include "onika/container/iterator.h"
#include "onika/vec.h"
#include "onika/language.h"

namespace onika { namespace container {

template<typename _BaseContainer, unsigned int _N>
struct ZipVec
{
	enum { NComp = _N };
	typedef _BaseContainer BaseContainer;
	typedef typename BaseContainer::value_type ElementType;
	typedef Vec<NComp,ElementType> ZippedValueType;

	// generic container API
	ONIKA_CONTAINER_ACCESS_DEF(ZipVec,ZippedValueType);
	ONIKA_CONTAINER_PUSH_BACK_DEF(ZipVec,ZippedValueType);

	inline void resize(size_t n, const value_type& ival)
	{
		for(int c=0;c<NComp;++c) data[c].resize(n,ival.x[c]);
	}
	inline void resize(size_t n)
	{
		for(int c=0;c<NComp;++c) data[c].resize(n);
	}
	inline size_t size() const { return data[0].size(); }
	inline void set(size_t i, const value_type& val)
	{
		for(int c=0;c<NComp;++c) data[c][i] = val.x[c];
	}
	inline value_type get(size_t i) const
	{
		value_type r;
		for(int c=0;c<NComp;++c) r.x[c] = data[c][i];
		return r;
	}

	BaseContainer data[NComp];
};


} } // end of namespace




//==========================================
//=== container algorithm specialization ===
//==========================================
#include "onika/container/algorithm.h"
namespace onika { namespace container {

	template<typename _B, unsigned int _N> inline auto 
	min_value(const ZipVec<_B,_N>& c) -> typename ZipVec<_B,_N>::value_type
	{
		typename ZipVec<_B,_N>::value_type r;
		for(unsigned int i=0;i<_N;i++) r[i] = min_value( c.data[i] );
		return r;
	}

	template<typename _B, unsigned int _N> inline auto 
	max_value(const ZipVec<_B,_N>& c) -> typename ZipVec<_B,_N>::value_type
	{
		typename ZipVec<_B,_N>::value_type r;
		for(unsigned int i=0;i<_N;i++) r[i] = max_value( c.data[i] );
		return r;
	}

	template<typename _B, unsigned int _N> inline auto 
	memory_bytes(const ZipVec<_B,_N>& c) -> size_t
	{
		size_t m = 0;
		for(unsigned int i=0;i<_N;i++) m += memory_bytes(c.data[i]);
		return m;
	}

} }


//==========================================
//=== codec method specialization        ===
//==========================================
#include "onika/codec/types.h"
#include "onika/codec/encoding.h"
namespace onika { namespace codec {

	template<typename _B, unsigned int _N> inline auto 
	wavelet_enc(container::ZipVec<_B,_N>& c, size_t a,size_t b)
	-> Vec< _N , BoundedValue< ONIKA_MAKE_SIGNED(typename _B::value_type) > >
	{
		Vec< _N , BoundedValue< ONIKA_MAKE_SIGNED(typename _B::value_type) > > r;
		for(int i=0;i<_N;++i) { r[i] = wavelet_enc(c.data[i],a,b); }
		return r;
	}

} } // end of onika::codec



//==========================================
//=== STL algorithm specialization       ===
//==========================================
#include <algorithm>
namespace std
{
	// std::swap
	template<typename _B, unsigned int _N> inline
	void swap(
		onika::container::ElementAccessorT< onika::container::ZipVec<_B,_N> > accA,
		onika::container::ElementAccessorT< onika::container::ZipVec<_B,_N> > accB )
	{
		onika::container::ZipVec<_B,_N> & zvec = accA.v;
		onika::debug::dbgassert( ( & zvec ) == ( & accB.v ) );
		for(int i=0;i<_N;++i) { std::swap( zvec.data[accA.i] , zvec.data[accB.i] ); }
	}

	// std::iter_swap
	template<typename _B, unsigned int _N> inline
	void iter_swap(
		onika::container::IteratorT< onika::container::ZipVec<_B,_N> > itA,
		onika::container::IteratorT< onika::container::ZipVec<_B,_N> > itB )
	{
		onika::container::ZipVec<_B,_N> & zvec = itA.vec;
		onika::debug::dbgassert( ( & zvec ) == ( & itB.vec ) );
		std::swap( zvec.data[itA.i] , zvec.data[itB.i] );
	}
}


// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#endif // end of zipvec.h

