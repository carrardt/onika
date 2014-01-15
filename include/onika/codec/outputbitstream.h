#ifndef __onika_codec_outputbitstream_h
#define __onika_codec_outputbitstream_h

#include "onika/bits.h"
#include <cstdint>

namespace onika { namespace codec {

	template<class StreamT>
	struct StreamOutputOp
	{
		inline StreamOutputOp( StreamT& s ) : stream(s) {}
		template<class T>
		inline void operator () (const T& x) { stream << x; }
		StreamT& stream;
	};

	template<class StreamT, class Word=uint64_t, unsigned int BUFFER_SIZE=128*1024>
	struct OutputBitStream
	{
		static constexpr unsigned int WORD_BITS = sizeof(Word)*8;

		inline OutputBitStream(StreamT& s) : stream(s), cursor_word(0), cursor_bit(0), streampos(0) {}
		inline ~OutputBitStream()
		{
			if(cursor_bit>0) { ++cursor_word; cursor_bit=0; }
			flush();
		}

		inline void flush()
		{
			stream.write((const char*)buffer,cursor_word*sizeof(Word));
			streampos += cursor_word*WORD_BITS;
			if(cursor_bit>0)
			{
				buffer[0] = buffer[cursor_word];
			}
			cursor_word = 0;
		}

		inline uint64_t totalBits() { return streampos+cursor_word*WORD_BITS+cursor_bit; }

		inline void writeBitsUnsafe(Word x, int n)
		{
			Word mask = mask1lsb<Word>(n) << cursor_bit;
			x = ( x << cursor_bit ) & mask;
			buffer[cursor_word] &= ~mask;
			buffer[cursor_word] |= x;
		}

		// writes the n least significant bits of x to stream
		inline void writeBits(Word x, int n)
		{
			int availBits = WORD_BITS - cursor_bit;
			if( availBits < n )
			{
				writeBitsUnsafe( x, availBits );
				cursor_bit = 0;
				++cursor_word;
				if( cursor_word >= BUFFER_SIZE ) { flush(); }
				x >>= availBits;
				n -= availBits;
			}
			writeBitsUnsafe( x , n );
			cursor_bit += n;
			if( cursor_bit == WORD_BITS )
			{
				cursor_bit=0;
				++cursor_word;
				if( cursor_word >= BUFFER_SIZE ) { flush(); }				
			}
		}


		StreamT& stream;
		uint64_t buffer[BUFFER_SIZE];
		uint64_t streampos;
		uint32_t cursor_word;
		uint32_t cursor_bit;
	};

} } // namespace

#endif


