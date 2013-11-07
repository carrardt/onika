#include "onika/container/bitarray.h"

#include <iostream>

int main()
{
	int* array = new int[10];
	onika::container::setarraybits(array,81,0x1324,13);
	std::cout<< (void*) onika::container::getarraybits(array,81,13) << "\n";
	return 0;
}

#endif

