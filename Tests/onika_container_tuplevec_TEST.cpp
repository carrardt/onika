#include "onika/container/tuplevec.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <utility>

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

int main()
{
	std::vector<int> xvalues(10);
	std::vector<double> yvalues(10);
	std::vector<float> zvalues(10);

	std::vector<std::string> avalues(10);
	std::vector<char> bvalues(10);
	const char* txt[] = { "Un", "Deux", "Trois", "Quatre", "Cinq", "Six", "Sept", "Huit", "Neuf", "Dix" };

	for(int i=0;i<10;i++)
	{
		xvalues[i] = i;
		yvalues[i] = 1.45676546546*i;
		zvalues[i] = 1.625*i;
		avalues[i] = txt[i];
		bvalues[i] = 'A'+i;
	}

	auto c1 = onika::container::zip_vectors( xvalues, yvalues, zvalues );
	c1.resize( 9 );
	std::tuple<int,double,float> c1InitA( 9, 3.14159, 0.5 );
	c1.push_back( c1InitA );
	//c1.push_back( std::make_tuple(9, 3.14159, 0.5) );

	std::tuple<int,double,float> c1InitB( 12, 1.11, 0.1 );
	c1.resize( 12 , c1InitB );
	//c1.resize( 12 , std::make_tuple( 12, 1.11, 0.1 ) );
	for( auto x : c1 ) { std::cout<< x <<"\n"; }
		
	auto c2 = onika::container::zip_vectors( avalues, bvalues );
	std::tuple<std::string,char> c2Initializer( "Default", 'Z' );
	c2.resize( 15, c2Initializer );
	//c2.resize( 15, std::make_tuple( "Default", 'Z') );
	for( auto x : c2 ) { std::cout<< x <<"\n"; }

	return 0;
}
 // end of file tuplevec.h


