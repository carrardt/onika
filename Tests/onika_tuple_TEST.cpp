#include "onika/tuple.h"
#include <iostream>
#include <functional>

struct AddOne
{
	template<class T> inline auto operator () ( const T& x) const
	ONIKA_AUTO_RET( x+1 )
};

ONIKA_USE_TUPLE_OSTREAM

int main()
{
	int x=10;
	double y=0.1;
	const char* z="Hello";
	int w[10] = {0,1,2,3,4,5,6,7,8,9};

	auto num = std::make_tuple(3,5,0.2);
	std::cout<<"is_arithmetic( (3,5,0.2) ) = "<< std::is_arithmetic< decltype(num) >::value <<"\n";
	auto alnum = std::make_tuple(3,"AA");
	std::cout<<"is_arithmetic( (3,'AA') ) = "<< std::is_arithmetic< decltype(alnum) >::value <<"\n";

	//std::cout<<"norm(3.5)="<<onika::math::norm(3.5)<<"\n";
	//std::cout<<"norm((3,4))="<<onika::math::norm(std::make_tuple(3,4))<<"\n";
	std::cout<<"sqrt(2.5) = "<< std::sqrt(2.5)<<"\n";

	auto t1 = std::tie( x, y, z, w );
	auto t2 = std::make_tuple( std::ref(x), y, z, w );
	auto seq = onika::tuple::sequence<4>();
	std::cout<<"seq="<<seq<<"\n";
	auto zipseq = onika::tuple::tie_zip( t1, seq );
	//auto ziprep = onika::tuple::zip( t2, onika::tuple::repeat<4,int>( 50 ) ); // works with gcc 4.8, not with intel 13.0	
	auto rep = onika::tuple::repeat<4,int>( 50 );
	std::cout<<"rep="<<rep<<"\n";
	auto ziprep = onika::tuple::zip( t2, rep );
	auto tt = std::tie( std::get<0>(t1) );
	
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";	
	
	std::get<0>( t1 ) = 11;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	std::get<0>( t2 ) = 12;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	x = 13;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	y = 0.2;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	std::get<0>( std::get<3>(ziprep) ) [9]=19;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";
	
	std::get<0>(tt) = 14;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	std::get<0>( std::get<0>(ziprep) ) = 15;
	std::cout<<"x="<<x<<", y="<<y<<", z="<<z<<", w[9]="<<w[9]<<"\n";
	std::cout<<"t1="<< t1 <<"\n";
	std::cout<<"t2="<< t2 <<"\n";
	std::cout<<"zipseq="<<zipseq <<"\n";
	std::cout<<"ziprep="<<ziprep <<"\n";
	std::cout<<"\n";

	auto plusone = onika::tuple::map( t1 , AddOne() );
	std::cout<<"plusone="<< plusone <<"\n";

	std::cout<<"Empty tuple: " << std::make_tuple() << "\n";
	std::cout<<"1-uple: "<<  std::make_tuple(1) << "\n";
	std::cout<<"tuple of tuples "<<  std::make_tuple(1,std::make_tuple("Thierry",'G',"Carrard"),2.0) << "\n";

	// next gen tuple funcs
	auto tx = std::make_tuple(1.0,"Hello",3u);
	auto ty = onika::tuple::map( tx, AddOne() );
	std::cout<<ty<<"\n";


	return 0;
}
 // end of language.h



