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

		//************* forward declaration of method selectors ****************
#define MATH_FUNC1(name) template <class T> struct C##name##X;
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
		MATH_FUNC1(norm)
		MATH_FUNC1(norm2)
#undef 	MATH_FUNC1
#define MATH_FUNC2(name) template <class T1, class T2> struct C##name##X;
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
		MATH_FUNC2(dot)
		MATH_FUNC2(distance)
		MATH_FUNC2(distance2)
#undef 	MATH_FUNC2
		//********************************************************

	//************* front-end methods definition ****************
#define MATH_FUNC1(name) \
		template <class T> static inline auto name(const T& x) \
		ONIKA_AUTO_RET( C##name##X<T>::name##X(x) )
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
		MATH_FUNC1(norm)
		MATH_FUNC1(norm2)
#undef 	MATH_FUNC1

#define MATH_FUNC2(name) \
		template <class T1, class T2> static inline auto name(const T1& x, const T2& y) \
		ONIKA_AUTO_RET( C##name##XX<T1,T2>::name##XX(x,y) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
		MATH_FUNC2(dot)
		MATH_FUNC2(distance)
		MATH_FUNC2(distance2)
#undef 	MATH_FUNC2
		//********************************************************



		//==========================================================
		//== Scalar versions of unary and binary functions        ==
		//==========================================================
	#define MATH_FUNC1(name,code) \
			template<class T> struct C##name##S { static inline auto name##S(const T& x) ONIKA_AUTO_RET( code ) };
			MATH_FUNC1(negate,(-x))
			MATH_FUNC1(inverse,(1/x))
			MATH_FUNC1(abs,std::abs(x))
			MATH_FUNC1(norm,std::abs(x))
			MATH_FUNC1(norm2,(x*x))
	#undef 	MATH_FUNC1
	#define MATH_FUNC2(name,code) \
			template<class T1,class T2> struct C##name##SS { static inline auto name##SS(const T1& x, const T2& y) ONIKA_AUTO_RET( code ) };
			MATH_FUNC2(sub,(x-y))
			MATH_FUNC2(add,(x+y))
			MATH_FUNC2(mul,(x*y))
			MATH_FUNC2(div,(x/y))
			MATH_FUNC2(dot,(x*y))
			MATH_FUNC2(distance,std::abs(y-x))
			MATH_FUNC2(distance2,(y-x)*(y-x))
	#undef 	MATH_FUNC2


	// tuple size helper template
	template<class T> struct TupleLength { static constexpr int value=-1; };
	template<class... T> struct TupleLength< std::tuple<T...> > { static constexpr int value=sizeof...(T); };

	// macros for tuple handling
#define MT(x...) std::make_tuple(x)
#define TA(x,y) std::tuple_cat(x,MT(y))
#define TG(t,i) std::get<i>(t)
#define TS(t) (TupleLength<t>::value)


	//==========================================================
	//== Select tuple/non tuple operators for unary functions ==
	//==========================================================
#define MATH_FUNC1(name) \
	template <class T> struct C##name##X { static inline auto name##X(const T& x) ONIKA_AUTO_RET( C##name##S::name##S(x) ) };
	MATH_FUNC1(negate)
	MATH_FUNC1(inverse)
	MATH_FUNC1(abs)
	MATH_FUNC1(norm)
	MATH_FUNC1(norm2)
#undef MATH_FUNC1

#define MATH_FUNC1(name) \
	template <class... T> struct C##name##X<std::tuple<T...> > \
	{ static inline auto name##X(const T& x) ONIKA_AUTO_RET( C##name##T::name##T(x) ) };
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
		MATH_FUNC1(norm)
		MATH_FUNC1(norm2)
#undef MATH_FUNC1

	//===========================================================
	//== Select tuple/non tuple operators for binary functions ==
	//===========================================================
	// both operands are something other than tuples
#define MATH_FUNC2(name) \
	template <class T1,class T2> struct C##name##XX \
	{ static inline auto name##XX(const T1& x, const T2& x) ONIKA_AUTO_RET( C##name##SS::name##SS(x) ) };
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
		MATH_FUNC2(dot)
		MATH_FUNC2(distance)
		MATH_FUNC2(distance2)
#undef 	MATH_FUNC2
	};

	// second operand is a tuple and first is not
#define MATH_FUNC2(name) \
	template <class T1,class... T2> struct C##name##XX<T1,std::tuple<T2...> > \
	{ static inline auto name##XX(const T1& x, const std::tuple<T2...>& y) ONIKA_AUTO_RET( C##name##TS::name##TS(y,x) ) };
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
#undef 	MATH_FUNC2
	};

	// first operand is a tuple and second is not
#define MATH_FUNC2(name) \
	template <class... T1, class T2> struct C##name##XX<std::tuple<T1...>,T2> \
	{ static inline auto name##XX(const std::tuple<T1...>& x, const T2& y) ONIKA_AUTO_RET( C##name##TS::name##TS(x,y) ) };
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2


#define MATH_FUNC2(name) \
		template <class T1, class T2, bool EQUAL> struct C##name##TTE { int name##TTE; }; \
		template <class T1, class T2> struct C##name##TTE<T1,T2,true> \
		{ static inline auto name##TTE(const T1& x, const T2& y) ONIKA_AUTO_RET( C##name##TT::name##TT(x,y) ) };
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
#undef 	MATH_FUNC2

	static inline auto distance2_eqtuple(const T1& x,const T2& y)
	ONIKA_AUTO_RET( UnaryFunc<decltype(sub_eqtuple(x,y))>::norm2_selector( sub_eqtuple(x,y) ) )

	static inline auto distance_eqtuple(const T1& x,const T2& y)
	ONIKA_AUTO_RET( std::sqrt(distance2_eqtuple(x,y)) )

	// both operands are tuples and have the same size
	template <class... T1, class... T2> struct BinaryFunc< std::tuple<T1...>, std::tuple<T2...> >
	{
#define MATH_FUNC2(name) \
	static inline auto name##_selector(const std::tuple<T1...>& x, const std::tuple<T2...>& y) \
	ONIKA_AUTO_RET( TupleBinaryFunc<std::tuple<T1...>,std::tuple<T2...>,sizeof...(T1)==sizeof...(T2)>::name##_eqtuple(x,y) )
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
	MATH_FUNC2(distance)
	MATH_FUNC2(distance2)
#undef 	MATH_FUNC2
	};
	//********************************************************

	template <class T,unsigned int I>
	struct TMI
	{
#define MATH_FUNC1(name) \
		static inline auto name##_t(const T& x) \
		ONIKA_AUTO_RET( TA( PTMI::name##_t(x) , onika::math::name(TG(x,I)) ) )
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
#undef 	MATH_FUNC1
	};

	template <class T>
	struct TMI<T,0>
	{
#define MATH_FUNC1(name) \
		static inline auto name##_t(const T& x) \
		ONIKA_AUTO_RET( MT(onika::math::name(TG(x,0))) )
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
#undef 	MATH_FUNC1
	};

	template <class T1, class T2, unsigned int I>
	struct TTMI
	{
#define MATH_FUNC2(name) \
		static inline auto name##_tt(const T1& x,const T2& y) \
		ONIKA_AUTO_RET( TA( PTTMI::name##_tt(x,y) , onika::math::name(TG(x,I),TG(y,I)) ) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
		static inline auto dot_tt(const T1& x, const T2& y)
		ONIKA_AUTO_RET( onika::math::add( PTTMI::dot_tt(x,y) , onika::math::dot(TG(x,I),TG(y,I)) ) )
	};

	template <class T1, class T2>
	struct TTMI<T1,T2,0>
	{
#define MATH_FUNC2(name) \
		static inline auto name##_tt(const T1& x,const T2& y) ONIKA_AUTO_RET( MT( onika::math::name(TG(x,0),TG(y,0)) ) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
		static inline auto dot_tt(const T1& x, const T2& y) ONIKA_AUTO_RET( onika::math::dot(TG(x,0),TG(y,0)) )
	};

	template <class T1, class T2, unsigned int I>
	struct TSMI
	{
#define MATH_FUNC2(name) \
		static inline auto name##_ts(const T1& x,const T2& y) \
		ONIKA_AUTO_RET( TA( PTSMI::name##_ts(x,y) , onika::math::name(TG(x,I),y) ) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
	};

	template <class T1, class T2>
	struct TSMI<T1,T2,0>
	{
#define MATH_FUNC2(name) \
		static inline auto name##_ts(const T1& x,const T2& y) ONIKA_AUTO_RET( MT( onika::math::name(TG(x,0),y) ) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
	};

#undef MT
#undef TA
#undef TG
#undef TS
#undef TMI
#undef PTMI
#undef TTMI
#undef PTTMI
#undef TSMI
#undef PTSMI

} }

#define ONIKA_USE_MATH \
using onika::math::negate; \
using onika::math::inverse; \
using onika::math::abs; \
using onika::math::sub; \
using onika::math::add; \
using onika::math::mul; \
using onika::math::div; \
using onika::math::dot; \
using onika::math::distance2; \
using onika::math::distance; \
using onika::math::norm2; \
using onika::math::norm

#endif

