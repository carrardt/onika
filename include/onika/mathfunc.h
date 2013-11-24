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
		template<class T> static inline auto name##_s(const T& x) ONIKA_AUTO_RET( code )
		MATH_FUNC1(negate,(-x))
		MATH_FUNC1(inverse,(1/x))
		MATH_FUNC1(abs,std::abs(x))
		MATH_FUNC1(norm,std::abs(x))
		MATH_FUNC1(norm2,(x*x))
#undef 	MATH_FUNC1
#define MATH_FUNC2(name,code) \
		template<class T1,class T2> static inline auto name##_ss(const T1& x, const T2& y) ONIKA_AUTO_RET( code )
		MATH_FUNC2(sub,(x-y))
		MATH_FUNC2(add,(x+y))
		MATH_FUNC2(mul,(x*y))
		MATH_FUNC2(div,(x/y))
		MATH_FUNC2(dot,(x*y))
		MATH_FUNC2(distance,std::abs(y-x))
		MATH_FUNC2(distance2,(y-x)*(y-x))
#undef 	MATH_FUNC2

	template<class T1, class T2=T1> struct TupleMath;

	//==========================================================
	//== Select tuple/non tuple operators for unary functions ==
	//==========================================================
	template <class T> struct UnaryFunc
	{
#define MATH_FUNC1(name) \
		static inline auto name(const T& x) ONIKA_AUTO_RET( name##_s(x) )
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
//		MATH_FUNC1(norm)
//		MATH_FUNC1(norm2)
#undef MATH_FUNC1
	};

	template <class... _T> struct UnaryFunc< std::tuple<_T...> >
	{
		typedef std::tuple<_T...> T;
#define MATH_FUNC1(name) \
		static inline auto name(const T& x) ONIKA_AUTO_RET( TupleMath<T>::name##_t(x) )
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
//		MATH_FUNC1(norm)
//		MATH_FUNC1(norm2)
#undef MATH_FUNC1		
	};

	template<class T> struct TupleLength { static constexpr int value=-1; };
	template<class... T> struct TupleLength< std::tuple<T...> > { static constexpr int value=sizeof...(T); };
	/*
	// a partir d'ici c'est important de bien specifier le comprtement tuple/non tuple pour les opérateurs binaires
	template <class T1
		 ,class T2
		 ,bool T1IsTuple=(TupleLength<T1>::value!=-1)
		 ,bool T2IsTuple=(TupleLength<T2>::value!=-1)
		 ,bool EqualSize=(TupleLength<T1>::value==TupleLength<T2>::value)
		 > struct BinaryFunc
	{
		//static constexpr bool T1IsTuple
	};

	// both operands are something other than tuples
	template <class T1, class T2> struct BinaryFunc<T1,T2,false,false,true>
	{
#define MATH_FUNC2(name) \
		static inline auto name(const T1& x, const T2& y) ONIKA_AUTO_RET( onika::math::name##_ss(x,y) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
		MATH_FUNC2(dot)
		MATH_FUNC2(distance)
		MATH_FUNC2(distance2)
#undef 	MATH_FUNC2
	};

	// first opeand is a tuple and second is not
	template <class T1, class T2> struct BinaryFunc<T1,T2,true,false,false>
	{
#define MATH_FUNC2(name) static inline auto name(const T1& x, const T2& y) ONIKA_AUTO_RET( TupleMath<T1,T2>::name##_ts(x,y) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
	};

	// both operands are tuples and have the same size
	template <class T1, class T2> struct BinaryFunc<T1,T2,true,true,true>
	{
#define MATH_FUNC2(name) \
	static inline auto name(const T1& x, const T2& y) ONIKA_AUTO_RET( TupleMath<T1,T2>::name##_tt(x,y) )
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
	MATH_FUNC2(distance)
#undef 	MATH_FUNC2
	};
*/
// front-end methods definition
#define MATH_FUNC1(name) \
	template <class T> static inline auto name(const T& x) ONIKA_AUTO_RET( UnaryFunc<T>::name(x) )
	MATH_FUNC1(negate)
	MATH_FUNC1(inverse)
	MATH_FUNC1(abs)
//	MATH_FUNC1(norm)
//	MATH_FUNC1(norm2)
#undef 	MATH_FUNC1
	/*
#define MATH_FUNC2(name) \
	template <class T1, class T2> static inline auto name(const T1& x, const T2& y) ONIKA_AUTO_RET( BinaryFunc<T1,T2>::name(x,y) )
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
	MATH_FUNC2(distance)
	MATH_FUNC2(distance2)
#undef 	MATH_FUNC2
*/

#define MT(x) std::make_tuple(x)
#define TA(x,y) std::tuple_cat(x,MT(y))
#define TG(t,i) std::get<i>(t)
#define TS(t) (TupleLength<t>::value)
#define TMI TupleMathIterator
#define PTMI TupleMathIterator<T1,T2,I-1>

	template <class T1, class T2=T1,unsigned int I=TS(T1)-1>
	struct TMI
	{
#define MATH_FUNC1(name) static inline auto name##_t(const T1& x) ONIKA_AUTO_RET( TA( PTMI::name##_t(x) , onika::math::name(TG(x,I)) ) )
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
#undef 	MATH_FUNC1
		/*
#define MATH_FUNC2(name) \
		static inline auto name##_tt(const T1& x,const T2& y) \
		ONIKA_AUTO_RET( TA( PTMI::name##_tt(x,y) , onika::math::name(TG(x,I),TG(y,I)) ) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
#define MATH_FUNC2(name) \
		static inline auto name##_ts(const T1& x,const T2& y) \
		ONIKA_AUTO_RET( TA( PTMI::name##_ts(x,y) , onika::math::name(TG(x,I),y) ) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
*/
//	static inline auto dot_tt(const T1& x, const T2& y) ONIKA_AUTO_RET( onika::math::add( PTMI::dot_tt(x,y) , onika::math::mul(TG(x,I),TG(y,I)) ) )
	};

	template <class T1, class T2>
	struct TMI<T1,T2,0>
	{
#define MATH_FUNC1(name) static inline auto name##_t(const T1& x) ONIKA_AUTO_RET( MT(onika::math::name(TG(x,0))) )
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)
#undef 	MATH_FUNC1
		/*
#define MATH_FUNC2(name) \
		static inline auto name##_tt(const T1& x,const T2& y) ONIKA_AUTO_RET( MT( onika::math::name(TG(x,0),TG(y,0)) ) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
#define MATH_FUNC2(name) \
		static inline auto name##_ts(const T1& x,const T2& y) ONIKA_AUTO_RET( MT( onika::math::name(TG(x,0),y) ) )
		MATH_FUNC2(sub)
		MATH_FUNC2(add)
		MATH_FUNC2(mul)
		MATH_FUNC2(div)
#undef 	MATH_FUNC2
*/
//		static inline auto dot_tt(const T1& x, const T2& y) ONIKA_AUTO_RET( onika::math::mul(TG(x,0),TG(y,0)) )
	};

	template <class T1, class T2> struct TupleMath
	{
#define MATH_FUNC1(name) static inline auto name##_t(const T1& x) ONIKA_AUTO_RET( TMI<T1>::name##_t(x) )
	MATH_FUNC1(negate)
	MATH_FUNC1(inverse)
	MATH_FUNC1(abs)
#undef 	MATH_FUNC1
/*
#define MATH_FUNC2(name) \
	static inline auto name##_tt(const T1& x,const T2& y) ONIKA_AUTO_RET( TMI<T1,T2>::name##_tt(x,y) ) \
	static inline auto name##_ts(const T1& x,const T2& y) ONIKA_AUTO_RET( TMI<T1,T2>::name##_ts(x,y) )
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
#undef 	MATH_FUNC2

	static inline auto dot_tt(const T1& x, const T2& y) ONIKA_AUTO_RET( TMI<T1,T2>::dot_tt(x,y) )
	static inline auto norm2_t(const T1& x) ONIKA_AUTO_RET( dot_tt(x,x) )
	static inline auto norm_t(const T1& x) ONIKA_AUTO_RET( std::sqrt(norm2_t(x)) )
	static inline auto distance2_tt(const T1& x,const T2& y) ONIKA_AUTO_RET( norm2_t(sub_tt(x,y)) )
	static inline auto distance_tt(const T1& x,const T2& y) ONIKA_AUTO_RET( std::sqrt(distance2_tt(x,y)) )
	*/
	};
#undef MT
#undef TA
#undef TG
#undef TS
#undef TMI
#undef PTMI

} }

#define ONIKA_USE_MATH \
using onika::math::negate; \
using onika::math::inverse; \
using onika::math::abs
/*
using onika::math::norm2; \
using onika::math::norm; \
using onika::math::sub; \
using onika::math::add; \
using onika::math::mul; \
using onika::math::div; \
using onika::math::dot; \
using onika::math::distance2; \
using onika::math::distance
*/

#endif

