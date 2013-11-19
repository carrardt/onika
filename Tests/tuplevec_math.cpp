#include "onika/tuple.h"
#include "onika/container/iterator.h"
#include "onika/container/tuplevec.h"
#include <iostream>
#include <vector>
#include <tuple>

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

typedef std::vector< std::tuple<
		std::tuple<double,double,double>	// vertex position
		, double				// vertex scalar
		> > MyVertexContainer;

namespace onika { namespace mesh {
template<class IdType>
inline auto vertexDistance( const MyVertexContainer& vertices, IdType a, IdType b )
ONIKA_AUTO_RET(   onika::math::distance( std::get<1>(vertices[a]), std::get<1>(vertices[b]) )   )
} }

int main()
{
	std::vector<int> xvalues(10);
	std::vector<double> yvalues(10);
	std::vector<float> zvalues(10);

	for(int i=0;i<10;i++)
	{
		xvalues[i] = i;
		yvalues[i] = 1.45676546546*i;
		zvalues[i] = 1.625*i;
	}

	auto c1 = onika::container::zip_vectors( xvalues, yvalues, zvalues );
	for( auto x : c1 )
	{
		std::tuple<int,double,float> element = x;
		int i = std::get<0>( element );
		std::cout<< "zip["<<i<<"] = "<<element << "\n";
	}
	//std::cout<<"distance between 5 and 7 : "<< distance( c1[5], c1[7] )<<"\n";

	MyVertexContainer vertices(10);
	for(int i=0;i<10;i++)
	{
		std::get<0>( std::get<0>( vertices[i] ) ) = i*1.1;
		std::get<1>( std::get<0>( vertices[i] ) ) = i*1.2;
		std::get<2>( std::get<0>( vertices[i] ) ) = i*1.3;
		std::get<1>( vertices[i] ) = i*0.3;
	}
	for( int i=0;i<10;i++ )
	{
		std::cout<< "vertices["<<i<<"] = "<<vertices[i] << "\n";
	}
	//std::cout<<"distance between 5 and 7 : "<< onika::mesh::vertexDistance(vertices,5,7) <<"\n";
	
	return 0;
}


