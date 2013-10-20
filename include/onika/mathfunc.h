#ifndef __onika_mathfunc_h
#define __onika_mathfunc_h

#include <cstdint>

namespace onika { namespace math {

#define MATH_FUNC1(name,code) \
inline int name(int x) { return code; } \
inline int64_t name(int64_t x) { return code; } \
inline float name(float x) { return code; } \
inline double name(double x) { return code; }

#define MATH_FUNC2(name,code) \
inline int name(int x,int y) { return code; } \
inline int64_t name(int64_t x,int64_t y) { return code; } \
inline float name(float x, float y) { return code; } \
inline double name(double x, double y) { return code; }

MATH_FUNC1(abs, ( (x>=0) ? x : -x ) )
MATH_FUNC1(norm,abs(x))
MATH_FUNC1(norm2,x*x)
MATH_FUNC2(dot,x*y)
MATH_FUNC2(distance,abs(y-x))
MATH_FUNC2(distance2,(y-x)*(y-x))

#undef MATH_FUNC1
#undef MATH_FUNC2

} }

#define ONIKA_USE_MATH \
using onika::math::dot; \
using onika::math::norm2; \
using onika::math::norm; \
using onika::math::distance2; \
using onika::math::distance


#ifdef onika_mathfunc_TEST
#include <iostream>
int main()
{
	std::cout<<"norm2(5) = "<<onika::math::norm2(5.0)<<"\n";
	std::cout<<"distance(3,5) = "<<onika::math::distance(3,5)<<"\n";
	std::cout<<"abs(-1.2) = "<<onika::math::abs(-1.2)<<"\n";
	return 0;
}
#endif

#endif
