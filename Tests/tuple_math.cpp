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
	auto t1 = MT(1.1f,2.2,-3);
	auto t2 = inverse(t1);
	auto t3 = abs(t1);
	auto t4 = negate(t1);

	std::cout<< t1 <<"\n";
	std::cout<< t2 <<"\n";
	std::cout<< t3 <<"\n";
	std::cout<< t4 <<"\n";

	std::cout<< sub(3.2,1.5) <<"\n";
	std::cout<< distance2(1.5,3.5) <<"\n";
	return 0;
}


