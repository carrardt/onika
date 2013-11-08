#include "onika/mathfunc.h"
#include <iostream>
int main()
{
	std::cout<<"norm2(5) = "<<onika::math::norm2(5.0)<<"\n";
	std::cout<<"distance(3,5) = "<<onika::math::distance(3,5)<<"\n";
	std::cout<<"abs(-1.2) = "<<onika::math::abs(-1.2)<<"\n";
	return 0;
}

