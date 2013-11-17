#include "onika/tuple.h"
#include <iostream>
#include <functional>
#include "onika/mathfunc.h"

struct AddOne
{
	template<class T> inline auto operator () ( const T& x) const
	ONIKA_AUTO_RET( x+1 )
};

ONIKA_USE_MATH;
ONIKA_USE_TUPLE_MATH;
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
	auto zipseq = onika::tuple::tie_zip( t1, seq );
	//auto ziprep = onika::tuple::zip( t2, onika::tuple::repeat<4,int>( 50 ) ); // works with gcc 4.8, not with intel 13.0	
	auto rep = onika::tuple::repeat<4,int>( 50 );
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

#define T std::make_tuple

#define TEST_TUPLE_BIN_OP(MT1,MT2,T1,op,T2) \
std::cout<<#T1 " " #op " " #T2 " = "<< ( MT1 T1 op MT2 T2 ) << "\n"
	TEST_TUPLE_BIN_OP(T,T, (1,2.5,"Hello") , + , (1,1,1) );
	TEST_TUPLE_BIN_OP(T,,  (1,2.5f,3.0) , > , 2 );
#undef TEST_TUPLE_BIN_OP

#define TEST_TUPLE_BIN_FUNC(MT1,MT2,T1,f,T2) \
std::cout<<#f "( " #T1 " , " #T2 " ) = "<< f(MT1 T1,MT2 T2) << "\n"
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , distance , (1,1,1) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , dot , (1.0,1,1) );
	//TEST_TUPLE_BIN_FUNC(T,T,  (1u,T(2.0f,3.0)) , dot , (1.0,T(1,1)) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , all_equal , (1.0,2u,3L) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , all_equal , (1.0,2.1,3L) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1u,2.0f,3.0) , lexical_order , (1.0,2.1,3L) );
	TEST_TUPLE_BIN_FUNC(T,T,  (1.001f,2.0f,3.0) , lexical_order , (1.0,2.1,3L) );
#undef TEST_TUPLE_BIN_FUNC


	// more complex tuple constructs
	// i.e. a vertex with its scalar value
	std::tuple<float,float,float> ctb1(1.0f,2.0f,3.0f);
	std::tuple<float,float,float> ctb2(1.5f,2.6f,3.7f);
	std::cout<< ctb1 << " - " << ctb2 << " = "<< (ctb1-ctb2) << "\n";

	std::tuple< std::tuple<float,float,float>, float > cta1( ctb1, 4.0f );
	std::tuple< std::tuple<float,float,float>, float > cta2( ctb2, 4.8f );

	std::cout<< std::get<0>(cta1) <<" - " << std::get<0>(cta2) << " = " << (std::get<0>(cta1) - std::get<0>(cta2)) << "\n";

	std::cout<< cta1 <<" - " << cta2 << " = " /*<< (cta1 - cta2)*/ << "\n";


	return 0;
}
 // end of language.h



