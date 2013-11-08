#include "onika/language.h"
#include <iostream>

int main()
{
	unsigned int a = -3;
	ONIKA_MAKE_SIGNED(unsigned long) b = -3;
	ONIKA_MAKE_SIGNED(double) c = -3;
	std::cout<<a<<' '<<b<<' '<<c<<'\n';
	
	return 0;
}
 // end of language.h



