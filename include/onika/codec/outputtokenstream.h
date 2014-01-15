#ifndef __onika_codec_outputtokenstream_h
#define __onika_codec_outputtokenstream_h


#include "onika/codec/types.h"

namespace onika { namespace codec {


template<class BitStreamT, class DebugStreamT>
struct OutputTokenStream
{
	inline OutputTokenStream(BitStreamT& bs, DebugStreamT& ds) : bitstream(bs), debugstream(ds) {}




	


	BitStreamT& bitstream;
	DebugStreamT& debugstream;
};

} }

#endif

