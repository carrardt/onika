#ifndef __onika_mathfunc_h
#define __onika_mathfunc_h

#include <cstdint>
#include <cmath>
#include <numeric>

#include "onika/language.h"
#include "onika/tuple.h"

namespace onika { namespace math {

		//************* forward declaration of method selectors ****************
#define MATH_FUNC1(name) template <class T> struct C##name##X;
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
		MATH_FUNC1(norm)
		MATH_FUNC1(norm2)
#undef 	MATH_FUNC1
#define MATH_FUNC2(name) template <class T1, class T2> struct C##name##XX;
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

	// macros for tuple handling
#define MT(x...) std::make_tuple(x)
#define TA(x,y) std::tuple_cat(x,MT(y))
#define TG(t,i) std::get<i>(t)
#define TS(t) (TupleLength<t>::value)

	// forward declaration of dot product, used by unary function norm2
	template <class T1,class T2,unsigned int I=std::tuple_size<T1>::value-1> struct CdotTT;

	//==========================================================
	//== Tuple versions of unary functions  *version boulot*  ==
	//==========================================================
#define MATH_FUNC1(name) \
	template <class T,unsigned int I=std::tuple_size<T>::value-1> struct C##name##T \
	{ static inline auto name##T(const T& x) ONIKA_AUTO_RET( TA( (C##name##T<T,I-1>::name##T(x)), onika::math::name(TG(x,I)) ) ) }; \
	template <class T> struct C##name##T<T,0> \
	{ static inline auto name##T(const T& x) ONIKA_AUTO_RET( MT(onika::math::name(TG(x,0))) ) };
	MATH_FUNC1(negate)
	MATH_FUNC1(inverse)
	MATH_FUNC1(abs)
#undef MATH_FUNC1

	//==========================================================
	//== Tuple/Tuple versions of binary functions             ==
	//==========================================================
#define MATH_FUNC2(name) \
	template <class T1,class T2,unsigned int I=std::tuple_size<T1>::value-1> struct C##name##TT \
	{ static inline auto name##TT(const T1& x, const T2& y) ONIKA_AUTO_RET( TA( (C##name##TT<T1,T2,I-1>::name##TT(x,y)), onika::math::name(TG(x,I),TG(y,I)) ) ) }; \
	template <class T1,class T2> struct C##name##TT<T1,T2,0> \
	{ static inline auto name##TT(const T1& x, const T2& y) ONIKA_AUTO_RET( MT(onika::math::name(TG(x,0),TG(y,0))) ) };
        MATH_FUNC2(sub)
        MATH_FUNC2(add)
        MATH_FUNC2(mul)
        MATH_FUNC2(div)
#undef MATH_FUNC2

    template <class T1,class T2,unsigned int I> struct CdotTT
	{ static inline auto dotTT(const T1& x, const T2& y) ONIKA_AUTO_RET( onika::math::add( CdotTT<T1,T2,I-1>::dotTT(x,y) , onika::math::dot(TG(x,I),TG(y,I)) )  ) };
	template <class T1,class T2> struct CdotTT<T1,T2,0>
	{ static inline auto dotTT(const T1& x, const T2& y) ONIKA_AUTO_RET( onika::math::dot(TG(x,0),TG(y,0)) ) };

	template <class T> struct Cnorm2T { static inline auto norm2T(const T& x) ONIKA_AUTO_RET( CdotTT<T,T>::dotTT(x,x) ) };
	template <class T> struct CnormT { static inline auto normT(const T& x) ONIKA_AUTO_RET( std::sqrt(Cnorm2T<T>::norm2T(x)) ) };

	template <class T1,class T2> struct Cdistance2TT
	{ static inline auto distance2TT(const T1& x, const T2& y) ONIKA_AUTO_RET( onika::math::norm2( CsubTT<T1,T2>::subTT(x,y) ) ) };
	template <class T1,class T2> struct CdistanceTT
	{ static inline auto distanceTT(const T1& x, const T2& y) ONIKA_AUTO_RET( onika::math::norm( CsubTT<T1,T2>::subTT(x,y) ) ) };


	//==========================================================
	//== Tuple/Scalar versions of binary functions            ==
	//==========================================================
#define MATH_FUNC2(name) \
	template <class T1,class T2,unsigned int I=std::tuple_size<T1>::value-1> struct C##name##TS \
	{ static inline auto name##TS(const T1& x, const T2& y) ONIKA_AUTO_RET( TA( (C##name##TS<T1,T2,I-1>::name##TS(x,y)), onika::math::name(TG(x,I),y) ) ) }; \
	template <class T1,class T2> struct C##name##TS<T1,T2,0> \
	{ static inline auto name##TS(const T1& x, const T2& y) ONIKA_AUTO_RET( MT(onika::math::name(TG(x,0),y)) ) };
        MATH_FUNC2(sub)
        MATH_FUNC2(add)
        MATH_FUNC2(mul)
        MATH_FUNC2(div)
#undef MATH_FUNC2


	//==========================================================
	//== Select tuple/non tuple operators for unary functions ==
	//==========================================================
#define MATH_FUNC1(name) \
	template <class T> struct C##name##X { static inline auto name##X(const T& x) ONIKA_AUTO_RET( C##name##S<T>::name##S(x) ) };
	MATH_FUNC1(negate)
	MATH_FUNC1(inverse)
	MATH_FUNC1(abs)
	MATH_FUNC1(norm)
	MATH_FUNC1(norm2)
#undef MATH_FUNC1

#define MATH_FUNC1(name) \
	template <class... T> struct C##name##X<std::tuple<T...> > \
	{ static inline auto name##X(const std::tuple<T...>& x) ONIKA_AUTO_RET( C##name##T<std::tuple<T...> >::name##T(x) ) };
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
	{ static inline auto name##XX(const T1& x, const T2& y) ONIKA_AUTO_RET( C##name##SS<T1,T2>::name##SS(x,y) ) };
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
	MATH_FUNC2(distance)
	MATH_FUNC2(distance2)
#undef 	MATH_FUNC2

	// second operand is a tuple and first is not
	// only allowed forms are x+(y,z), tranformed to (y,z)+x
	// and x*(y,z) tranformed to (y,z)*x
#define MATH_FUNC2(name) \
	template <class T1,class... T2> struct C##name##XX<T1,std::tuple<T2...> > \
	{ static inline auto name##XX(const T1& x, const std::tuple<T2...>& y) ONIKA_AUTO_RET( C##name##TS<std::tuple<T2...>,T1>::name##TS(y,x) ) };
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
#undef 	MATH_FUNC2

	// first operand is a tuple and second is not
#define MATH_FUNC2(name) \
	template <class... T1, class T2> struct C##name##XX<std::tuple<T1...>,T2> \
	{ static inline auto name##XX(const std::tuple<T1...>& x, const T2& y) ONIKA_AUTO_RET( C##name##TS<std::tuple<T1...>,T2>::name##TS(x,y) ) };
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2


		// both operands are tuples and have the same size
#define MATH_FUNC2(name) \
		template <class T1, class T2, bool EQUAL> struct C##name##TTE { int name##TTE;/*easily recognizable error*/ }; \
		template <class T1, class T2> struct C##name##TTE<T1,T2,true> \
		{ static inline auto name##TTE(const T1& x, const T2& y) ONIKA_AUTO_RET( C##name##TT<T1,T2>::name##TT(x,y) ) }; \
		template <class... T1, class... T2> struct C##name##XX<std::tuple<T1...>,std::tuple<T2...> > \
		{  	static inline auto name##XX(const std::tuple<T1...>& x,const std::tuple<T2...>& y) \
			ONIKA_AUTO_RET( C##name##TTE<std::tuple<T1...>,std::tuple<T2...>,sizeof...(T1)==sizeof...(T2)>::name##TTE(x,y) )  };
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
		MATH_FUNC2(dot)
		MATH_FUNC2(distance)
		MATH_FUNC2(distance2)
#undef 	MATH_FUNC2

#undef MT
#undef TA
#undef TG
#undef TS

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
