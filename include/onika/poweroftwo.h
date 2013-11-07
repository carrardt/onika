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

#endif

