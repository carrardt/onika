#include "onika/container/nbitsvector.h"
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

 // end of nbitsvector.h

