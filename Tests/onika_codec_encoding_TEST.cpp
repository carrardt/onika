#include "onika/codec/encoding.h"

#include <iostream>
#include <cstdint>

inline uint64_t mask(int nbits)
{
	uint64_t m = 1;
	m = ( m<<(nbits-1) ) - 1;
	return (m<<1) | 1;
}

inline uint64_t randN(int nbits)
{
	if( nbits < 32 )
	{
		return static_cast<uint64_t>( drand48() * mask(nbits) );
	}
	else
	{
		uint64_t x = mrand48();
		nbits -= 32;
		x <<= nbits;
		x |= static_cast<uint64_t>( drand48() * mask(nbits) );
		return x;
	}
}

inline int64_t signed_randN(int nbits)
{
	int64_t x = 0;
	int shiftbits = 64 - nbits;
	if( nbits < 32 )
	{
		x = static_cast<int64_t>( drand48() * mask(nbits) );
	}
	else
	{
		x = mrand48();
		nbits -= 32;
		x <<= nbits;
		x |= static_cast<int64_t>( drand48() * mask(nbits) );
	}
	x = (x << shiftbits) >> shiftbits;
	return x;
}

using namespace std;

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"Seeding with "<<seed<<"\n";
	srand48(seed);

	cout<<"Testing edge encoding ...\n";
	for(int x=0;x<1000;x++)
	{
		int a,b;
		onika::codec::edge_dec(x,a,b);
		//cout<<x<<" -> "<<a<<","<<b<<endl;
		int y = onika::codec::edge_enc(a,b);
		onika::debug::dbgassert(y==x);
	}
	for(int i=0;i<1000;i++)
	{
		uint64_t y,a=0,b=0,x=randN(31);
		onika::codec::edge_dec(x,a,b);
		y = onika::codec::edge_enc(a,b);
		//cout<<x<<" -> "<<a<<","<<b<<" -> "<<y<<endl;
		onika::debug::dbgassert(y==x);		
	}
/*
	cout<<"uint wavelet : "; cout.flush();
	for(int nbits=4;nbits<=64;nbits++)
	{
		cout<<nbits<<" "; cout.flush();
		for(int i=0;i<1000;i++)
		{
			uint64_t a=randN(nbits), b=randN(nbits), a2=0, b2=0;
			uint64_t avg; int64_t delta;
			onika::codec::wavelet_enc(a,b,avg,delta);
			onika::codec::wavelet_dec(avg,delta,a2,b2);
			int64_t bdiff = b-b2; if(bdiff<0) bdiff=-bdiff;
			onika::debug::dbgassert(a==a2 && bdiff<=1);
		}
	}

	cout<<"\nint wavelet : "; cout.flush();
	for(int nbits=4;nbits<=64;nbits++)
	{
		cout<<nbits<<" "; cout.flush();
		for(int i=0;i<1000;i++)
		{
			int64_t a=signed_randN(nbits), b=signed_randN(nbits), a2=0, b2=0;
			int64_t avg,delta;
			onika::codec::wavelet_enc(a,b,avg,delta);
			onika::codec::wavelet_dec(avg,delta,a2,b2);
			//cout<<a<<','<<b<<" -> "<<avg<<','<<delta<<" -> "<<a2<<','<<b2<<"\n";
			int64_t bdiff = b-b2; if(bdiff<0) bdiff=-bdiff;
			onika::debug::dbgassert(a==a2 && bdiff<=1);
		}
	}
	cout<<"\n";

	cout<<"double value pair ? "; cout.flush();
	double da,db;
	cin>>da>>db;
	onika::codec::wavelet_enc(da,db,da,db);
	cout<<"avg="<<da<<", delta="<<db<<endl;
	onika::codec::wavelet_dec(da,db,da,db);
	cout<<"a="<<da<<", b="<<db<<endl;
*/
	return 0;
}

 // end of file encoding.h


