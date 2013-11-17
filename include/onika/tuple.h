#ifndef __onika_tuple_h
#define __onika_tuple_h

#include <cmath>
#include <numeric>

#include <tuple>
#include <type_traits>
#include "onika/language.h"

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

namespace onika { namespace tuple {

	template<unsigned int N> struct TupleHelper;

	//======================================
	//== Select tuple/non tuple operators ==
	//======================================
	template <class T1> struct TupleHelperSelector
	{
		template<class T2> static inline bool all_equal(const T1& t1, const T2& t2) { return t1 == t2; }
		template<class T2> static inline bool lexical_order(const T1& t1, const T2& t2) { return t1 < t2; }
	};
	template <class... T1> struct TupleHelperSelector< std::tuple<T1...> >
	{
		template<class T2> static inline bool all_equal(const std::tuple<T1...>& t1, const T2& t2)
		{ return TupleHelper<sizeof...(T1)>::all_equal(t1,t2); }

		template<class T2> static inline bool lexical_order(const std::tuple<T1...>& t1, const T2& t2)
		{ return TupleHelper<sizeof...(T1)>::lexical_order(t1,t2); }
	};

	// ========== sorting & ordering =======
	template<class T1, class T2>
	inline bool all_equal( const T1& x1, const T2& x2 ) { return TupleHelperSelector<T1>::all_equal(x1,x2); }

	template<class T1, class T2>
	inline bool lexical_order( const T1& x1, const T2& x2 ) { return TupleHelperSelector<T1>::lexical_order(x1,x2); }

	//======================================
	//=========== Helper classes ===========
	//======================================
	template<unsigned int N> struct TupleHelper
	{
		// APPLY
		template <class Func, class... Types>
		static inline void apply( std::tuple<Types...> & t, Func f )
		{
		        TupleHelper<N-1>::apply( t , f );
		        f ( std::get<N-1>( t ) );
		}
		template <class Func, class... Types>
		static inline void apply( const std::tuple<Types...> & t, Func f )
		{
		        TupleHelper<N-1>::apply( t , f );
		        f ( std::get<N-1>( t ) );
		}

		// MAP
		template <class Func, class... Types>
		static inline auto map( const std::tuple<Types...> & t, Func f )
		ONIKA_AUTO_RET( std::tuple_cat( TupleHelper<N-1>::map(t,f) , std::make_tuple( f(std::get<N-1>(t)) ) ) )

		// ZIP
		template<class... Types1, class... Types2>
		static inline auto zip( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 )
		ONIKA_AUTO_RET( std::tuple_cat( TupleHelper<N-1>::zip(t1,t2) , std::make_tuple( std::make_tuple( std::get<N-1>(t1) , std::get<N-1>(t2) ) ) ) )

		// TIE_ZIP
		template<class... Types1, class... Types2>
		static inline auto tie_zip( std::tuple<Types1...>& t1 , std::tuple<Types2...>& t2 )
		ONIKA_AUTO_RET( std::tuple_cat( TupleHelper<N-1>::tie_zip(t1,t2) , std::make_tuple( std::tie( std::get<N-1>(t1) , std::get<N-1>(t2) ) ) ) )

		static inline auto sequence()
		ONIKA_AUTO_RET( std::tuple_cat( TupleHelper<N-1>::sequence() , std::tuple<unsigned int>(N-1) ) )

		template<class T>
		static inline auto repeat(const T& x)
		ONIKA_AUTO_RET( std::tuple_cat( TupleHelper<N-1>::repeat(x) , std::make_tuple(x) ) )

		template<class... Types1, class... Types2>
		static inline bool all_equal(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return TupleHelper<N-1>::all_equal(t1,t2) && tuple::all_equal( std::get<N-1>(t1), std::get<N-1>(t2) );
		}

		template<class... Types1, class... Types2>
		static inline bool lexical_order(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return TupleHelper<N-1>::lexical_order(t1,t2)
				|| (    TupleHelper<N-1>::all_equal( t1, t2 )
				     && tuple::lexical_order( std::get<N-1>(t1), std::get<N-1>(t2) )
				   );
		}

		// =============== MATH ===================
#define BIN_OP(op,f) \
		template<class... Types1, class... Types2> \
		static inline auto f( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 ) \
		ONIKA_AUTO_RET( std::tuple_cat( TupleHelper<N-1>::f(t1,t2) , std::make_tuple( std::get<N-1>(t1) op std::get<N-1>(t2) )  ) ) \
		template<class... Types1, class Type2> \
		static inline auto f( const std::tuple<Types1...>& t1 , const Type2& t2 ) \
		ONIKA_AUTO_RET( std::tuple_cat( TupleHelper<N-1>::f(t1,t2) , std::make_tuple( std::get<N-1>(t1) op t2 )  ) )
		BIN_OP(+,add_op)
		BIN_OP(-,subtract_op)
		BIN_OP(*,multiply_op)
		BIN_OP(/,divide_op)
		BIN_OP(&&,logical_and_op)
		BIN_OP(||,logical_or_op)
		BIN_OP(&,bitwise_and_op)
		BIN_OP(|,bitwise_or_op)
		BIN_OP(<,less_op)
		BIN_OP(<=,less_or_equal_op)
		BIN_OP(>,greater_op)
		BIN_OP(>=,greater_or_equal_op)
		BIN_OP(==,equal_op)
#undef BIN_OP

#define UN_OP(op,f) \
		template<class... T1> inline auto f (const std::tuple<T1...>& t1) \
		ONIKA_AUTO_RET( std::tuple_cat( TupleHelper<N-1>::f(t1) , std::make_tuple( op std::get<N-1>(t1) )  ) )
		UN_OP(-,negate)
		UN_OP(!,logical_not)
		UN_OP(~,bitwise_not)
#undef UN_OP
		template<class... Types1, class... Types2>
		static inline auto dot( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 )
		ONIKA_AUTO_RET( TupleHelper<N-1>::dot(t1,t2) + ( std::get<N-1>(t1) * std::get<N-1>(t2) )  )
	};
	
	template<> struct TupleHelper<1>
	{
		template <class Func, class... Types>
		static inline void apply( std::tuple<Types...> & t, Func f ) { f ( std::get<0>( t ) ); }

		template <class Func, class... Types>
		static inline void apply( const std::tuple<Types...> & t, Func f ) { f ( std::get<0>( t ) ); }

		template <class Func, class... Types>
		static inline auto map( const std::tuple<Types...> & t, Func f )
		ONIKA_AUTO_RET( std::make_tuple( f ( std::get<0>( t ) ) ) )

		template<class... Types1, class... Types2>
		static inline auto zip( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 )
		ONIKA_AUTO_RET( std::make_tuple( std::make_tuple(std::get<0>(t1) , std::get<0>(t2)) )  ) 

		template<class... Types1, class... Types2>
		static inline auto tie_zip( std::tuple<Types1...>& t1 , std::tuple<Types2...>& t2 )
		ONIKA_AUTO_RET( std::make_tuple( std::tie(std::get<0>(t1) , std::get<0>(t2)) )  ) 

		static inline std::tuple<unsigned int> sequence() { return std::tuple<unsigned int>(0); }
		
		template<class T> static inline auto repeat(const T& x) ONIKA_AUTO_RET( std::make_tuple(x) )

		template<class... Types1, class... Types2>
		static inline bool all_equal(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return tuple::all_equal( std::get<0>(t1), std::get<0>(t2) );
		}

		template<class... Types1, class... Types2>
		static inline bool lexical_order(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return tuple::lexical_order( std::get<0>(t1), std::get<0>(t2) );
		}

		// =============== MATH ===================
#define BIN_OP(op,f) \
		template<class... Types1, class... Types2> \
		static inline auto f( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 ) \
		ONIKA_AUTO_RET( std::make_tuple( std::get<0>(t1) op std::get<0>(t2) ) ) \
		template<class... Types1, class Type2> \
		static inline auto f( const std::tuple<Types1...>& t1 , const Type2& t2 ) \
		ONIKA_AUTO_RET( std::make_tuple( std::get<0>(t1) op t2 ) )
		BIN_OP(+,add_op)
		BIN_OP(-,subtract_op)
		BIN_OP(*,multiply_op)
		BIN_OP(/,divide_op)
		BIN_OP(&&,logical_and_op)
		BIN_OP(||,logical_or_op)
		BIN_OP(&,bitwise_and_op)
		BIN_OP(|,bitwise_or_op)
		BIN_OP(<,less_op)
		BIN_OP(<=,less_or_equal_op)
		BIN_OP(>,greater_op)
		BIN_OP(>=,greater_or_equal_op)
		BIN_OP(==,equal_op)
#undef BIN_OP
#define UN_OP(op,f) \
		template<class... T1> inline auto f (const std::tuple<T1...>& t1) \
		ONIKA_AUTO_RET( std::make_tuple( op std::get<0>(t1) ) )
		UN_OP(-,negate)
		UN_OP(!,logical_not)
		UN_OP(~,bitwise_not)
#undef UN_OP
		template<class... Types1, class... Types2>
		static inline auto dot( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 )
		ONIKA_AUTO_RET( std::get<0>(t1) * std::get<0>(t2) )
	};

	template<> struct TupleHelper<0>
	{
		template <class Func, class... Types>
		static inline void apply( std::tuple<Types...> & t, Func f ) {}

		template <class Func, class... Types>
		static inline void apply( const std::tuple<Types...> & t, Func f ) {}

		template <class Func, class... Types>
		static inline std::tuple<> map( const std::tuple<Types...> & t, Func f ) { return std::tuple<>(); }

		template<class... Types1, class... Types2>
		static inline std::tuple<> zip( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 ) { return std::tuple<>(); }

		template<class... Types1, class... Types2>
		static inline std::tuple<> tie_zip( std::tuple<Types1...>& t1 , std::tuple<Types2...>& t2 ) { return std::tuple<>(); }

		static inline std::tuple<> sequence() { return std::tuple<>(); }
		
		template<class T>
		static inline std::tuple<> repeat(const T& x) { return std::tuple<>(); }
	};

	// ======== apply =============
	template<class Functor, class... Types>
	inline void apply( std::tuple<Types...>& t , Functor f )
	{
		TupleHelper<sizeof...(Types)>::apply( t , f );
	}
	template<class Functor, class... Types>
	inline void apply( const std::tuple<Types...>& t , Functor f )
	{
		TupleHelper<sizeof...(Types)>::apply( t , f );
	}

	// ============ map ==================
	template<class Functor, class... Types>
	inline auto map( const std::tuple<Types...>& t , Functor f )
	ONIKA_AUTO_RET( TupleHelper<sizeof...(Types)>::map( t , f ) )

	// ============== zip  ==============
	template<class... Types1, class... Types2>
	inline auto zip( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 )
	ONIKA_AUTO_RET( TupleHelper<sizeof...(Types1)>::zip(t1,t2) )

	// ============== tie_zip  ==============
	template<class... Types1, class... Types2>
	inline auto tie_zip( std::tuple<Types1...>& t1 , std::tuple<Types2...>& t2 )
	ONIKA_AUTO_RET( TupleHelper<sizeof...(Types1)>::tie_zip(t1,t2) )

	// ============ sequence ================
	template<unsigned int N> inline auto sequence() ONIKA_AUTO_RET( TupleHelper<N>::sequence()  )


	// ============= repeat ================
	template<unsigned int N, class T> inline auto repeat(const T& x) ONIKA_AUTO_RET( TupleHelper<N>::repeat(x)  )


	// ========================================
	// =============== print ==================
	// ========================================

	template<class StreamT>
	struct PrintTupleOp
	{
		inline PrintTupleOp(StreamT& s) : out(s) {}
		template<class T> inline StreamT& operator () ( const T& x) const { out<< x << ','; return out;}
		StreamT& out;
	};

	template<class T> struct PrintTuple {};

	template<> struct PrintTuple< std::tuple<> >
	{
		template<class StreamT>
		static inline StreamT& print( StreamT& out, const std::tuple<>& x ) { return out; }
	};

	template<class T> struct PrintTuple< std::tuple<T> >
	{
		template<class StreamT>
		static inline StreamT& print( StreamT& out, const std::tuple<T>& x)
		{
			out << std::get<0>(x) ;
			return out;
		}
	};

	template<class... T> struct PrintTuple< std::tuple<T...> >
	{
		template<class StreamT>
		static inline StreamT& print( StreamT& out, const std::tuple<T...>& x )
		{
			out<< '(';
			TupleHelper< sizeof...(T) - 1 >::apply( x, PrintTupleOp<StreamT>(out) );
			out << std::get< sizeof...(T) - 1 >(x) << ')';
			return out;
		}
	};

	template<class StreamT, class... T>
	inline StreamT& print(StreamT& out, const std::tuple<T...>& t ) { return PrintTuple<std::tuple<T...> >::print(out,t); }

	// ========================================
	// =============== MATH ===================
	// ========================================
#define BIN_OP(op,f) \
	template<class... T1, class... T2> inline auto operator op (const std::tuple<T1...>& x, const std::tuple<T2...>& y ) \
	ONIKA_AUTO_RET( TupleHelper<sizeof...(T1)>::f( x , y ) ) \
	template<class... T1, class T2> inline auto operator op (const std::tuple<T1...>& x, const T2& y ) \
	ONIKA_AUTO_RET( TupleHelper<sizeof...(T1)>::f( x , y ) )
	BIN_OP(+,add_op)
	BIN_OP(-,subtract_op)
	BIN_OP(*,multiply_op)
	BIN_OP(/,divide_op)
	BIN_OP(&&,logical_and_op)
	BIN_OP(||,logical_or_op)
	BIN_OP(&,bitwise_and_op)
	BIN_OP(|,bitwise_or_op)
	BIN_OP(<,less_op)
	BIN_OP(<=,less_or_equal_op)
	BIN_OP(>,greater_op)
	BIN_OP(>=,greater_or_equal_op)
	BIN_OP(==,equal_op)
#undef BIN_OP

#define UN_OP(op,f) \
	template<class... T1> inline auto operator op (const std::tuple<T1...>& x) \
	ONIKA_AUTO_RET( TupleHelper<sizeof...(T1)>::f( x ) )
	UN_OP(-,negate)
	UN_OP(!,logical_not)
	UN_OP(~,bitwise_not)
#undef UN_OP

	template<class... T1, class... T2> inline auto dot (const std::tuple<T1...>& x, const std::tuple<T2...>& y )
	ONIKA_AUTO_RET( TupleHelper<sizeof...(T1)>::dot( x , y ) )
	template<class... T1> inline auto norm2 (const std::tuple<T1...>& x )
	ONIKA_AUTO_RET( TupleHelper<sizeof...(T1)>::dot( x , x ) )
	template<class... T1, class... T2> inline auto distance2 (const std::tuple<T1...>& x , const std::tuple<T2...>& y )
	ONIKA_AUTO_RET( tuple::norm2( y - x ) )
	template<class... T1> inline auto norm (const std::tuple<T1...>& x )
	ONIKA_AUTO_RET( std::sqrt( (double) tuple::norm2( x ) ) )
	template<class... T1, class... T2> inline auto distance (const std::tuple<T1...>& x , const std::tuple<T2...>& y )
	ONIKA_AUTO_RET( std::sqrt( (double) tuple::distance2( x , y ) ) )
	
} }


// ==========================================================
// =============MATH SPECIALIZATION FOR TUPLES ==============
// ==========================================================
namespace onika { namespace math {
#define MATH_FUNC1(name) \
template<class... T1> inline auto name(const std::tuple<T1...>& x) ONIKA_AUTO_RET( tuple::name(x) )
#define MATH_FUNC2(name) \
template<class... T1, class... T2> inline auto name(const std::tuple<T1...>& x, const std::tuple<T2...>& y) ONIKA_AUTO_RET( tuple::name(x,y) )

// MATH_FUNC1(abs)
MATH_FUNC1(norm)
MATH_FUNC1(norm2)
MATH_FUNC2(dot)
MATH_FUNC2(distance)
MATH_FUNC2(distance2)
#undef MATH_FUNC1
#undef MATH_FUNC2
} }

#define ONIKA_USE_TUPLE_MATH \
using onika::tuple::operator +; \
using onika::tuple::operator -; \
using onika::tuple::operator *; \
using onika::tuple::operator /; \
using onika::tuple::operator &&; \
using onika::tuple::operator ||; \
using onika::tuple::operator &; \
using onika::tuple::operator |; \
using onika::tuple::operator !; \
using onika::tuple::operator ~; \
using onika::tuple::operator >; \
using onika::tuple::operator >=; \
using onika::tuple::operator <; \
using onika::tuple::operator <=; \
using onika::tuple::operator ==; \
using onika::tuple::all_equal; \
using onika::tuple::lexical_order

#define ONIKA_USE_TUPLE_OSTREAM \
template<class... T> inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t ) { onika::tuple::print( out, t ); return out; }

// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#endif // end of tuple.h



