#include "onika/codec/debugstream.h"

#include <iostream>

int main()
{
	{
		onika::codec::DebugStream dbgout( std::cout<<"DEBUG: " );
		dbgout<<"message without newline";
	}
	return 0;
}

#endif

