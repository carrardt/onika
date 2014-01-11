#ifndef __dbgmessage_h
#define __dbgmessage_h

#include "onika/sysio.h"
#include "onika/codec/nullstream.h"
#include "onika/language.h"

namespace onika { namespace debug {

#if defined(_DEBUG) || defined(DEBUG)
inline auto dbgmessage() ONIKA_AUTO_RET( onika::sys::dbg() )
#else
inline codec::NullStream dbgmessage() { return codec::NullStream(); }
#endif

} } // namespace

#endif

