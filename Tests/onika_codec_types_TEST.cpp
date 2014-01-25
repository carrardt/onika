#include "onika/codec/types.h"
#include <vector>

int main()
{
	std::vector<int> vec(10);
	onika::codec::bounded_value(23,10,100);
//	onika::codec::bounded_uuipair_enc(10,100,23,31);
	return 0;
}

