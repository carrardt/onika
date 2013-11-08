#include "onika/bits.h"

#include <iostream>

int main()
{
	std::cout<< onika::mask1lsb<uint64_t>(0) << "\n";
	std::cout<< onika::mask1lsb<uint64_t>(64) << "\n";
	std::cout<< onika::mask1lsb<uint64_t>(1) << "\n";
	return 0;
}

 // end of header file

