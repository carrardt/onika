#ifndef __onika_language_h
#define __onika_language_h

#include <type_traits>
#include <tuple>


#define ONIKA_AUTO_RET(E...) -> decltype(E) { return E; }
#define ONIKA_MAKE_SIGNED(T...) typename onika::language::MakeSigned<T>::type
#define ONIKA_NOOP(x...) onika::language::noop(x)
#define ONIKA_CONST(x) std::integral_constant<decltype(x),x>()


namespace onika { namespace language {

	template< typename _Tp, bool _IsInt=std::is_integral<_Tp>::value > struct MakeSigned;
	template< typename _Tp> struct MakeSigned<_Tp,true> { typedef typename std::make_signed<_Tp>::type type; };
	template< typename _Tp> struct MakeSigned<_Tp,false> { typedef _Tp type; };

	template<class... T> inline void noop( const T&... ) {}

} }

// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#endif // end of language.h



