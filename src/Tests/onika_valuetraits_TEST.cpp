#include "onika/valuetraits.h"
#include <iostream>

int main()
{
	std::cout<<"min int = "<< onika::value_traits<int>::min()<<std::endl;
	std::cout<<"max float = "<< onika::value_traits<float>::min()<<std::endl;
	return 0;
}
#endif

