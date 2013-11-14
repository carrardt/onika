#ifndef __onika_algorithm_basics_h
#define __onika_algorithm_basics_h

#include <algorithm>

namespace onika { namespace algorithm {

template<class Vector>
struct IndexedLess
{
        inline IndexedLess(const Vector& _m) : vec(_m) {}
        template<class Integer>
        inline bool operator () (Integer a, Integer b) const
        {
                return vec[a] < vec[b];
        }
        const Vector& vec;
};

template<class Vector>
inline IndexedLess<Vector> indexed_less(const Vector&v) { return IndexedLess<Vector>(v); }

} }
#endif

