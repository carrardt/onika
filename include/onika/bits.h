#ifndef __onika_bits_h
#define __onika_bits_h

namespace onika {

	template<typename Word>
	static inline Word mask1lsb(unsigned int n)
	{
		Word m = 1;
		m = ( m<<(n-1) ) - 1;
		m = (m<<1) | static_cast<Word>(1);
		m = ( n>0 ) ? m : 0;
		return m;
	}
}



// ======================== UNIT TEST =======================
#ifdef onika_bits_TEST

#include <iostream>

int main()
{
	std::cout<< onika::mask1lsb<uint64_t>(0) << "\n";
	std::cout<< onika::mask1lsb<uint64_t>(64) << "\n";
	std::cout<< onika::mask1lsb<uint64_t>(1) << "\n";
	return 0;
}

#endif // end of unit test

#endif // end of header file

