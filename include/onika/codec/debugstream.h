#ifndef __onika_codec_debugstream_h
#define __onika_codec_debugstream_h

#include <iostream>

namespace onika { namespace codec {

	struct DebugStream
	{
		inline DebugStream(std::ostream& s) : stream(s) { }
		inline ~DebugStream() { stream<<std::endl; stream.flush(); }
		template<typename T>
		inline DebugStream& operator << (const T& x) { stream<<x; return *this; }
		std::ostream& stream;
	};
} } // namespace onika::codec

#endif //  __onika_codec_debugstream_h



//============== UNIT TEST ==============
#ifdef onika_codec_debugstream_TEST

#include <iostream>

int main()
{
	{
		onika::codec::DebugStream dbgout( std::cout<<"DEBUG: " );
		dbgout<<"message without newline";
	}
	return 0;
}

#endif

