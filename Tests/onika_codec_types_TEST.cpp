#include "onika/codec/types.h"
#include <vector>

int main()
{
	std::vector<int> vec(10);
	onika::codec::bounded_value(23,10,100);
	//onika::codec::bounded_integer_set(10,100,vec.begin(),vec.end());
	return 0;
}

