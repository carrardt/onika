#ifndef __onika_container_bitarray_h
#define __onika_container_bitarray_h

#include <cstdint>
#include <stdlib.h>

#include "onika/bits.h"

namespace onika { namespace container {

	template<typename T>
	static inline void setarraybits(T* data, size_t b, T x, unsigned int NBits)
	{
		const unsigned int UnitBits = sizeof(T)*8;
		size_t startUnit = b / UnitBits;
		size_t startBit = b % UnitBits;
		size_t endUnit = (b+NBits-1) / UnitBits;
		size_t endBit = (b+NBits-1) % UnitBits;
		for(size_t u = startUnit; u<=endUnit; ++u)
		{
			int bs = (u==startUnit)?startBit:0;
			int be = (u==endUnit)?endBit:(UnitBits-1);
			int nb = be-bs+1;
			T mask = mask1lsb<T>(nb);
			data[u] &= ~(mask << bs);
			data[u] |= (x & mask) << bs;
			x >>= nb;
		}
	}

	template<typename T>
	static inline T getarraybits(T* data, size_t b, unsigned int NBits)
	{
		const unsigned int UnitBits = sizeof(T)*8;
		size_t startUnit = b / UnitBits;
		size_t startBit = b % UnitBits;
		size_t endUnit = (b+NBits-1) / UnitBits;
		size_t endBit = (b+NBits-1) % UnitBits;
		T x = 0;
		int bc = 0;
		for(size_t u = startUnit; u<=endUnit; ++u)
		{
			int bs = (u==startUnit)?startBit:0;
			int be = (u==endUnit)?endBit:(UnitBits-1);
			int nb = be-bs+1;
			T mask = mask1lsb<T>(nb);
			x |= ( ( data[u] >> bs ) & mask ) << bc;
			bc += nb;
		}
		const int shift = UnitBits - NBits;
		x <<= shift;
		x >>= shift;
		return x;
	}

} } // end of namespace

#endif 




