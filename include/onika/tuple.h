#ifndef __onika_tuple_h
#define __onika_tuple_h

#include <cmath>
#include <numeric>

#include <tuple>
#include <type_traits>
#include "onika/language.h"

// only for operator <<
#include <iostream>

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

namespace onika { namespace tuple {


	// ======================== Tuple Indices =======================
	template<int... I>
	struct indices {
		using next = indices<I..., sizeof...(I)>;
		static constexpr int count = sizeof...(I);
	};

	template<int Size>
	struct BuildIndices {
		using type = typename BuildIndices<Size - 1>::type::next;
	};
	template<>
	struct BuildIndices<0> {
		using type = indices<>;
	};
	template<int N> using make_indices = typename BuildIndices<N>::type;

	// ================= Type tuple place holder =====================
	template<class... T>
	struct types
	{
		static constexpr int count = sizeof...(T);
	};


	// ======================== Tuple type check =======================
	template<class T> struct is_tuple { static constexpr bool value=false; };
	template<class... T> struct is_tuple<std::tuple<T...> > { static constexpr bool value=true; };


	// ============ extending a tuple type by one new element ===============
	template<class Tuple, class NewElement> struct GrowTuple {};
	template<class... T, class NewElement> struct GrowTuple<std::tuple<T...>,NewElement>
	{
		using type = std::tuple<T...,NewElement>;
	};

	// ============ make a tuple type with repeated element type ===============
	template<class T, int N>
	struct UniformTuple
	{
		using type = typename GrowTuple< typename UniformTuple<T,N-1>::type , T >::type;
	};
	template<class T>
	struct UniformTuple<T,0>
	{
		using type = std::tuple<>;
	};
	template <class T,int N> using uniform_tuple = typename UniformTuple<T,N>::type;

	// forward declaration for helper template;
	template<int N> struct TupleHelper;


	//======================================
	//== Select tuple/non tuple operators ==
	//======================================
	template <class T1> struct TupleHelperSelector
	{
		template<class T2> static inline bool all_equal(const T1& t1, const T2& t2) { return t1 == t2; }
		template<class T2> static inline bool all_less_or_equal(const T1& t1, const T2& t2) { return t1 <= t2; }
		template<class T2> static inline bool lexical_order(const T1& t1, const T2& t2) { return t1 < t2; }
	};
	template <class... T1> struct TupleHelperSelector< std::tuple<T1...> >
	{
		template<class T2> static inline bool all_equal(const std::tuple<T1...>& t1, const T2& t2)
		{ return TupleHelper<sizeof...(T1)>::all_equal(t1,t2); }

		template<class T2> static inline bool all_less_or_equal(const std::tuple<T1...>& t1, const T2& t2)
		{ return TupleHelper<sizeof...(T1)>::all_less_or_equal(t1,t2); }

		template<class T2> static inline bool lexical_order(const std::tuple<T1...>& t1, const T2& t2)
		{ return TupleHelper<sizeof...(T1)>::lexical_order(t1,t2); }
	};

	// ========== sorting & ordering =======
	template<class T1, class T2>
	inline bool all_equal( const T1& x1, const T2& x2 ) { return TupleHelperSelector<T1>::all_equal(x1,x2); }

	template<class T1, class T2>
	inline bool all_less_or_equal( const T1& x1, const T2& x2 ) { return TupleHelperSelector<T1>::all_less_or_equal(x1,x2); }

	template<class T1, class T2>
	inline bool lexical_order( const T1& x1, const T2& x2 ) { return TupleHelperSelector<T1>::lexical_order(x1,x2); }

	//======================================
	//=========== Helper class ===========
	//======================================
	template<int N> struct TupleHelper
	{
		template<class... Types1, class... Types2>
		static inline bool all_equal(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return TupleHelper<N-1>::all_equal(t1,t2) && tuple::all_equal( std::get<N-1>(t1), std::get<N-1>(t2) );
		}

		template<class... Types1, class... Types2>
		static inline bool all_less_or_equal(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return TupleHelper<N-1>::all_less_or_equal(t1,t2) && tuple::all_less_or_equal( std::get<N-1>(t1), std::get<N-1>(t2) );
		}

		template<class... Types1, class... Types2>
		static inline bool lexical_order(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return TupleHelper<N-1>::lexical_order(t1,t2)
				|| (    TupleHelper<N-1>::all_equal( t1, t2 )
				     && tuple::lexical_order( std::get<N-1>(t1), std::get<N-1>(t2) )
				   );
		}

	};
	template<> struct TupleHelper<1>
	{
		template<class... Types1, class... Types2>
		static inline bool all_equal(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return tuple::all_equal( std::get<0>(t1), std::get<0>(t2) );
		}

		template<class... Types1, class... Types2>
		static inline bool all_less_or_equal(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return tuple::all_less_or_equal( std::get<0>(t1), std::get<0>(t2) );
		}

		template<class... Types1, class... Types2>
		static inline bool lexical_order(const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2)
		{
			return tuple::lexical_order( std::get<0>(t1), std::get<0>(t2) );
		}
	};

	template<> struct TupleHelper<0>
	{
		static inline bool all_equal(const std::tuple<>& t1 , const std::tuple<>& t2) { return true; }
		static inline bool all_less_or_equal(const std::tuple<>& t1 , const std::tuple<>& t2) { return true; }
		static inline bool lexical_order(const std::tuple<>& t1 , const std::tuple<>& t2) { return false;	}
	};

	// ======== apply =============
	// map function to a subset of tuple elements referenced by their indices
	template<class Tuple, class Func, int... I>
	void apply_subset( Tuple& x, Func f, indices<I...> ignored )
	{ ONIKA_NOOP( ONIKA_RETURN_0(f)(std::get<I>(x)) ... ); }

	template<class Func, class... T>
	void apply( std::tuple<T...>& x, Func f )
	{ apply_subset(x,f,make_indices<sizeof...(T)>()); }

	template<class Tuple, class Func, int... I>
	void apply_indexed_subset( Tuple& x, Func f, indices<I...> ignored )
	{ ONIKA_NOOP( ONIKA_RETURN_0(f)(I,std::get<I>(x)) ... ); }

	template<class Func, class... T>
	void apply_indexed( std::tuple<T...>& x, Func f )
	{ apply_indexed_subset(x,f,make_indices<sizeof...(T)>()); }

	template<class Tuple, class Func, int... I>
	void apply_indexed_subset( const Tuple& x, Func f, indices<I...> ignored )
	{ ONIKA_NOOP( ONIKA_RETURN_0(f)(I,std::get<I>(x)) ... ); }

	template<class Func, class... T>
	void apply_indexed( const std::tuple<T...>& x, Func f )
	{ apply_indexed_subset(x,f,make_indices<sizeof...(T)>()); }

	// this guarantees sequential application of f on tuple elements from 0 ton N-1
	template<int N> struct SApplyHelper
	{
		template<class Func, class... T>
		static inline void sapply_indexed(const std::tuple<T...>& t, Func f)
		{ SApplyHelper<N-1>::sapply_indexed(t,f); f(N-1,std::get<N-1>(t)); }

		template<class Func, class... T>
		static inline void sapply(const std::tuple<T...>& t, Func f)
		{ SApplyHelper<N-1>::sapply(t,f); f(std::get<N-1>(t)); }
	};
	template<> struct SApplyHelper<1>
	{
		template<class Func, class... T>
		static inline void sapply_indexed(const std::tuple<T...>& t, Func f)
		{ f(0,std::get<0>(t)); }

		template<class Func, class... T>
		static inline void sapply(const std::tuple<T...>& t, Func f)
		{ f(std::get<0>(t)); }
	};
	template<> struct SApplyHelper<0>
	{
		template<class Func, class T> static inline void sapply_indexed(const T& t, Func f) {}
		template<class Func, class T> static inline void sapply(const T& t, Func f) {}
	};

	template<class Func, class... T>
	void sapply_indexed( const std::tuple<T...>& x, Func f )
	{ SApplyHelper<sizeof...(T)>::sapply_indexed(x,f); }

	template<class Func, class... T>
	void sapply( const std::tuple<T...>& x, Func f )
	{ SApplyHelper<sizeof...(T)>::sapply(x,f); }


	// ============ map ==================
	// map function to a subset of tuple elements referenced by their indices
	template<class Tuple, class Func, int... I>
	auto map( const Tuple& x, Func f, indices<I...> ignored )
	ONIKA_AUTO_RET( std::make_tuple( f(std::get<I>(x)) ... ) )

	template<class Func, class... T>
	auto map( const std::tuple<T...>& x, Func f )
	ONIKA_AUTO_RET( map(x,f,make_indices<sizeof...(T)>()) )

	// ============== zip  ==============
	template<class T1, class T2, int... I>
	auto zip( const T1& t1, const T2& t2, indices<I...> ignored )
	ONIKA_AUTO_RET( std::make_tuple( std::make_tuple(std::get<I>(t1),std::get<I>(t2)) ... ) )

	template<class... Types1, class... Types2>
	inline auto zip( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 )
	ONIKA_AUTO_RET( zip(t1,t2,make_indices<sizeof...(Types1)>()) )

	// ============== tie_zip  ==============
	template<class T1, class T2, int... I>
	auto tie_zip( const T1& t1, const T2& t2, indices<I...> ignored )
	ONIKA_AUTO_RET( std::make_tuple( std::tie(std::get<I>(t1),std::get<I>(t2)) ... ) )

	template<class... Types1, class... Types2>
	inline auto tie_zip( const std::tuple<Types1...>& t1 , const std::tuple<Types2...>& t2 )
	ONIKA_AUTO_RET( tie_zip(t1,t2,make_indices<sizeof...(Types1)>()) )

	// ============ sequence ================
	template<int... I>
	auto sequence( indices<I...> ignored )
	ONIKA_AUTO_RET( std::make_tuple(I...) )

	template<int N> inline auto sequence()
	ONIKA_AUTO_RET( sequence(make_indices<N>()) )

	// ============= repeat ================
	template<class T, int... I>
	auto repeat( const T& x, indices<I...> ignored )
	ONIKA_AUTO_RET( std::make_tuple( ((void)I,x) ... ) )

	template< int N, class T> inline auto repeat(const T& x)
	ONIKA_AUTO_RET( repeat(x,make_indices<N>()) )


	// ========================================
	// =============== print ==================
	// ========================================

	template<class StreamT> struct PrintTupleOp;

	template<class T> struct PrintTuple
	{
		template<class StreamT>
		static inline void print( StreamT& out, const T& x ) { out << x; }
	};

	template<class... T> struct PrintTuple< std::tuple<T...> >
	{
		template<class StreamT>
		static inline StreamT& print( StreamT& out, const std::tuple<T...>& x )
		{
			out<< '(';
			onika::tuple::sapply_indexed( x, onika::tuple::PrintTupleOp<StreamT>(out) );
			out << ')';
			return out;
		}
	};

	template<class StreamT>
	struct PrintTupleOp
	{
		inline PrintTupleOp(StreamT& s) : out(s) {}
		template<class T> inline void operator () ( int i, const T& x ) const
		{
			if(i!=0) { out<<','; }
			onika::tuple::PrintTuple<T>::print(out,x);
		}
		StreamT& out;
	};

	template<class StreamT, class... T>
	inline StreamT& print(StreamT& out, const std::tuple<T...>& t ) { return onika::tuple::PrintTuple<std::tuple<T...> >::print(out,t); }

	template<class... T> inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
	{
		onika::tuple::print( out, t ); return out;
	}

} }

#define ONIKA_USE_TUPLE_OSTREAM \
using onika::tuple::operator <<;

#endif // end of tuple.h



