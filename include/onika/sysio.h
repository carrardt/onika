#ifndef __onika_sysio_h
#define __onika_sysio_h

#include <fstream>
#include "onika/language.h"

namespace onika { namespace sys {

	std::ofstream* stdout = 0;
	std::ofstream* stderr = 0;

	static inline void initialize_sys_io()
	{
		stdout = new std::ofstream("/dev/stdout");
		stderr = new std::ofstream("/dev/stderr");
	}

	static inline auto info() ONIKA_AUTO_RET( *stdout )
	static inline auto dbg() ONIKA_AUTO_RET( *stdout )
	static inline auto err() ONIKA_AUTO_RET( *stderr )

} }

#endif
