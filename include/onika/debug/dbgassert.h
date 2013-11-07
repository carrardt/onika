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


#endif

