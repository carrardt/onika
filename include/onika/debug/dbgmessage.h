#ifndef __dbgmessage_h
#define __dbgmessage_h

#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#else
#include "onika/codec/nullstream.h"
#endif

namespace onika { namespace debug {

#if defined(_DEBUG) || defined(DEBUG)
inline auto dbgmessage() -> decltype(std::cerr)& { return std::cerr; }
#else
inline codec::NullStream dbgmessage() { return codec::NullStream(); }
#endif

} } // namespace

#endif

