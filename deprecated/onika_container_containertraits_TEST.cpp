#include "onika/container/containertraits.h"
#include <iostream>
#include <vector>
#include "onika/codec/asciistream.h"
int main()
{
	std::vector<int> ivec(10);
	onika::codec::AsciiStream os(std::cout);
	os << onika::container::value_enc(ivec,5);
	std::cout<<"\nPASSED\n";
	return 0;
}


