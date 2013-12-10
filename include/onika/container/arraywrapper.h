#ifndef __onika_container_arraywrapper_h
#define __onika_container_arraywrapper_h

#include <cstddef>
#include <iterator>
#include <assert.h>

#include "onika/container/tuplevec.h"

namespace onika { namespace container {

	template<class T,int ElementStride=1>
	struct PointerIteratorT
	{
		typedef std::ptrdiff_t difference_type;
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef const T* const_pointer;
		typedef const T& const_reference;
		typedef std::random_access_iterator_tag iterator_category;

		inline PointerIteratorT(T* p) : ptr(p) {}
		inline PointerIteratorT(const PointerIteratorT& it) : ptr(it.ptr) {}
		inline PointerIteratorT& operator = (const PointerIteratorT& it) { ptr = it.ptr; return *this; }

		inline T& operator * () { return *ptr; }
		inline const T& operator * () const { return *ptr; }

		inline bool operator < (const PointerIteratorT& it) const { return ptr < it.ptr; }
		inline bool operator == (const PointerIteratorT& it) const { return ptr == it.ptr; }
		inline bool operator != (const PointerIteratorT& it) const { return ptr != it.ptr; }
		inline PointerIteratorT& operator ++ () { ptr+=ElementStride; return *this; }
		inline PointerIteratorT operator ++ (int) { PointerIteratorT it(*this); ptr+=ElementStride; return it; }
		inline PointerIteratorT& operator -- () { ptr-=ElementStride;	return *this; }
		inline PointerIteratorT operator -- (int) { PointerIteratorT it(*this); ptr-=ElementStride; return it; }
		inline PointerIteratorT operator - (difference_type n) const { PointerIteratorT it(*this); it.ptr-=n*ElementStride; return it; }
		inline PointerIteratorT& operator -= (difference_type n) { ptr-=n*ElementStride; return *this; }
		inline PointerIteratorT operator + (difference_type n) const { PointerIteratorT it(*this); it.ptr+=n*ElementStride; return it; }
		inline PointerIteratorT& operator += (difference_type n)	{ ptr+=n*ElementStride; return *this; }
		inline difference_type operator - (PointerIteratorT& it) const { return (ptr - it.ptr) / ElementStride;	}

		pointer ptr;
	};

	template<class T,int ElementStride=1>
	struct ArrayWrapper
	{
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef PointerIteratorT<T> iterator;
		typedef const PointerIteratorT<T> const_iterator;
		typedef typename std::iterator_traits<iterator>::difference_type difference_type;
		typedef size_t size_type;
		typedef typename std::reverse_iterator<iterator> reverse_iterator;

		//! Warning: if ElementStride<>1 using begin() and end() will result in undefined behavior
		inline iterator begin() { return iterator(dataptr); }
		inline iterator end() { return iterator(dataptr+size()*ElementStride); }

		inline size_t capacity() const { return datacapacity; }
		inline size_t size() const { return datasize; }
		inline void resize(size_t n)
		{
			assert(n<=datacapacity);
			datasize = n;
		}
		inline void clear() { datasize=0; }

		inline T& operator [] (const size_t i) { return dataptr[i*ElementStride]; }
		inline const T& operator [] (const size_t i) const { return dataptr[i*ElementStride]; }

		T* dataptr;
		size_t datasize;
		size_t datacapacity;
	};

	template<class T,int S=1>
	ArrayWrapper<T,S> array_wrapper( T* pointer, size_t maxSize) { ArrayWrapper<T,S> aw = {pointer,maxSize,maxSize}; return aw; }

	// wrapper for arrays that describe tuple arrays with tuple elements serialized in array.
	// i.e. : building a 3-tuple wrapper from an array containing x1,y1,z1,x2,y2,z2,...,xN,yN,zN
	// will result in a container of 3-tuples (x1,y1,z1) , (x2,y2,z2) , ... , (xN,yN,zN)
	template<class T,unsigned int... Indices>
	auto flat_tuple_array_wrapper_aux( T* p, size_t sz, onika::tuple::TupleIndices<Indices...> ignored )
	ONIKA_AUTO_RET( zip_vectors_cpy( array_wrapper<T,sizeof...(Indices)>(p+Indices,sz) ... ) )

	template<unsigned int N,class T>
	auto flat_tuple_array_wrapper( T* p, size_t sz)
	ONIKA_AUTO_RET( flat_tuple_array_wrapper_aux(p,sz,onika::tuple::make_tuple_indices<N>() ) )

} }

#endif
