#include "onika/mathfunc.h"
#include "onika/tuple.h"
#include <iostream>

ONIKA_USE_MATH;

template<class... T>
inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
{
	onika::tuple::print( out, t );
	return out;
}

#define MT(x...) std::make_tuple(x)

int main()
{
	auto t1 = MT(1.1f,2.2,3u);
	std::cout<< t1 <<"\n";
	return 0;
}


