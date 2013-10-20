#ifndef __onika_codec_encoding_h
#define __onika_codec_encoding_h

#include "onika/debug/dbgassert.h"
#include "onika/codec/types.h"
#include "onika/container/algorithm.h"
#include "onika/language.h"

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

	// (!) Read carefully :
	// for integer case, lossless reconstruction is guaranteed for a only
	// b's least significant bit will be faulty
	// specialization toward multicomponent values (tuples,Vec,etc.) should be done
	template<typename IntType>
	inline void wavelet_enc(IntType a, IntType b, IntType& avg, ONIKA_MAKE_SIGNED(IntType) &delta)
	{
		avg = ( a + b ) / 2;
		delta = avg - a;
	}

	template<typename IntType>
	inline void wavelet_dec(IntType avg, ONIKA_MAKE_SIGNED(IntType) delta, IntType& a, IntType &b)
	{
		b = avg + delta;
		a = avg - delta;
	}

	template<typename IntType>
	inline void wavelet_delta_bounds(IntType avg, IntType low, IntType high,
				ONIKA_MAKE_SIGNED(IntType) delta, 
				ONIKA_MAKE_SIGNED(IntType) &dlow, 
				ONIKA_MAKE_SIGNED(IntType) &dhigh )
	{
		IntType lowRange = avg-low;
		IntType highRange = high-avg;
		IntType range = (lowRange>highRange) ? lowRange : highRange;
		dlow = -range;
		dhigh = range;
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

	/* Function : wavelet_enc
	 * Parameters :
	 * 	c : container
	 * 	a : position of first element
	 * 	b : position of second element
	 * Notes:
	 * container modifying encoding method
	 * store average value of elements a and b into element a, and encodes (b-a)/2
	 * optimally, this should allow a lossless reconstruction.
	 */
	template<typename _C> inline auto
	wavelet_enc( _C& c, size_t a, size_t b) -> BoundedValue<CSignedValue>
	{
		CValue Min = container::min_value(c);
		CValue Max = container::max_value(c);
		CValue va = c[a];
		CValue vb = c[b];
		CValue avg;
		CSignedValue delta;
		wavelet_enc(va,vb,avg,delta);
		c[a] = avg;
		return bounded_delta(avg,delta,Min,Max);
	}

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
		CValue delta = vb - va;
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
#ifdef onika_codec_encoding_TEST

#include <iostream>
#include <cstdint>

inline uint64_t mask(int nbits)
{
	uint64_t m = 1;
	m = ( m<<(nbits-1) ) - 1;
	return (m<<1) | 1;
}

inline uint64_t randN(int nbits)
{
	if( nbits < 32 )
	{
		return static_cast<uint64_t>( drand48() * mask(nbits) );
	}
	else
	{
		uint64_t x = mrand48();
		nbits -= 32;
		x <<= nbits;
		x |= static_cast<uint64_t>( drand48() * mask(nbits) );
		return x;
	}
}

inline int64_t signed_randN(int nbits)
{
	int64_t x = 0;
	int shiftbits = 64 - nbits;
	if( nbits < 32 )
	{
		x = static_cast<int64_t>( drand48() * mask(nbits) );
	}
	else
	{
		x = mrand48();
		nbits -= 32;
		x <<= nbits;
		x |= static_cast<int64_t>( drand48() * mask(nbits) );
	}
	x = (x << shiftbits) >> shiftbits;
	return x;
}

using namespace std;

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"Seeding with "<<seed<<"\n";
	srand48(seed);

	cout<<"Testing edge encoding ...\n";
	for(int x=0;x<1000;x++)
	{
		int a,b;
		onika::codec::edge_dec(x,a,b);
		//cout<<x<<" -> "<<a<<","<<b<<endl;
		int y = onika::codec::edge_enc(a,b);
		onika::debug::dbgassert(y==x);
	}
	for(int i=0;i<1000;i++)
	{
		uint64_t y,a=0,b=0,x=randN(31);
		onika::codec::edge_dec(x,a,b);
		y = onika::codec::edge_enc(a,b);
		//cout<<x<<" -> "<<a<<","<<b<<" -> "<<y<<endl;
		onika::debug::dbgassert(y==x);		
	}

	cout<<"uint wavelet : "; cout.flush();
	for(int nbits=4;nbits<=64;nbits++)
	{
		cout<<nbits<<" "; cout.flush();
		for(int i=0;i<1000;i++)
		{
			uint64_t a=randN(nbits), b=randN(nbits), a2=0, b2=0;
			uint64_t avg; int64_t delta;
			onika::codec::wavelet_enc(a,b,avg,delta);
			onika::codec::wavelet_dec(avg,delta,a2,b2);
			int64_t bdiff = b-b2; if(bdiff<0) bdiff=-bdiff;
			onika::debug::dbgassert(a==a2 && bdiff<=1);
		}
	}

	cout<<"\nint wavelet : "; cout.flush();
	for(int nbits=4;nbits<=64;nbits++)
	{
		cout<<nbits<<" "; cout.flush();
		for(int i=0;i<1000;i++)
		{
			int64_t a=signed_randN(nbits), b=signed_randN(nbits), a2=0, b2=0;
			int64_t avg,delta;
			onika::codec::wavelet_enc(a,b,avg,delta);
			onika::codec::wavelet_dec(avg,delta,a2,b2);
			//cout<<a<<','<<b<<" -> "<<avg<<','<<delta<<" -> "<<a2<<','<<b2<<"\n";
			int64_t bdiff = b-b2; if(bdiff<0) bdiff=-bdiff;
			onika::debug::dbgassert(a==a2 && bdiff<=1);
		}
	}
	cout<<"\n";

	cout<<"double value pair ? "; cout.flush();
	double da,db;
	cin>>da>>db;
	onika::codec::wavelet_enc(da,db,da,db);
	cout<<"avg="<<da<<", delta="<<db<<endl;
	onika::codec::wavelet_dec(da,db,da,db);
	cout<<"a="<<da<<", b="<<db<<endl;

	return 0;
}

#endif // end of unit test


#endif // end of file encoding.h


