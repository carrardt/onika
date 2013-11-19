#include "onika/mathfunc.h"

ONIKA_USE_MATH

int main()
{

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


