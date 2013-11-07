#include "onika/vec.h"

#include <iostream>

int main()
{
	onika::Vec<3,int> v;
	std::cin>>v.x[0]>>v.x[1]>>v.x[2];
	std::cout<<"norm = "<<v.norm()<<"\n";
	return 0;
}

 // end of vec.h

