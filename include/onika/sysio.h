#ifndef __onika_sysio_h
#define __onika_sysio_h

#include <fstream>
#include "onika/language.h"

namespace onika { namespace sys {

	static std::ofstream* g_stdout = 0;
	static std::ofstream* g_stderr = 0;

	static inline std::ofstream& stdout()
	{
		if( g_stdout == 0 ) { g_stdout = new std::ofstream("/dev/stdout"); }
		return *g_stdout;
	}

	static inline std::ofstream& stderr()
	{
		if( g_stderr == 0 ) { g_stderr = new std::ofstream("/dev/stderr"); }
		return *g_stderr;
	}

	static inline auto info() ONIKA_AUTO_RET( stdout() )
	static inline auto dbg() ONIKA_AUTO_RET( stdout() )
	static inline auto err() ONIKA_AUTO_RET( stderr() )
} }

#endif
