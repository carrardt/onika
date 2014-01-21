#ifndef __onika_codec_inputbitstream_h
#define __onika_codec_inputbitstream_h

#include "onika/codec/encoding.h"
#include "onika/codec/types.h"
#include "onika/codec/debugstream.h"
#include "onika/debug/dbgassert.h"
#include "onika/poweroftwo.h"
#include "onika/bits.h"

#include <map>
#include <vector>

namespace onika { namespace codec {

	template<class _StreamT, class _DbgStreamT>
	struct InputBitStream
	{
		using Stream = _StreamT;
		using DbgStream = _DbgStreamT;
		using Word = uint64_t;
		enum
		{
			WORD_BITS = sizeof(Word)*8,
			MAX_INTEGER_SET_SIZE = 8,
#if defined(_DEBUG) || defined(DEBUG)
			BUFFER_SIZE = 4 
#else
			BUFFER_SIZE = 128*1024 
#endif
			, MAX_STRING_LEN = 256
		}; 

		inline InputBitStream(Stream& s, DbgStream& dbgout )
			: stream(s)
			, dbgstream(dbgout)
			, cursor_word(0)
			, cursor_bit(0)
			, bufferSize(0)
			, streampos(0)
			{
				readsome();
			}

		inline ~InputBitStream()
		{
		}

		inline uint64_t getStreamPosition() { return streampos+cursor_word*WORD_BITS+cursor_bit; }
		inline DebugStream debug() { return DebugStream(dbgstream<<'@'<<getStreamPosition()<<" : "); }

		inline void readsome()
		{
			if( cursor_bit == WORD_BITS )
			{
				++cursor_word;
				cursor_bit=0;
			}
			if( cursor_word == bufferSize )
			{
				streampos += bufferSize;
				bufferSize = ( stream.readsome( (char*)(buffer), BUFFER_SIZE*sizeof(Word) ) * 8 ) / WORD_BITS;
				cursor_word = 0;
			}
		}

		inline Word readBits(unsigned int n)
		{
			debug::dbgassert( n <= WORD_BITS );
			if( n == 0 ) return 0;

			Word x = 0;

			int availBits = (WORD_BITS-cursor_bit);
			int bitOffset = 0;
			if( availBits < n )
			{
				x = buffer[cursor_word];
				x >>= cursor_bit;
				n -= availBits;
				bitOffset = availBits;
				cursor_bit += availBits;
				readsome();
			}
			if( n == 0 ) return x;
			if( bufferSize == 0 )
			{
				debug() << "Premature end of stream";
				return x;
			}
			Word remBits = buffer[cursor_word];
			remBits >>= cursor_bit;
			if( n < WORD_BITS )
			{
				remBits &= mask1lsb<Word>(n);
			}
			x |= remBits << bitOffset;

			// advance stream buffer cursor
			cursor_bit += n;
			readsome();

			return x;
		}

		template<typename DataType>
		inline InputBitStream& operator >> (DataType& x)
		{
			// FIXME: iterate to handle cases where DataType is bigger than Word
			Word w;
			readBits(w,sizeof(DataType)*8);
			x = static_cast<DataType>(w);
			return *this;
		}

		template<typename T>
		inline InputBitStream& operator >> ( BoundedValue<T>& i)
		{
			debug::dbgassert(i.high >= i.low);
			if( i.high > i.low )
			{
				uint64_t range = i.high - i.low + 1;
				int nbits = nextpo2log(range);
				i.x = i.low + readBits( nbits );
			}
			else { i.x = i.low; }
			return (*this);
		}

#define BOUNDED_VALUE_INTEGER_SPECIALIZATION(T) \
		inline InputBitStream& operator >> ( BoundedValue<T>& i) \
		{ \
			debug::dbgassert(i.high >= i.low); \
			if( i.high > i.low ) \
			{ \
				uint64_t range = i.high - i.low + 1; \
				int nbits = nextpo2log(range); \
				i.x = i.low + readBits( nbits ); \
			} else { i.x = i.low; } \
			return (*this); \
		}
		BOUNDED_VALUE_INTEGER_SPECIALIZATION(int8_t)
		BOUNDED_VALUE_INTEGER_SPECIALIZATION(uint8_t)
		BOUNDED_VALUE_INTEGER_SPECIALIZATION(int16_t)
		BOUNDED_VALUE_INTEGER_SPECIALIZATION(uint16_t)
		BOUNDED_VALUE_INTEGER_SPECIALIZATION(int32_t)
		BOUNDED_VALUE_INTEGER_SPECIALIZATION(uint32_t)
		BOUNDED_VALUE_INTEGER_SPECIALIZATION(int64_t)
		BOUNDED_VALUE_INTEGER_SPECIALIZATION(uint64_t)
#undef BOUNDED_VALUE_INTEGER_SPECIALIZATION

		inline InputBitStream& operator >> ( BoundedValue<float>& i){ return (*this)>>i.x; }
		inline InputBitStream& operator >> ( BoundedValue<double>& i){ return (*this)>>i.x; }

		inline InputBitStream& operator >> ( UUIPair& p )
		{
			uint64_t l = 0;
			uint64_t h = uuipair_bound( p.maxvalue );			
			auto token = bounded_value( l, h, l );
			*(this) >> token;
			auto r = uuipair_dec( token.x );
			p.i = r.first;
			p.j = r.second;
			return *this;
		}

		/*! list2 est le sous-ensemble de list1. le champs de bits est de la taille de list1.
		 *  un bit 1 indique que cet élément de list1 fait partie du sous-ensemble list2.
		 * L'implémentation supporte un conteneur list2 pré-alloué ou un insert_iterator
		 */
		template<typename Iterator1,typename Iterator2>
		inline InputBitStream& operator >> ( Subset<Iterator1,Iterator2>& s)
		{
			s.ssl = s.ssf;
			for(Iterator1 it=s.rsf; it!=s.rsl; ++it)
			{
				if( readBits(1) )
				{
					*s.ssl++ = *it;
				}
			}
			return (*this);
		}

		Stream& stream;
		DbgStream& dbgstream;
		Word buffer[BUFFER_SIZE];
		uint64_t streampos;
		uint32_t bufferSize;
		uint32_t cursor_word;
		uint32_t cursor_bit;
	};

} } // namespace

#endif


