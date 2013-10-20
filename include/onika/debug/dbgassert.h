#ifndef __dbgassert_h
#define __dbgassert_h

#include <assert.h>
#include <iostream>

namespace onika { namespace debug {

#ifdef _DEBUG
#define dbgassert(x) _dbgassert(x,#x,__FILE__,__LINE__)
inline void _dbgassert(bool x,const char* condText,const char* file,int line)
	{
		if(!x) { std::cerr<<"Assertion '"<<condText<<"' failed at "<<file<<":"<<line<<std::endl; abort(); }
		//assert(x);
	}
#else
inline void dbgassert(bool) { }
#endif

} } // namespace

#endif


#ifdef onika_debug_dbgassert_TEST

int main(int argc, char* argv[])
{
	onika::debug::dbgassert(argc<2);
	return 0;
}

#endif

