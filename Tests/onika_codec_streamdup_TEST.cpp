#include "onika/codec/streamdup.h"

#include <iostream>

int main()
{
	auto out = onika::codec::streamdup( std::cout, std::cerr );

	out << "Hello World\n";
	out.flush();

	return 0;
}


