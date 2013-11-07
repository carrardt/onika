#include "onika/codec/bitstream.h"

#include <iostream>
#include <fstream>

int main()
{
	std::ofstream binfile("out.bin");
	onika::codec::BitStream out(binfile,std::cout);

	std::cout<<"Word bits = "<<onika::codec::BitStream::WORD_BITS<<"\n";

	int set1[10] = {10,9,8,7,6,5,4,3,2,1};
	int set2[10], n=0;
	onika::Vec<3,onika::codec::BoundedValue<double> > v_of_bv;
	onika::codec::BoundedValue<onika::Vec<3,double> > bv_of_v;

	out<< v_of_bv;
	out<< bv_of_v;

	out<< onika::codec::list(set1,set1+10);
	out.flush();
	out.debug() << "--- list ---";

	std::cout<<"subset ? "; std::cout.flush();
	do { std::cin>>set2[n++]; } while(set2[n-1]>=0);
	--n;

	out<< onika::codec::subset(set1,set1+10,set2,set2+n);
	out.flush();
	out.debug() << "--- subset ---";

	out<< onika::codec::bounded_integer_set(1,10,set2,set2+n);
	out.flush();
	out.debug() << "--- bounded_integer_set ---";

	// just a compile test
	out<< onika::codec::bounded_value(124,100,150);
	out.flush();
	out.debug() << "--- bounded_integer ---";

	return 0;
}

#endif


