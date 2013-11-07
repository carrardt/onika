#include "onika/container/sequence.h"

#include <algorithm>
#include <iostream>

int main()
{
	onika::container::SequenceContainer<int> seq(10);

	for( auto i : seq ) { std::cout<<i<<' '; }
	std::cout<<'\n';

	return 0;
}

#endif

