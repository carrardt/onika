#include "onika/container/arraywrapper.h"
#include <iostream>
#include <algorithm>

#include "onika/tuple.h"
#include "onika/container/iterator.h"
#include "onika/container/tuplevec.h"

template<class... T>
inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
{
	onika::tuple::print( out, t );
	return out;
}

template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ConstElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}

template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}

using onika::container::array_wrapper;
using onika::container::zip_vectors;

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand48(seed);

	double array[100];
	double array2[100];
	for(int i=0;i<100;i++) { array[i] = drand48(); array2[i] = drand48();}

	std::cout<<"******** Testing Array wrapper **************\n";
	auto aw = array_wrapper( array, 100 );
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	aw.resize(50);
	std::sort( aw.begin(), aw.end() );
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	aw.resize(25);
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	aw.resize(50);
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	aw.resize(100);
	std::cout<<aw.size()<<" values: "; for( auto x:aw ) { std::cout<<x<<" "; } std::cout<<"\n\n";

	std::cout<<"******** Testing Array zip **************\n";
	auto aw2 = array_wrapper( array2, 100 );
	for(int i=0;i<100;i++)
	{
		std::cout<<'('<<aw[i]<<','<<aw2[i]<<") ";
	} std::cout<<"\n\n";

	auto t1 = zip_vectors(aw, aw2);
	for( auto x:t1 ) { std::cout<< x <<" "; }
	std::cout<<"\n\n";

	t1[10] = std::make_tuple(666.0,-1.0);

	for(int i=0;i<100;i++)
	{
		std::cout<<'('<<aw[i]<<','<<aw2[i]<<") ";
	} std::cout<<"\n\n";

	return 0;
}
