#ifndef __onika_nbitsvector_h
#define __onika_nbitsvector_h

#include <cstdint>
#include <vector>
#include <stdlib.h>

#include "onika/debug/dbgassert.h"
#include "onika/container/iterator.h"
#include "onika/bits.h"
#include "onika/language.h"

#define DECL_AUTO_RET(expr) -> decltype(expr) { return expr; }

namespace onika { namespace container {

template<typename _StorageUnit=uint64_t>
struct NBitsVector
{

	typedef _StorageUnit Integer;
	typedef _StorageUnit StorageUnit;
	typedef std::vector<StorageUnit> BaseContainer;

	// generic container API
	ONIKA_CONTAINER_ACCESS_DEF(NBitsVector,Integer);

	typedef ONIKA_MAKE_SIGNED(value_type) signed_value_type;
	enum { UnitBits=sizeof(StorageUnit)*8 };

	// constructor
	inline NBitsVector() : NBits(sizeof(StorageUnit)), nelems(0) {}
	inline NBitsVector(unsigned int nb) : NBits(nb), nelems(0) {}

	inline unsigned int getNBits() const { return NBits; }

	inline Integer min_value() const
	{
		return std::is_signed<Integer>::value ? ( static_cast<Integer>(-1) << (NBits-1) ) : 0 ;
	}

	inline Integer max_value() const
	{
		return std::is_signed<Integer>::value ? mask1lsb<Integer>(NBits-1) : mask1lsb<Integer>(NBits) ;
	}

	inline void set(size_t i, Integer x)
	{
		debug::dbgassert( x >= min_value() && x <= max_value() );
		size_t b = i*NBits;
		size_t startUnit = b / UnitBits;
		size_t startBit = b % UnitBits;
		size_t endUnit = (b+NBits-1) / UnitBits;
		size_t endBit = (b+NBits-1) % UnitBits;
		for(size_t u = startUnit; u<=endUnit; ++u)
		{
			debug::dbgassert(u<data.size());
			int bs = (u==startUnit)?startBit:0;
			int be = (u==endUnit)?endBit:(UnitBits-1);
			int nb = be-bs+1;
			Integer mask = mask1lsb<Integer>(nb);
			data[u] &= ~(mask << bs);
			data[u] |= (x & mask) << bs;
			x >>= nb;
		}
	}
	inline Integer get(size_t i) const
	{
		size_t b = i*NBits;
		size_t startUnit = b / UnitBits;
		size_t startBit = b % UnitBits;
		size_t endUnit = (b+NBits-1) / UnitBits;
		size_t endBit = (b+NBits-1) % UnitBits;
		Integer x = 0;
		int bc = 0;
		for(size_t u = startUnit; u<=endUnit; ++u)
		{
			debug::dbgassert(u<data.size());
			int bs = (u==startUnit)?startBit:0;
			int be = (u==endUnit)?endBit:(UnitBits-1);
			int nb = be-bs+1;
			Integer mask = mask1lsb<Integer>(nb);
			x |= ( ( data[u] >> bs ) & mask ) << bc;
			bc += nb;
		}
		const int shift = UnitBits - NBits;
		x <<= shift;
		x >>= shift;
		return x;
	}

	inline size_t capacity() const
	{
		return (data.capacity()*UnitBits)/NBits;
	}

	inline size_t size() const
	{
		return nelems;
	}

	inline void reserve(size_t n)
	{
		data.reserve( (n*UnitBits)/NBits );
	}

	inline void resize(size_t n)
	{
		size_t nunits = (n*NBits+UnitBits-1)/UnitBits;
		data.resize(nunits,0);
		nelems = n;
	}
	inline void resize(size_t n, Integer x)
	{
		size_t oldN = size();
		resize(n);
		for(size_t i=oldN;i<n;++i) { set(i,x); }
	}
	
	inline void clear(int elembits)
	{
		data.clear();
		nelems = 0;
		NBits = elembits;
	}

	inline void push_back(Integer x)
	{
		++ nelems;
		if( ( (nelems*NBits+UnitBits-1) / UnitBits ) >= data.size() )
		{
			data.push_back(0);
		}
		set(nelems-1,x);
	}

	inline void pop_back()
	{
		-- nelems;
		if( ( (nelems*NBits+UnitBits-1) / UnitBits ) < (data.size()-1) )
		{
			data.pop_back();
		}
	}

	size_t nelems;
	BaseContainer data;
	unsigned int NBits;
};

} } // end of namespace


//==========================================
//=== container algorithm specialization ===
//==========================================
#include "onika/container/algorithm.h"
namespace onika { namespace container {

	template<typename _S> inline auto 
	min_value(const NBitsVector<_S>& c) 
	DECL_AUTO_RET( c.min_value() )

	template<typename _S> inline auto 
	max_value(const NBitsVector<_S>& c) 
	DECL_AUTO_RET( c.max_value() )

	template<typename _S> inline auto 
	memory_bytes(const NBitsVector<_S>& c) -> size_t
	{
		return sizeof(c) - sizeof(c.data) + memory_bytes(c.data);
	}

} }

// No STL algorithm specialization

// No codec specialization


#undef DECL_AUTO_RET



// ======================== UNIT TEST =======================
#ifdef onika_container_nbitsvector_TEST
#include <iostream>
#include <algorithm>

#include <cstdint>
#include <stdlib.h>

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
		return static_cast<uint64_t>( drand48() * ((1UL<<nbits)-1) );
	}
	else
	{
		uint64_t x = mrand48();
		nbits -= 32;
		x <<= nbits;
		x |= static_cast<uint64_t>( drand48() * ((1UL<<nbits)-1) );
		return x;
	}
}


#define CHECK_VECTOR_EQUALITY() \
for(int i=0;i<N;i++) { Integer x=vec[i]; onika::debug::dbgassert( ref[i] == x ); }

template<typename Integer>
inline void runTest()
{
	onika::container::NBitsVector<Integer> vec;
	const int wordbits = sizeof(Integer)*8;
	bool is_signed = std::is_signed<Integer>::value;
	std::cout<<"Testing "<<(is_signed?"signed":"unsigned")<<" "<<wordbits<<"-Bits storage :"  ;
	for(int nbits=1;nbits<=wordbits;nbits++)
	{
		std::cout<<nbits<<" "; std::cout.flush();
		const int shiftbits = wordbits - nbits;
		vec.clear(nbits);
		std::vector<Integer> ref;
		bool exhaust = (nbits <= 10);
		size_t N = exhaust ? mask(nbits) : 1000;
		//std::cout<<N<<"\n";
		for(size_t i=0;i<=N;i++)
		{
			Integer x = exhaust ? i : ( randN(nbits) & mask(nbits) );
			x <<= shiftbits;
			x >>= shiftbits;
			ref.push_back(x);
			vec.push_back(x);		
			//if(wordbits==8) std::cout<<(int)x<<" "<<(int)ref[i]<<" "<<(int)vec[i]<<"\n";
			//else std::cout<<x<<" "<<ref[i]<<" "<<vec[i]<<"\n";
		}
		//std::cout<<"\n";
		CHECK_VECTOR_EQUALITY();
		vec.resize(N/2); ref.resize(N/2);
		vec.resize(N+1); ref.resize(N+1);
		for(size_t i=N/2;i<=N;i++)
		{
			Integer x = exhaust ? i : ( randN(nbits) & mask(nbits) );
			x <<= shiftbits;
			x >>= shiftbits;
			vec[i] = x;
			ref[i] = x;
		}
		CHECK_VECTOR_EQUALITY();
		std::sort( vec.begin(), vec.end() );
		std::sort( ref.begin(), ref.end() );
		CHECK_VECTOR_EQUALITY();
	}
	std::cout<<"\n";
}

int main(int argc, char* argv[])
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand48(seed);
	runTest<int8_t>();
	runTest<uint8_t>();
	runTest<int16_t>();
	runTest<uint16_t>();
	runTest<int32_t>();
	runTest<uint32_t>();
	runTest<int64_t>();
	runTest<uint64_t>();

	std::cout<<"PASSED\n";
	
	return 0;
}

#endif // end of unit test


#endif // end of nbitsvector.h

