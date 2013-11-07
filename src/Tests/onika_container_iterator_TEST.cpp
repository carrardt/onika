#include "onika/container/iterator.h"

#include <iostream>
#include "onika/container/algorithm.h"

template<typename T>
struct MyArrayWrapper
{
	typedef T ElementType;
	
	// generic container API
	ONIKA_CONTAINER_ACCESS_DEF(MyArrayWrapper,ElementType);	
	
	inline size_t size() const { return n; }
	
	inline void set(size_t i, const value_type & x) const { data[i]=x; }
	inline value_type get(size_t i) const { return data[i]; }

	size_t n;
	value_type * data;
};


#include <algorithm>
#include <vector>

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand48(seed);

	MyArrayWrapper<int> array;
	array.n = 100;
	array.data = new int[array.n];
	for(int i=0;i<array.n;i++)
	{
		array[i] = rand()/10;
	}

	std::sort( array.begin(), array.end() );
	std::for_each( array.begin(), array.end() , [](int x){std::cout<<x<<' ';} );
	std::cout<<'\n';
	for(int i=0;i<array.n;i++) { array[i] ++; }
	for(int i=0;i<array.n;i++) { array[i] *= 10; }
	std::for_each( array.begin(), array.end() , [](int x){std::cout<<x<<' ';} );
	std::cout<<'\n';

	std::vector<int> vector(100);
	std::copy( array.begin(), array.end(), vector.rbegin() );
	std::sort( vector.begin(), vector.end() );
	std::for_each( vector.begin(), vector.end() , [](int x){std::cout<<x<<' ';} );
	std::cout<<'\n';
	
	std::cout<< array[1] << " + " << array[2] << " = " << ( array[1] + array[2] ) <<"\n";
	
	return 0;
}

 // end of iterator.h

