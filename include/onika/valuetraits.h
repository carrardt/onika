#ifndef __onika_valuetraits_h
#define __onika_valuetraits_h

#include <limits>
#include <cstdint>

namespace onika {


// numeric_limits doesn't provide real minimum value in case of denormalized floating point type.
// this template aleviates this caveat providing mathematical minimum representable value of a type.
template<typename value_type>
struct value_traits
{
	static inline value_type min()
	{
		return ( -std::numeric_limits<value_type>::max() < std::numeric_limits<value_type>::min() ) ?
			 -std::numeric_limits<value_type>::max()
			: std::numeric_limits<value_type>::min();
	}
	static inline value_type max() { return std::numeric_limits<value_type>::max(); }
};

} // end of namespace

#endif

// ================== UNIT TEST ===============
#ifdef onika_valuetraits_TEST
#include <iostream>

int main()
{
	std::cout<<"min int = "<< onika::value_traits<int>::min()<<std::endl;
	std::cout<<"max float = "<< onika::value_traits<float>::min()<<std::endl;
	return 0;
}
#endif

