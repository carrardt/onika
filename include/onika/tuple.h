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
#define COMP_OP(op,f) \
	template<class... T1, class... T2> inline auto operator op (const std::tuple<T1...>& x, const std::tuple<T2...>& y ) \
	ONIKA_AUTO_RET( TupleHelper<sizeof...(T1)>::f( x , y ) )
#undef COMP_OP
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
#ifdef onika_tuple_TEST
#include <iostream>
#include <functional>
#include "onika/mathfunc.h"

struct AddOne
{
	template<class T> inline auto operator () ( const T& x) const
	ONIKA_AUTO_RET( x+1 )
};

ONIKA_USE_MATH;
ONIKA_USE_TUPLE_MATH;
ONIKA_USE_TUPLE_OSTREAM

int main()
{
	int x=10;
	double y=0.1;
	const char* z="Hello";
	int w[10] = {0,1,2,3,4,5,6,7,8,9};

	auto num = std::make_tuple(3,5,0.2);
	std::cout<<"is_arithmetic( (3,5,0.2) ) = "<< std::is_arithmetic< decltype(num) >::value <<"\n";
	auto alnum = std::make_tuple(3,"AA");
	std::cout<<"is_arithmetic( (3,'AA') ) = "<< std::is_arithmetic< decltype(alnum) >::value <<"\n";

	//std::cout<<"norm(3.5)="<<onika::math::norm(3.5)<<"\n";
	//std::cout<<"norm((3,4))="<<onika::math::norm(std::make_tuple(3,4))<<"\n";
	std::cout<<"sqrt(2.5) = "<< std::sqrt(2.5)<<"\n";

	auto t1 = std::tie( x, y, z, w );
	auto t2 = std::make_tuple( std::ref(x), y, z, w );
	auto seq = onika::tuple::sequence<4>();
	auto zipseq = onika::tuple::tie_zip( t1, seq );
	auto ziprep = onika::tuple::zip( t2, onika::tuple::repeat<4,int>( 50 ) );
	auto tt = std::tie( std::get<0>(t1) );
	
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";	
	
	std::get<0>( t1 ) = 11;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	std::get<0>( t2 ) = 12;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	x = 13;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	y = 0.2;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	std::get<0>( std::get<3>(ziprep) ) [9]=19;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";
	
	std::get<0>(tt) = 14;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	std::get<0>( std::get<0>(ziprep) ) = 15;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	auto plusone = onika::tuple::map( t1 , AddOne() );
	std::cout<<"plusone="<< plusone <<"\n";

	std::cout<<"Empty tuple: " << std::make_tuple() << "\n";
	std::cout<<"1-uple: "<<  std::make_tuple(1) << "\n";
	std::cout<<"tuple of tuples "<<  std::make_tuple(1,std::make_tuple("Thierry",'G',"Carrard"),2.0) << "\n";

#define T std::make_tuple

#define TEST_TUPLE_BIN_OP(MT1,MT2,T1,op,T2) \
std::cout<<#T1 " " #op " " #T2 " = "<< ( MT1 T1 op MT2 T2 ) << "\n"
	TEST_TUPLE_BIN_OP(T,T, (1,2.5,"Hello") , + , (1,1,1) );
	TEST_TUPLE_BIN_OP(T,,  (1,2.5f,3.0) , > , 2 );
#undef TEST_TUPLE_BIN_OP

#define TEST_TUPLE_BIN_FUNC(MT1,MT2,T1,f,T2) \
std::cout<<#f "( " #T1 " , " #T2 " ) = "<< f(MT1 T1,MT2 T2) << "\n"
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , distance , (1,1,1) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , dot , (1.0,1,1) );
	//TEST_TUPLE_BIN_FUNC(T,T,  (1u,T(2.0f,3.0)) , dot , (1.0,T(1,1)) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , all_equal , (1.0,2u,3L) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , all_equal , (1.0,2.1,3L) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , lexical_order , (1.0,2.1,3L) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1.001f,2.0f,3.0) , lexical_order , (1.0,2.1,3L) );
#undef TEST_TUPLE_BIN_FUNC

	return 0;
}
#endif // end of unit test


#endif // end of language.h



