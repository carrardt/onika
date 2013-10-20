#ifndef __onika_it_type_h
#define __onika_it_type_h

template<typename Iterator> struct it_type { typedef Iterator T; };
template<typename Type> struct it_type<Type*> { typedef Type* T; };

#endif

#ifdef onika_it_type_TEST
int main()
{
	it_type<int> x;
	it_type<int*> y;
	it_type<int[16]> z;
	return 0;
}

#endif

