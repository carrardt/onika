#ifndef __onika_codec_outputtokenstream_h
#define __onika_codec_outputtokenstream_h

#include "onika/codec/types.h"
#include "onika/debug/dbgmessage.h"

namespace onika { namespace codec {


template<class BitStreamT, class DebugStreamT>
struct OutputTokenStream
{
	inline OutputTokenStream(BitStreamT& bs, DebugStreamT& ds) : bitstream(bs), debugstream(ds) {}

	template<class T>
	inline void write( const T& x )
	{
		bitstream << x;
	}

	template<class T>
	inline OutputTokenStream& operator << (const T& x)
	{
		debugstream << "raw token : "<< x <<"\n";
		write(x);
		return *this;
	}

	template<class T>
	inline OutputTokenStream& operator << (const debug::DebugMessage<T>& mesg)
	{
		debugstream << mesg.data;
		return *this;
	}

	inline DebugStreamT& debug() { return debugstream; }

	BitStreamT& bitstream;
	DebugStreamT& debugstream;
};

} }

#endif

