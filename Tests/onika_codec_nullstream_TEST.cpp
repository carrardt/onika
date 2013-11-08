#include "onika/codec/nullstream.h"
int main()
{
	onika::codec::NullStream ns;
	ns<<"This number "<<65<<" will neve be printed\n";
	return 0;
}

#endif

