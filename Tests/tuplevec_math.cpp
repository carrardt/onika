#include "onika/tuple.h"
#include "onika/mathfunc.h"
#include "onika/container/iterator.h"
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

#if 0
namespace onika { namespace math {
inline auto vertexDistance( const MyVertexContainer& vertices, int a, int b )
ONIKA_AUTO_RET( onika::math::distance(
	  std::get<0>( static_cast<typename MyVertexContainer::value_type>(vertices[a]) )
	, std::get<0>( static_cast<typename MyVertexContainer::value_type>(vertices[b]) )
	) )
} }
#endif



ONIKA_USE_MATH;
ONIKA_USE_TUPLE_MATH;

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
	for( auto x : c1 ) { std::cout<< x <<"\n"; }
	std::cout<<"distance between 5 and 7 : "<< distance( c1[5], c1[7] )<<"\n";

	return 0;
}


