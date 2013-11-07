#ifndef __onika_quantize_lossydoublequantizer_h
#define __onika_quantize_lossydoublequantizer_h

#include <cstdint>
#include <vector>
#include <algorithm>
#include "onika/poweroftwo.h"
#include "onika/debug/dbgassert.h"
#include "onika/debug/dbgmessage.h"

namespace onika { namespace quantize {

struct LossyDoubleQuantizer
{
	typedef double DecodedType;

	inline unsigned int getNBits() const { return nbits; }

	inline double getLowerBound() const { return low; }

	inline double getHigherBound() const { return high; }

	inline uint64_t max_integer() const
	{
		uint64_t m = 1;
		m = ( m<<(nbits-1) ) - 1;
		return (m<<1) | static_cast<uint64_t>(1);
	}

	static inline uint64_t mantissa_mask()
	{
		return 0x000FFFFFFFFFFFFFull;
	}

	inline uint64_t encode(double x) const
	{
		if( high <= low ) return 0;
		if( x <= low ) { return 0; }
		if( x >= high ) { return max_integer(); }
		x = 1.0 + (x - low) / (high - low);
		if( x <= 1.0 ) { return 0; }
		if( x >= 2.0 ) { return max_integer(); }
		union { double d; uint64_t i; } u = { x };
		uint64_t i = u.i & mantissa_mask();
		unsigned int shift = ( nbits >= 52 ) ? 0 : (52-nbits);
		return i>>shift;
	}

	inline double decode(uint64_t x) const
	{
		union { double d; uint64_t i; } u;
		const uint64_t mantissa_clear_mask = ~ mantissa_mask();
		if( nbits > 52 )
		{
			x = x >> (nbits-52);
		}
		else
		{
			x = x << (52-nbits);
		}
		u.d = 1.5; // correctly fills sign and exponent
		u.i = ( u.i & mantissa_clear_mask ) | x;
		double r = u.d;
		return low + (r-1.0) * (high-low) ;
	}

	template<typename Iterator>
	inline unsigned int initialize(Iterator first, Iterator last, unsigned int minbits=1, unsigned int maxbits=52)
	{
		if(last==first)
		{
			return nbits;
		}

		std::vector<double> buffer(last-first);
		std::copy(first,last,buffer.begin());
		std::sort(buffer.begin(),buffer.end());
		low = buffer.front();
		high = buffer.back();
		nbits = maxbits;

		uint64_t minGap = max_integer();
		uint64_t maxGap = 0;
		double avgGap = 0;
		size_t n=buffer.size();
		size_t nonZeroGaps = 0;
		for(size_t i=1;i<n;i++)
		{
			uint64_t g = encode(buffer[i]) - encode(buffer[i-1]);
			if( g > 0 )
			{
				if( g > maxGap ) maxGap = g;
				else if( g < minGap ) minGap = g;
				avgGap += g;
				++ nonZeroGaps;
			}
		}
		avgGap /= nonZeroGaps;
		unsigned int unused_low_bits = onika::nextpo2log(minGap) - 1;
		unsigned int suggested_bits = nbits;
		//debug::dbgmessage()<<"nbits="<<nbits<<", minGap="<<minGap<<", avgGap="<<avgGap<<", unused="<<unused_low_bits;
		if( unused_low_bits > 0 )
		{
			suggested_bits = nbits - unused_low_bits;
		}
		nbits = suggested_bits;
		if( nbits < minbits ) nbits = minbits;
		return nbits;
	}
	
	// ============= members ===================
	unsigned int nbits;
	double low,high;
};

} } // end of namespace



// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#endif // end of lossydoublequantizer.h

