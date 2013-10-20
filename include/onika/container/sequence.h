#ifndef __onika_container_sequence_h
#define __onika_container_sequence_h

#include <iterator>

#include "onika/container/iterator.h"

namespace onika { namespace container {

template<typename Integer>
struct SequenceIterator
{
	inline SequenceIterator(Integer _i) : i(_i) {}
	typedef std::ptrdiff_t difference_type;
	typedef Integer value_type;
	typedef Integer pointer;
  	typedef Integer reference;
  	typedef std::random_access_iterator_tag iterator_category;
	inline Integer operator * () const { return i; }
	inline bool operator == (const SequenceIterator& it) const { return it.i==i; }
	inline bool operator != (const SequenceIterator& it) const { return it.i!=i; }	
	inline SequenceIterator& operator += (difference_type n) { i += n; return *this; }
	inline SequenceIterator operator + (difference_type n) const { SequenceIterator it = {static_cast<Integer>(i+n)}; return it; }
	inline SequenceIterator& operator ++ () { ++ i; return *this; }
	inline operator Integer () const { return i; }
	Integer i;
};

template<typename Integer>
struct SequenceContainer
{
	inline SequenceContainer(size_t n) : nelems(n) {}
	inline void set(size_t i, Integer x) {}
	inline Integer get(size_t i) const { return i; }
	inline size_t size() const
	{
		return nelems;
	}
	inline size_t memory_bytes() const
	{
		return sizeof(nelems);
	}

	size_t nelems;

	// generic container API
	ONIKA_CONTAINER_ACCESS_DEF(SequenceContainer,Integer);
};

} } // end of namespace
#endif


// ======================== UNIT TEST =======================
#ifdef onika_container_sequence_TEST

#include <algorithm>
#include <iostream>

int main()
{
	onika::container::SequenceContainer<int> seq(10);

	for( auto i : seq ) { std::cout<<i<<' '; }
	std::cout<<'\n';

	return 0;
}

#endif

