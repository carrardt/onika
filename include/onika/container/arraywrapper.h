#ifndef __onika_container_arraywrapper_h
#define __onika_container_arraywrapper_h

#include <cstddef>
#include <iterator>
#include <assert.h>

namespace onika { namespace container {

	template<class T>
	struct ArrayWrapper
	{
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T*  iterator;
		typedef const T* const_iterator;
		typedef typename std::iterator_traits<iterator>::difference_type difference_type;
		typedef size_t size_type;
		typedef typename std::reverse_iterator<iterator> reverse_iterator;

		inline T* begin() { return dataptr; }
		inline const T* begin() const { return dataptr; }
		inline T* end() { return dataptr+size(); }
		inline const T* end() const { return dataptr+size(); }

		inline size_t capacity() const { return datacapacity; }
		inline size_t size() const { return datasize; }
		inline void resize(size_t n)
		{
			assert(n<=datacapacity);
			datasize = n;
		}
		inline void clear() { datasize=0; }

		inline T& operator [] (const size_t i) { return dataptr[i]; }
		inline const T& operator [] (const size_t i) const { return dataptr[i]; }

		T* dataptr;
		size_t datasize;
		size_t datacapacity;
	};

	template<class T>
	ArrayWrapper<T> array_wrapper( T* pointer, size_t maxSize) { ArrayWrapper<T> aw = {pointer,maxSize,maxSize}; return aw; }

} }

#endif
