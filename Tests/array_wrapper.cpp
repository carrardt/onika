#include "onika/container/arraywrapper.h"
#include <iostream>
#include <algorithm>

using onika::container::array_wrapper;

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand48(seed);

	double array[100];
	for(int i=0;i<100;i++) { array[i] = drand48(); }

	auto aw = array_wrapper( array, 100 );
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	aw.resize(50);
	std::sort( aw.begin(), aw.end() );
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	aw.resize(25);
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	aw.resize(50);
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	aw.resize(100);
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	return 0;
}
