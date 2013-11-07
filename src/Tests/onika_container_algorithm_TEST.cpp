#include "onika/container/algorithm.h"

#include <iostream>
#include <vector>

int main()
{
	std::vector<int> ivec(10);
	std::cout<<"memory_bytes = " << onika::container::memory_bytes(ivec) << "\n";
	std::cout<<"min_value = " << onika::container::min_value(ivec) << "\n";
	std::cout<<"max_value = " << onika::container::max_value(ivec) << "\n";
	return 0;
}

 // end of algorithm.h

