#ifndef __onika_codec_encoding_h
#define __onika_codec_encoding_h

#include "onika/debug/dbgassert.h"
#include "onika/codec/types.h"
#include "onika/language.h"

// should only contain encoding algorithms

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

	inline uint64_t uuipair_enc(uint64_t a,uint64_t b)
	{
		debug::dbgassert(a!=b);
		if( b < a ) { std::swap(a,b); }
		return ROWINDEX(b) + a;
	}

	template<class Integer>
	inline uint64_t uuipair_enc(const std::pair<Integer,Integer>& p)
	{
		return uuipair_enc(p.first,p.second);
	}

	inline uint64_t uuipair_bound(uint64_t n)
	{
		return uuipair_enc(n,n-1);
	}

	// TODO: decoding part should go in a separate file
	inline std::pair<uint64_t,uint64_t>
	uuipair_dec(uint64_t x)
	{
		uint64_t a,b;
		uint64_t bmin=1, bmax=1;
		while( ROWINDEX(bmax) <= x )
		{
			bmin = bmax;
			bmax *= 2;
		}
		while( bmin<(bmax-1) )
		{
			uint64_t pivot = (bmax+bmin)/2;
			if( ROWINDEX(pivot) > x ) bmax=pivot;
			else bmin=pivot;
		}
		b = bmin;
		a = x - ROWINDEX(b);
		return std::make_pair(a,b);
	}
#undef ROWINDEX

	inline auto bounded_uuipair_enc(uint64_t low, uint64_t high,uint64_t a, uint64_t b)
	ONIKA_AUTO_RET( bounded_value(0ul, uuipair_bound(high-low), uuipair_enc(a-low,b-low) ) )

	
} } // namespace onika::codec


// ======================== UNIT TEST =======================
#endif // end of file encoding.h


