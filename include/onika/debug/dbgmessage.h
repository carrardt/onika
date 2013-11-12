#ifndef __dbgmessage_h
#define __dbgmessage_h

#ifdef _DEBUG
#include <iostream>
#else
#include "onika/codec/nullstream.h"
#endif

namespace onika { namespace debug {

#ifdef _DEBUG
inline auto dbgmessage() -> decltype(std::cerr)& { return std::cerr; }
#else
inline codec::NullStream dbgmessage() { return codec::NullStream(); }
#endif

} } // namespace

#endif

