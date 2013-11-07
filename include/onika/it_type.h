#ifndef __onika_it_type_h
#define __onika_it_type_h

template<typename Iterator> struct it_type { typedef Iterator T; };
template<typename Type> struct it_type<Type*> { typedef Type* T; };

#endif

#endif

