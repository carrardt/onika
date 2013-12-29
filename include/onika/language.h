#ifndef __onika_language_h
#define __onika_language_h

#include <type_traits>
#include <tuple>


#define ONIKA_AUTO_RET(E...) -> decltype(E) { return E; }
#define ONIKA_MAKE_SIGNED(T...) typename onika::language::MakeSigned<T>::type
#define ONIKA_NOOP(x...) onika::language::noop(x)
#define ONIKA_CONST(x) std::integral_constant<decltype(x),x>()
#define ONIKA_RETURN_0(f) onika::language::ret0(f)

namespace onika { namespace language {

	template< typename _Tp, bool _IsInt=std::is_integral<_Tp>::value > struct MakeSigned;
	template< typename _Tp> struct MakeSigned<_Tp,true> { typedef typename std::make_signed<_Tp>::type type; };
	template< typename _Tp> struct MakeSigned<_Tp,false> { typedef _Tp type; };

	template<class... T> inline void noop( const T&... ) {}

	template<class Func>
	struct Ret0
	{
		Func func;
		inline Ret0(Func f) : func(f) {}
		template<class... T>
		inline int operator () (T... x) { func(x...); return 0; }
	};

	template<class Func>
	static inline auto ret0(Func f) ONIKA_AUTO_RET( Ret0<Func>(f) )

} }

// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#endif // end of language.h



