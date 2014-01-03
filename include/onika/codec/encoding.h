#ifndef __onika_codec_encoding_h
#define __onika_codec_encoding_h

#include "onika/debug/dbgassert.h"
#include "onika/codec/types.h"
#include "onika/container/algorithm.h"
#include "onika/language.h"
#include "onika/mathfunc.h"

namespace onika { namespace codec { 

	//  \ a 0  1  2  3  4  5  
	// b \___________________
	// 1 |  0
	// 2 |  1  2
	// 3 |  3  4  5
	// 4 |  6  7  8  9
	// 5 | 10 11 12 13 14
	// 6 | 15 16 17 18 19 20

#define ROWINDEX(v) ((v*(v-1))/2)

	template<typename ValueType>
	inline ValueType edge_enc(ValueType a,ValueType b)
	{
		debug::dbgassert(a!=b);
		if( b < a ) { std::swap(a,b); }
		return ROWINDEX(b) + a;
	}

	template<typename ValueType>
	inline void edge_dec(ValueType x, ValueType &a,ValueType &b)
	{
		ValueType bmin=1, bmax=1; 
		while( ROWINDEX(bmax) <= x )
		{
			bmin = bmax;
			bmax *= 2;
		}

		while( bmin<(bmax-1) )
		{
			ValueType pivot = (bmax+bmin)/2;
			if( ROWINDEX(pivot) > x ) bmax=pivot;
			else bmin=pivot;
		}
		b = bmin;
		a = x - ROWINDEX(b);
	}
#undef ROWINDEX

	template<typename Iterator>
	inline uint64_t uint_set_enc(Iterator first, Iterator last)
	{
		uint64_t n = last - first;
		if( n == 0 ) return 0;
		Iterator it=first;
		uint64_t a = *it;
		if( n == 1 ) return a;
		++it;
		uint64_t b = *it;
		if( n == 2 ) return edge_enc(a,b);
		else
		{
			debug::dbgassert(0 && "not implemented");
			return 0;
		}
	}

	inline uint64_t uint_set_enc_bound(int n, uint64_t maxValue)
	{
		if( n <= 1 ) return maxValue;
		else if( n==2 ) return edge_enc(maxValue-1,maxValue);
		else
		{
			debug::dbgassert(0 && "not implemented");
			return 0;
		}
	}

	template<typename Iterator>
	inline Iterator uint_set_dec(uint64_t value, uint64_t n, Iterator first)
	{
		Iterator it = first;
		if( n==1 ) { *(it++) = value; }
		else if( n==2 ) { uint64_t a,b; edge_dec(value,a,b); *(it++)=a; *(it++)=b; }
		else
		{
			debug::dbgassert(0 && "not implemented");
		}
		return it;		
	}

	/* ============================================
	 * === container aware encoding operations ====
	 * ============================================
	 * === Requirements: encoding of a single token
	 * === has a worst case complexity of O(log(n))
	 * ===
	 */

// container element value type
#define CValue typename _C::value_type

// signed version of container element value type
#define CSignedValue ONIKA_MAKE_SIGNED(typename _C::value_type)


	/* Function : value_enc
	 * Parameters :
	 * 	c : container
	 * 	i : item position
	 * Notes:
	 * encode a single value.
	 * TODO: type specialization to let double and float as single value, not bounded value
	 */

	template<typename _C> inline auto
	bounded_value_enc(const _C& c , size_t i , CValue Min , CValue Max) -> BoundedValue<CValue> 
	{
		CValue v = c[i];
		return bounded_value(v,Min,Max);
	}

	template<typename _C> inline auto
	value_enc( const _C& c , size_t i ) ONIKA_AUTO_RET( static_cast<typename _C::value_type>(c[i]) )

	template<typename _C> inline auto
	bounded_delta_enc(const _C& c, size_t a, size_t b, CValue Min, CValue Max) -> BoundedValue<CSignedValue>
	{
		CValue va = c[a];
		CValue vb = c[b];
		CValue delta = onika::math::sub( vb , va );
		return bounded_delta(va,delta,Min,Max);
	}

	template<typename _C> inline auto
	delta_enc(const _C& c , size_t a , size_t b) -> BoundedValue<CSignedValue>
	{
		CValue Min = container::min_value(c);
		CValue Max = container::max_value(c);
		return bounded_delta_enc(c,a,b,Min,Max);
	}

#undef CValue
#undef CSignedValue

} } // namespace onika::codec


// ======================== UNIT TEST =======================
#endif // end of file encoding.h


