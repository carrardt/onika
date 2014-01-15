#include "onika/codec/outputbitstream.h"

#include <iostream>
#include <fstream>

using obitstream = onika::codec::OutputBitStream<std::ostream>;

int main()
{
	std::ofstream binfile("out.bin");
	obitstream binout(binfile);
	std::cout<<"Word bits = "<<obitstream::WORD_BITS<<"\n";

	std::cout<<"bits = 0"<<binout.totalBits()<<"\n";
	binout.writeBits( onika::mask1lsb<uint64_t>(64) , 9 );
	std::cout<<"bits = "<<binout.totalBits()<<"\n";
	binout.writeBits( 333 , 12 );
	std::cout<<"bits = "<<binout.totalBits()<<"\n";
	binout.writeBits( 999 , 11 );
	std::cout<<"bits = "<<binout.totalBits()<<"\n";

	return 0;
}

