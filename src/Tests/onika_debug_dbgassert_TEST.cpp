#include "onika/debug/dbgassert.h"

int main(int argc, char* argv[])
{
	onika::debug::dbgassert(argc<2);
	return 0;
}

#endif

