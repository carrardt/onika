#ifndef __onika_container_iterator_h
#define __onika_container_iterator_h

#include <stdlib.h>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include "onika/debug/dbgassert.h"
#include "onika/language.h"
#include "onika/mathfunc.h"

namespace onika { namespace container {

/*
 * Random access iterator base.
 * Implements the "element accessor" design pattern. acts mostly like a reference to a virtually stored element
 * WARNING ! every container class is understood as a Random Access container (implementing the [] in constant time)
 */

template<typename Container, bool implement_math_ops = std::is_arithmetic<typename Container::value_type>::value> struct ElementAccessorT;

// ca serait interessant de generaliser le concept pour des accès non random (i.e. forward iterator)
template<typename Container, bool implement_math_ops = std::is_arithmetic<typename Container::value_type>::value >
struct ConstElementAccessorT
{
	typedef typename Container::value_type value_type;
	inline ConstElementAccessorT(const ConstElementAccessorT& ea) : v(ea.v), i(ea.i) {}
	inline ConstElementAccessorT(const Container& _v, const size_t _i) : v(_v), i(_i) {}
	inline value_type get() const { return v.get(i) ; }
	inline operator value_type () const { return get(); }
	const Container &v;
	const size_t i;
};

template<typename Container>
struct ConstElementAccessorT<Container,true>
{
	typedef typename Container::value_type value_type;
	inline ConstElementAccessorT(const ConstElementAccessorT& ea) : v(ea.v), i(ea.i) {}
	inline ConstElementAccessorT(const Container& _v, const size_t _i) : v(_v), i(_i) {}
	inline value_type get() const { return v.get(i) ; }
	inline operator value_type () const { return get(); }
	const Container &v;
	const size_t i;

	// non modifying arithmetic operators should be available through cast conversion
#define BOOL_OP(op) \
	template<class T> inline bool operator op (const ConstElementAccessorT<T>& x) const { return get() op x.get(); } \
	template<class T> inline bool operator op (const ElementAccessorT<T>& x) const { return get() op x.get(); } \
	template<class T> inline bool operator op (const T& x) const { return get() op x; }
	BOOL_OP(<)
	BOOL_OP(<=)
	BOOL_OP(>)
	BOOL_OP(>=)
	BOOL_OP(&&)
	BOOL_OP(||)
#undef BOOL_OP

#define BIN_OP(op) \
	template<class T> inline auto operator op (const ConstElementAccessorT<T>& x) const ONIKA_AUTO_RET( this->get() op x.get() ) \
	template<class T> inline auto operator op (const ElementAccessorT<T>& x) const ONIKA_AUTO_RET( this->get() op x.get() ) \
	template<class T> inline auto operator op (const T& x) const ONIKA_AUTO_RET( this->get() op x )
	BIN_OP(+)
	BIN_OP(-)
	BIN_OP(*)
	BIN_OP(/)
#undef BIN_OP
};

template<typename Container, bool implement_math_ops>
struct ElementAccessorT
{
	typedef typename Container::value_type value_type;
	inline ElementAccessorT(const ElementAccessorT& ea) : v(ea.v), i(ea.i) {}
	inline ElementAccessorT(Container& _v, const size_t _i) : v(_v), i(_i) {}
	inline ElementAccessorT& set(const value_type& x) { v.set(i,x); return *this; }
	inline ElementAccessorT& operator = (const ElementAccessorT& ea) { return set( ea.get() ); }
	inline ElementAccessorT& operator = (const ConstElementAccessorT<Container>& ea) { return set( ea.get() ); }
	inline ElementAccessorT& operator = (const value_type& x) { return set(x); }
	inline value_type get() const { return v.get(i) ; }
	inline operator value_type () const { return get(); }
	Container &v;
	const size_t i;
};

template<typename Container>
struct ElementAccessorT<Container,true>
{
	typedef typename Container::value_type value_type;
	inline ElementAccessorT(const ElementAccessorT& ea) : v(ea.v), i(ea.i) {}
	inline ElementAccessorT(Container& _v, const size_t _i) : v(_v), i(_i) {}
	inline ElementAccessorT& set(const value_type& x) { v.set(i,x); return *this; }
	inline ElementAccessorT& operator = (const ElementAccessorT& ea) { return set( ea.get() ); }
	inline ElementAccessorT& operator = (const ConstElementAccessorT<Container>& ea) { return set( ea.get() ); }
	inline ElementAccessorT& operator = (const value_type& x) { return set(x); }
	inline value_type get() const { return v.get(i) ; }
	inline operator value_type () const { return get(); }
	Container &v;
	const size_t i;
	
	inline ElementAccessorT& operator ++ () { set( get() + 1 ); return *this; }
	inline value_type operator ++ (int) { value_type r=get(); set( get() + 1 ); return r; }
	inline ElementAccessorT& operator -- () { set( get() - 1 ); return *this; }
	inline value_type operator -- (int) { value_type r=get(); set( get() - 1 ); return r; }	

#define ACCUM_OP(op) \
	template<class T> inline ElementAccessorT& operator op##= (const ConstElementAccessorT<T>& x) { set( get() op x.get() ); return *this; } \
	template<class T> inline ElementAccessorT& operator op##= (const ElementAccessorT<T>& x) { set( get() op x.get() ); return *this; } \
	template<class T> inline ElementAccessorT& operator op##= (const T& x) { set( get() op x ); return *this; } 
	ACCUM_OP(+)
	ACCUM_OP(-)
	ACCUM_OP(*)
	ACCUM_OP(/)
#undef ACCUM_OP

	// non modifying arithmetic operators should be available through cast conversion
#define BOOL_OP(op) \
	template<class T> inline bool operator op (const ConstElementAccessorT<T>& x) const { return get() op x.get(); } \
	template<class T> inline bool operator op (const ElementAccessorT<T>& x) const { return get() op x.get(); } \
	template<class T> inline bool operator op (const T& x) const { return get() op x; }
	BOOL_OP(<)
	BOOL_OP(<=)
	BOOL_OP(>)
	BOOL_OP(>=)
	BOOL_OP(&&)
	BOOL_OP(||)
#undef BOOL_OP

#define BIN_OP(op) \
	template<class T> inline auto operator op (const ConstElementAccessorT<T>& x) const ONIKA_AUTO_RET( this->get() op x.get() ) \
	template<class T> inline auto operator op (const ElementAccessorT<T>& x) const ONIKA_AUTO_RET( this->get() op x.get() ) \
	template<class T> inline auto operator op (const T& x) const ONIKA_AUTO_RET( this->get() op x )
	BIN_OP(+)
	BIN_OP(-)
	BIN_OP(*)
	BIN_OP(/)
#undef BIN_OP

};


template<typename Container>
struct IteratorT
{
	typedef std::ptrdiff_t difference_type;
	typedef typename Container::value_type value_type;

	typedef ElementAccessorT<Container> pointer;
  	typedef ElementAccessorT<Container> reference;

  	typedef std::random_access_iterator_tag iterator_category;
	inline IteratorT(Container& _vec, difference_type _i) : vec(_vec), i(_i) {}
	inline IteratorT(const IteratorT& it) : vec(it.vec), i(it.i) {}
	inline IteratorT& operator = (const IteratorT& it) { i = it.i; return *this; }

	inline ElementAccessorT<Container> operator * () { return ElementAccessorT<Container>(vec,i); }
	inline ConstElementAccessorT<Container> operator * () const { return ConstElementAccessorT<Container>(vec,i); }

	inline bool operator < (const IteratorT& it) const { return i < it.i; }
	inline bool operator == (const IteratorT& it) const { return i == it.i; }
	inline bool operator != (const IteratorT& it) const { return i != it.i; }
	inline IteratorT& operator ++ () { ++ i; return *this; }
	inline IteratorT operator ++ (int) { IteratorT it(*this); ++i; return it; }		
	inline IteratorT& operator -- () { --i;	return *this; }
	inline IteratorT operator -- (int) { IteratorT it(*this); --i; return it; }
	inline IteratorT operator - (difference_type n) const { IteratorT it(*this); it.i-=n; return it; }
	inline IteratorT& operator -= (difference_type n) { i-=n;	return *this; }
	inline IteratorT operator + (difference_type n) const { IteratorT it(*this); it.i+=n; return it; }
	inline IteratorT& operator += (difference_type n)	{ i += n; return *this; }
	inline difference_type operator - (IteratorT& it) const { return i - it.i;	}
	
	/*
	friend inline void std::iter_swap(IteratorT<Container> a, IteratorT<Container> b)
    	{
		value_type tmp = a.vec.get(a.i);
		a.vec.set(a.i, b.vec.get(b.i) );
		b.vec.set(b.i, tmp );
   	}
*/

	Container & vec;
	difference_type i;
};

template<typename Container>
struct ConstIteratorT
{
	typedef std::ptrdiff_t difference_type;
	typedef typename Container::value_type value_type;
	typedef ConstElementAccessorT<Container> pointer;
  	typedef ConstElementAccessorT<Container> reference;
  	typedef std::random_access_iterator_tag iterator_category;

	inline ConstIteratorT(const Container& _vec, difference_type _i) : vec(_vec), i(_i) {}
	inline ConstIteratorT(const ConstIteratorT& it) : vec(it.vec), i(it.i) {}

	inline ConstIteratorT& operator = (const ConstIteratorT& it) { i = it.i; return *this; }
	inline ConstElementAccessorT<Container> operator * () const { return ConstElementAccessorT<Container>(vec,i); }
	
	inline bool operator < (const ConstIteratorT& it) const { return i < it.i; }
	inline bool operator == (const ConstIteratorT& it) const { return i == it.i; }
	inline bool operator != (const ConstIteratorT& it) const { return i != it.i; }
	inline ConstIteratorT& operator ++ () { ++ i; return *this; }
	inline ConstIteratorT operator ++ (int) { ConstIteratorT it(*this); ++i; return it; }		
	inline ConstIteratorT& operator -- () { --i;	return *this; }
	inline ConstIteratorT operator -- (int) { ConstIteratorT it(*this); --i; return it; }
	inline ConstIteratorT operator - (difference_type n) const { ConstIteratorT it(*this); it.i-=n; return it; }
	inline ConstIteratorT& operator -= (difference_type n) { i-=n;	return *this; }
	inline ConstIteratorT operator + (difference_type n) const { ConstIteratorT it(*this); it.i+=n; return it; }
	inline ConstIteratorT& operator += (difference_type n)	{ i += n; return *this; }
	inline difference_type operator - (const ConstIteratorT& it) const { return i - it.i;	}

	const Container & vec;
	difference_type i;
};

#define ONIKA_CONTAINER_ACCESS_DEF(C,V) \
	typedef V value_type; \
	typedef onika::container::IteratorT<C> iterator; \
	typedef onika::container::ConstIteratorT<C> const_iterator; \
	inline onika::container::ConstElementAccessorT<C> operator [] (const size_t i) const { return onika::container::ConstElementAccessorT<C>(*this,i); } \
	inline onika::container::ElementAccessorT<C> operator [] (const size_t i) { return onika::container::ElementAccessorT<C>(*this,i); } \
	inline onika::container::ConstIteratorT<C> begin() const { return onika::container::ConstIteratorT<C>(*this,0); } \
	inline onika::container::ConstIteratorT<C> end() const { return onika::container::ConstIteratorT<C>(*this,size()); } \
	inline onika::container::IteratorT<C> begin() { return onika::container::IteratorT<C>(*this,0); } \
	inline onika::container::IteratorT<C> end() { return onika::container::IteratorT<C>(*this,size()); }

#define ONIKA_CONTAINER_PUSH_BACK_DEF(C,V) \
	inline void push_back(const value_type& x) { resize(size()+1,x);  } \
	inline void pop_back() { resize(size()-1);  }

} } // end of namespace

//==========================================
//=== STL algorithm specialization       ===
//==========================================
namespace std
{
	template<class T>
	inline void iter_swap
	( onika::container::IteratorT<T> itA,
	  onika::container::IteratorT<T> itB )
	{
		typename onika::container::IteratorT<T>::value_type a = itA.vec.get(itA.i);
		typename onika::container::IteratorT<T>::value_type b = itB.vec.get(itB.i);
		itA.vec.set( itA.i , b );
		itB.vec.set( itB.i , a );
	}

	template<class T>
	inline void swap
	( onika::container::ElementAccessorT<T> itA,
	  onika::container::ElementAccessorT<T> itB )
	{
		typename onika::container::IteratorT<T>::value_type a = itA.v.get(itA.i);
		typename onika::container::IteratorT<T>::value_type b = itB.v.get(itB.i);
		itA.v.set( itA.i , b );
		itB.v.set( itB.i , a );
	}
}

// ==========================================================
// ====== MATH SPECIALIZATION FOR CONTAINER ACCESSORS =======
// ==========================================================
namespace onika { namespace math {

#define _MATH_FUNC1(name,AccT) \
	template <class T> struct C##name##X<onika::container::AccT<T> > \
	{ static inline auto name##X(onika::container::AccT<T> x) ONIKA_AUTO_RET( onika::math::name(x.get()) ) };

#define _MATH_FUNC2(name,AccT1,AccT2) \
	template <class T1,class T2> struct C##name##XX<onika::container::AccT1<T1>,onika::container::AccT2<T2> > \
	{ static inline auto name##XX(onika::container::AccT1<T1> x, onika::container::AccT2<T2> y) \
	ONIKA_AUTO_RET( onika::math::name(x.get(),y.get()) ) };

#define MATH_FUNC1(name) \
	_MATH_FUNC1(name,ElementAccessorT) \
	_MATH_FUNC1(name,ConstElementAccessorT)

#define MATH_FUNC2(name) \
	_MATH_FUNC2(name,ElementAccessorT,ElementAccessorT) \
	_MATH_FUNC2(name,ElementAccessorT,ConstElementAccessorT) \
	_MATH_FUNC2(name,ConstElementAccessorT,ElementAccessorT) \
	_MATH_FUNC2(name,ConstElementAccessorT,ConstElementAccessorT)

	MATH_FUNC1(negate)
	MATH_FUNC1(inverse)
	MATH_FUNC1(abs)
	MATH_FUNC1(norm)
	MATH_FUNC1(norm2)

	MATH_FUNC2(sub)
	MATH_FUNC2(add)
	MATH_FUNC2(mul)
	MATH_FUNC2(div)
	MATH_FUNC2(dot)
	MATH_FUNC2(distance)
	MATH_FUNC2(distance2)

#undef MATH_FUNC1
#undef _MATH_FUNC1
#undef MATH_FUNC2
#undef _MATH_FUNC2

} }

#endif // end of iterator.h

