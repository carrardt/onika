#ifndef __onika_container_algorithm_h
#define __onika_container_algorithm_h

#include <limits>
#include <stdlib.h>
#include "onika/language.h"
#include "onika/valuetraits.h"

namespace onika { namespace container {

template<typename _C>
inline auto min_value(const _C& c) 
ONIKA_AUTO_RET( value_traits<typename _C::value_type>::min() )

template<typename _C>
inline auto max_value(const _C& c) 
ONIKA_AUTO_RET( value_traits<typename _C::value_type>::max() )

template<typename _C>
inline size_t memory_bytes(const _C& c)
{
	return sizeof(_C) + sizeof(typename _C::value_type) * c.size() ;
}

// null container
struct NullContainer
{
	inline NullContainer() : s(0) {}
	inline NullContainer(size_t n) : s(n) {}
	inline size_t size() const { return s; }
	inline void resize(size_t n) { s=n; }
	inline void pop_back() { --s; }
	inline int& operator [] (size_t i) { return x; }
	inline const int& operator [] (size_t i) const { return x; }
	size_t s;
	int x;
};


} } // end of namespace



// ======================== UNIT TEST =======================
#ifdef onika_container_algorithm_TEST

#include <iostream>
#include <vector>

int main()
{
	std::vector<int> ivec(10);
	std::cout<<"memory_bytes = " << onika::container::memory_bytes(ivec) << "\n";
	std::cout<<"min_value = " << onika::container::min_value(ivec) << "\n";
	std::cout<<"max_value = " << onika::container::max_value(ivec) << "\n";
	return 0;
}

#endif // end of unit test


#endif // end of algorithm.h

