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
#define MATH_FUNC2(name,code) \
	template<class T1,class T2> static inline auto name##_ss(const T1& x, const T2& y) ONIKA_AUTO_RET( code )
	MATH_FUNC1(negate,(-x))
	MATH_FUNC1(inverse,(1/x))
	MATH_FUNC1(abs,((x>=0)?x:-x))
	MATH_FUNC1(norm,((x>=0)?x:-x))
	MATH_FUNC1(norm2,(x*x))
	MATH_FUNC2(sub,(x-y))
	MATH_FUNC2(add,(x+y))
	MATH_FUNC2(mul,(x*y))
	MATH_FUNC2(div,(x/y))
	MATH_FUNC2(dot,(x*y))
	MATH_FUNC2(distance,(((y-x)>=0)?(y-x):(x-y)))
	MATH_FUNC2(distance2,((y-x)*(y-x)))
#undef 	MATH_FUNC1
#undef 	MATH_FUNC2

	template<unsigned int N> struct TupleMath;

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
		MATH_FUNC1(norm)
		MATH_FUNC1(norm2)
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
		MATH_FUNC1(norm)
		MATH_FUNC1(norm2)
#undef MATH_FUNC1		
	};

	template<class T> struct TupleLength { static constexpr int value=-1; };
	template<class... T> struct TupleLength< std::tuple<T...> > { static constexpr int value=sizeof...(T); };
	
	// a partir d'ici c'est important de bien specifier le comprtement tuple/non tuple pour les opérateurs binaires
	template <class T1
		 ,class T2
		 ,bool T1IsTuple=(TupleLength<T1>::value!=-1)
		 ,bool T2IsTuple=(TupleLength<T2>::value!=-1)
		 ,bool EqualSize=(TupleLength<T1>::value==TupleLength<T2>::value)
		 > struct BinaryFunc
	{
	};

	// both operands are something other than tuples
	template <class T1, class T2> struct BinaryFunc<T1,T2,false,false>
	{
#define MATH_FUNC2(name,code) \
	static inline auto name(const T1& x, const T2& y) ONIKA_AUTO_RET( name##_ss(x,y) )
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
	MATH_FUNC2(distance)
	MATH_FUNC2(distance2)
#undef 	MATH_FUNC1
#undef 	MATH_FUNC2
	};

	// first opeand is a tuple and second is not
	template <class T1, class T2> struct BinaryFunc<T1,T2,true,false>
	{
#define MATH_FUNC2(name,code) \
	static inline auto name(const T1& x, const T2& y) ONIKA_AUTO_RET( TupleMath<T1,T2>::name##_ts(x,y) )
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
#undef 	MATH_FUNC1
#undef 	MATH_FUNC2
	};

	// both operands are tuples and have the same size
	template <class T1, class T2> struct BinaryFunc<T1,T2,true,true,true>
	{
#define MATH_FUNC2(name,code) \
	static inline auto name(const T1& x, const T2& y) ONIKA_AUTO_RET( TupleMath<T1,T2>::name##_tt(x,y) )
	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
	MATH_FUNC2(distance)
	MATH_FUNC2(distance2)
#undef 	MATH_FUNC1
#undef 	MATH_FUNC2
	};

#define TUPLECAT_INIT(x) std::make_tuple(x)
#define TUPLECAT_REDUCE(t,x) std::tuple_cat(t,std::make_tuple(x))
#define NO_INIT(x) (x)
#define ADD_REDUCE(t,x) ((t)+(x))

	template <class T1, class T2=T1> struct TupleMath
	{
#define ITERATE_MATH_FUNC1()
		template<int I> static inline auto name##_t_aux(const T1& x) \
		ONIKA_AUTO_RET( reduce( name##_t_aux<I-1>(x) , init(std::get<I>(x)) ) ) \
		static inline auto name##_t_aux<0>(const T1& x) \
		ONIKA_AUTO_RET( init( std::get<0>(x) ) ) \
		static inline auto name##_t(const T1& x) ONIKA_AUTO_RET( name##_t_aux<TupleLength<T1>::value>(x) )
		MATH_FUNC1(negate)
		MATH_FUNC1(inverse)
		MATH_FUNC1(abs)

		static inline auto norm2_t(const T1& x) ONIKA_AUTO_RET( dot_tt(x,x) )
		static inline auto norm_t(const T1& x) ONIKA_AUTO_RET( sqrt(norm2_t(x)) )
	};

} }

#define ONIKA_USE_MATH \
using onika::math::dot; \
using onika::math::norm2; \
using onika::math::norm; \
using onika::math::distance2; \
using onika::math::distance


#endif
