#include "onika/codec/asciistream.h"

#include <iostream>

int main()
{
	onika::codec::AsciiStream out(std::cout);
	int set1[10] = {10,9,8,7,6,5,4,3,2,1};
	int set2[10], n=0;

	out<< onika::codec::list(set1,set1+10) << "\n";

	std::cout<<"subset ? "; std::cout.flush();
	do { std::cin>>set2[n++]; } while(set2[n-1]>=0);
	--n;

	out<< onika::codec::subset(set1,set1+10,set2,set2+n);

	out<< onika::codec::bounded_integer_set(1,10,set1,set1+5);

	// just a compile test
	out<< onika::codec::bounded_value(124,100,150);

	return 0;
}

#endif


