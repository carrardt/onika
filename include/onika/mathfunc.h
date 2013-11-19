#ifndef __onika_mathfunc_h
#define __onika_mathfunc_h

#include <cstdint>
#include <cmath>
#include <numeric>

#include "onika/language.h"


// extends definition of what an arithmetic type is to tuples. in particular tuple of arithmetics are arithmetic
namespace std
{
	template<class T>
	class is_arithmetic< std::tuple<T> > : public std::is_arithmetic<T> {};

	template<class T, class... Types>
	class is_arithmetic< std::tuple<T,Types...> > : public std::integral_constant<bool,
		   is_arithmetic<T>::value
		&& is_arithmetic<std::tuple<Types...> >::value
		> {};
}


namespace onika { namespace math {

// definition of math functions on simple scalars

#define MATH_FUNC1(name,code) \
inline int64_t name##_s(int64_t x) { return code; } \
inline double name##_s(double x) { return code; }

#define MATH_FUNC2(name,code) \
inline int64_t name##_ss(int64_t x,int64_t y) { return code; } \
inline double name##_ss(double x, double y) { return code; }

MATH_FUNC1(negate,-x)
MATH_FUNC1(abs, ( (x>=0) ? x : -x ) )
MATH_FUNC1(norm,abs(x))
MATH_FUNC1(norm2,x*x)
MATH_FUNC2(sub,x-y)
MATH_FUNC2(add,x+y)
MATH_FUNC2(mul,x*y)
MATH_FUNC2(div,x/y)
MATH_FUNC2(dot,x*y)
MATH_FUNC2(distance,abs(y-x))
MATH_FUNC2(distance2,(y-x)*(y-x))

#undef MATH_FUNC1
#undef MATH_FUNC2

	template<unsigned int N> struct TupleMath;

	//==========================================================
	//== Select tuple/non tuple operators for unary functions ==
	//==========================================================
	template <class T> struct UnaryFunctionSelector
	{
#define MATH_FUNC1(name) \
		static inline auto name(const T& x) ONIKA_AUTO_RET( name##_s(x) )
		MATH_FUNC1(negate)
		MATH_FUNC1(abs)
		MATH_FUNC1(norm)
		MATH_FUNC1(norm2)
#undef MATH_FUNC1
	};
	template <class... T> struct UnaryFunctionSelector< std::tuple<T...> >
	{
#define MATH_FUNC1(name) \
		static inline auto name(const std::tuple<T...>& x) ONIKA_AUTO_RET( TupleMath<sizeof...(T)>::name##_t(x) )
		MATH_FUNC1(negate)
		MATH_FUNC1(abs)
		MATH_FUNC1(norm)
		MATH_FUNC1(norm2)
#undef MATH_FUNC1		
	};

	// a partir d'ici c'est important de bien specifier le comprtement tuple/non tuple pour les opérateurs binaires
	template <class T1, class T2,bool SameLength=true> struct BinaryFunctionSelector
	{
#define MATH_FUNC2(name) \
		static inline auto name(const T1& x, const T2& y) ONIKA_AUTO_RET( name##_ss(x) )
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
	MATH_FUNC2(distance)
	MATH_FUNC2(distance2)
#undef MATH_FUNC2
	};

} }

#define ONIKA_USE_MATH \
using onika::math::dot; \
using onika::math::norm2; \
using onika::math::norm; \
using onika::math::distance2; \
using onika::math::distance


#endif
