#include "onika/codec/inputbitstream.h"

#include <iostream>
#include <fstream>

template<class T>
inline std::ostream& operator << (std::ostream& out, onika::codec::BoundedValue<T>& bv)
{
	out << '['<<bv.low<<','<<bv.high<<"]("<<bv.x<<')';
	return out;
}

template<typename I1, typename I2>
inline std::ostream& operator << (std::ostream& out, onika::codec::Subset<I1,I2>& ss)
{
	out<<'[';
	for( I1 it=ss.rsf; it!=ss.rsl; ++it )
	{
		if( it!=ss.rsf ) out<<',';
		out<<(*it);
	}
	out<<"](";
	for( I2 it=ss.ssf; it!=ss.ssl; ++it )
	{
		if( it!=ss.ssf ) out<<',';
		out<<(*it);
	}
	out<<')';
	return out;
}

int main(int argc, char* argv[])
{
	bool autoMode = argc > 1 && std::string("-a") == argv[1];
	std::string fname = "out.bin";
	if( ! autoMode )
	{
		std::cout<<"File ? ";
		std::cout.flush();
		std::cin>>fname;
	}

	std::ifstream ifile(fname.c_str());
	if( !ifile )
	{
		std::cerr<<"Failed to open "<<fname<<"\n";
		return 1;
	}

	onika::codec::InputBitStream<std::ifstream,std::ostream> in(ifile,std::cout);

	bool endOfTest = false;
	do
	{
		std::cout<<"type ? "; std::cout.flush();
		std::string token;
		std::cin>>token;
		if( token == "b" )
		{
			std::cout<<"nbits ? "; std::cout.flush();
			int nbits=0; std::cin>>nbits;
			uint64_t x = in.readBits(nbits);
			in.debug() << "hex:"<< (void*)x <<" dec:"<<x;
			std::cout<<"\n";
		}
		else if( token == "bv" )
		{
			std::cout<<"min ? "; std::cout.flush();
			int min=0; std::cin>>min;
			std::cout<<"max ? "; std::cout.flush();
			int max=0; std::cin>>max;
			auto bv = onika::codec::bounded_value(min,max);
			in >> bv;
			std::cout << bv << "\n";
		}
		else if( token == "s" )
		{
			std::cout<<"bits ? "; std::cout.flush();
			int bits=0; std::cin>>bits;
			while( bits > 64 ) { in.readBits(64); bits-=64; }
			in.readBits(bits);
		}
		else if( token == "ss" )
		{
			std::cout<<"set ? "; std::cout.flush();
			std::vector<int> refSet;
			int x=-1; do { std::cin>>x; refSet.push_back(x); } while(x!=-1);
			refSet.pop_back();
			std::vector<int> result(refSet.size());
			auto subset = onika::codec::subset( refSet.begin(), refSet.end(), result.begin() );
			in >> subset;
			std::cout << subset;
		}
		else { endOfTest = true; }
	}
	while( ! endOfTest );

#if 0
	std::cout<<"Word bits = "<<onika::codec::InputBitStream::WORD_BITS<<"\n";

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
#endif

	return 0;
}


