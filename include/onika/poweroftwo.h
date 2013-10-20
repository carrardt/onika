#ifndef __onika_poweroftwo_h
#define __onika_poweroftwo_h

namespace onika
{
	template<int x,int pot=0>
	struct NextPowOfTwoLog
	{
		enum { value = ( (1UL<<pot) >= x ) ? pot : NextPowOfTwoLog<x,pot+1>::value };
	};
	template<int x> struct NextPowOfTwoLog<x,32> { enum { value = 32 }; };

	template<typename ValueType>
	inline int nextpo2log(ValueType x)
	{
		int l=0;
		while( x > 0 ) { x/=2; ++l; }
		return l;
	}

};

#endif



// ======================================================
// ======================= Unit Test ====================
// ======================================================

#ifdef onika_poweroftwo_TEST

#include <iostream>
using namespace std;
using namespace onika;
int main()
{
	cout<<"NextPowOfTwoLog(0) = "<< NextPowOfTwoLog<0>::value <<endl;
	cout<<"NextPowOfTwoLog(1) = "<< NextPowOfTwoLog<1>::value <<endl;
	cout<<"NextPowOfTwoLog(2) = "<< NextPowOfTwoLog<2>::value <<endl;
	cout<<"NextPowOfTwoLog(3) = "<< NextPowOfTwoLog<3>::value <<endl;
	cout<<"NextPowOfTwoLog(4) = "<< NextPowOfTwoLog<4>::value <<endl;
	cout<<"NextPowOfTwoLog(7) = "<< NextPowOfTwoLog<7>::value <<endl;
	cout<<"NextPowOfTwoLog(33) = "<< NextPowOfTwoLog<33>::value <<endl;
	cout<<"NextPowOfTwoLog(5177) = "<< NextPowOfTwoLog<5177>::value <<endl;
	int x=0;
	cin>>x;
	cout<<"nextPowerOfTwoLog("<<x<<") = "<<nextpo2log(x)<<endl;
	return 0;
}

#endif

