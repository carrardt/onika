#ifndef __onika_language_h
#define __onika_language_h

#include <type_traits>
#include <tuple>


#define ONIKA_AUTO_RET(expr) -> decltype(expr) { return expr; }
#define ONIKA_MAKE_SIGNED(T) typename onika::language::MakeSigned<T>::type

namespace onika { namespace language {

	template< typename _Tp, bool _IsInt=std::is_integral<_Tp>::value > struct MakeSigned;
	template< typename _Tp> struct MakeSigned<_Tp,true> { typedef typename std::make_signed<_Tp>::type type; };
	template< typename _Tp> struct MakeSigned<_Tp,false> { typedef _Tp type; };
} }

// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#ifdef onika_language_TEST
#include <iostream>

int main()
{
	unsigned int a = -3;
	ONIKA_MAKE_SIGNED(unsigned long) b = -3;
	ONIKA_MAKE_SIGNED(double) c = -3;
	std::cout<<a<<' '<<b<<' '<<c<<'\n';
	
	return 0;
}
#endif // end of unit test


#endif // end of language.h



