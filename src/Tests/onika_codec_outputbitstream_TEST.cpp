#include "onika/codec/outputbitstream.h"

#include "onika/codec/asciistream.h"
#include "onika/codec/streamdup.h"

#include <iostream>
#include <fstream>

int main()
{
	std::ofstream binfile("out.bin");
	onika::codec::OutputBitStream binout(binfile,std::cout);
	onika::codec::AsciiStream asciiout(std::cout);
	auto out = onika::codec::streamdup(binout,asciiout);

	std::cout<<"Word bits = "<<onika::codec::OutputBitStream::WORD_BITS<<"\n";

	binout.writeBits( onika::mask1lsb<uint64_t>(64) , 9 );

	uint64_t x=0;
	std::cout<<"integer ? "; std::cout.flush();
	std::cin>>x;
	std::cout<<(void*)x<<"\n";
	out<<x;	
	out.stream1.debug()<<"streampos = "<<out.stream1.streampos;

	onika::Vec<3,onika::codec::BoundedValue<double> > v_of_bv = {
		onika::codec::bounded_value(1.24,0.5,2.3),
		onika::codec::bounded_value(1.3,-0.1,1.9),
		onika::codec::bounded_value(0.0,-0.56,2.9) };
	out<< v_of_bv;
	out.stream1.debug()<<"streampos = "<<out.stream1.streampos;

	onika::codec::BoundedValue<onika::Vec<3,double> > bv_of_v(
		onika::vec(1.23,1.1,1.2),
		onika::vec(0.2,0.4,0.6),
		onika::vec(2.1,3.1,4.1) );
	out<< bv_of_v;
	out.stream1.debug()<<"streampos = "<<out.stream1.streampos;

	int set1[10] = {10,9,8,7,6,5,4,3,2,1};
	int set2[10], n=0, min=0, max=0;
	out<< onika::codec::list(set1,set1+10);
	out.stream1.debug()<<"streampos = "<<out.stream1.streampos;

	std::cout<<"subset ? "; std::cout.flush();
	n=0;
	do { std::cin>>set2[n++]; } while(set2[n-1]>=0);
	--n;
	out<< onika::codec::subset(set1,set1+10,set2,set2+n);
	out.stream1.debug()<<"streampos = "<<out.stream1.streampos;

	std::cout<<"bounded integer set (min,max,values...,-1) ? "; std::cout.flush();
	std::cin>>min>>max;
	n=0;
	do { std::cin>>set2[n++]; } while(set2[n-1]>=0);
	--n;
	out<< onika::codec::bounded_integer_set(min,max,set2,set2+n);
	out.stream1.debug()<<"streampos = "<<out.stream1.streampos;

	// just a compile test
	out<< onika::codec::bounded_value(124,100,150);
	out.flush();
	out.stream1.debug()<<"streampos = "<<out.stream1.streampos;

	return 0;
}

#endif


