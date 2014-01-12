#ifndef __onika_codec_outputbitstream_h
#define __onika_codec_outputbitstream_h

#include "onika/codec/encoding.h"
#include "onika/codec/types.h"
#include "onika/codec/debugstream.h"
#include "onika/poweroftwo.h"
#include "onika/bits.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <tuple>
#include "onika/tuple.h"


namespace onika { namespace codec {

	template<class StreamT>
	struct StreamOutputOp
	{
		inline StreamOutputOp( StreamT& s ) : stream(s) {}
		template<class T>
		inline void operator () (const T& x) { stream << x; }
		StreamT& stream;
	};

	struct OutputBitStream
	{
		typedef uint64_t Word;
		enum
		{
			WORD_BITS = sizeof(Word)*8,
			MAX_INTEGER_SET_SIZE = 8,
#if defined(_DEBUG) || defined(DEBUG)
			BUFFER_SIZE = 4 
#else
			BUFFER_SIZE = 128*1024 
#endif
			, MAX_STRING_LEN = 32
		}; 

		inline OutputBitStream(std::ostream& s, std::ostream& dbg) : stream(s), dbgstream(dbg), cursor_word(0), cursor_bit(0), streampos(0) {}
		inline ~OutputBitStream()
		{
			if(cursor_bit>0) { ++cursor_word; cursor_bit=0; }
			flush();
		}

		inline DebugStream debug() { return DebugStream(dbgstream<<"\n@"<<totalBits()<<" : "); }

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

		template<typename DataType>
		inline OutputBitStream& operator << (const DataType& x)
		{
			writeBits(x,sizeof(DataType)*8);
			return *this;
		}

		inline OutputBitStream& operator << (const std::string& s)
		{
			unsigned int n = s.length();
			if( n > MAX_STRING_LEN ) n = MAX_STRING_LEN;
			(*this) << bounded_value(n,0u,static_cast<unsigned int>(MAX_STRING_LEN));
			for(int i=0;i<n;i++)
			{
				unsigned char c = s[i];
				(*this) << c;
			}
			return (*this);
		}		

		template<typename T1, typename T2>
		inline OutputBitStream& operator << (const std::pair<T1,T2>& v)
		{
			return (*this) << v.first << v.second;
		}

		template<typename Iterator>
		inline OutputBitStream& operator << (const List<Iterator>& l)
		{
			for(Iterator i=l.f;i!=l.l;++i) { (*this)<<(*i); }
			return *this;
		}

		template<typename T>
		inline OutputBitStream& operator << (const std::vector<T>& l)
		{
			return (*this) << List< typename std::vector<T>::const_iterator >(l.begin(),l.end());
		}

		template<typename T>
		inline OutputBitStream& operator << (const BoundedValue<T>& i)
		{
			debug::dbgassert(i.max >= i.min);
			if( i.max > i.min )
			{
				uint64_t range = i.max - i.min + 1;
				uint64_t value = i.x - i.min;
				int nbits = nextpo2log(range);
				writeBits( value , nbits );
			}
			return (*this);
		}

#define BOUNDED_VALUE_INTEGER_SPECIALIZATION(T) \
		inline OutputBitStream& operator << (const BoundedValue<T>& i) \
		{ \
			if( i.max > i.min ) \
			{ \
				uint64_t range = i.max - i.min + 1; \
				uint64_t value = i.x - i.min; \
				int nbits = nextpo2log(range); \
				writeBits( value , nbits ); \
			} \
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

		inline OutputBitStream& operator << (const BoundedValue<float>& i){ return (*this)<<i.x; }
		inline OutputBitStream& operator << (const BoundedValue<double>& i){ return (*this)<<i.x; }

		template<typename Iterator>
		inline OutputBitStream& operator << (const BoundedIntegerSet<Iterator>& s)
		{
			uint64_t buffer[MAX_INTEGER_SET_SIZE];
			int nvalues=0;
			for(Iterator it=s.list.f; it!=s.list.l; ++it)
			{
				buffer[nvalues] = *it - s.min;
				++nvalues;
			}
			uint64_t value = uint_set_enc(buffer,buffer+nvalues);
			uint64_t Min = 0;
			uint64_t Max = uint_set_enc_bound(nvalues,s.max-s.min);
			return (*this) << bounded_value( value , Min, Max );
		}

		template<typename Iterator1,typename Iterator2>
		inline OutputBitStream& operator << (const Subset<Iterator1,Iterator2>& s)
		{
			int bits=0;
			Word w=0;
			for(Iterator1 it=s.list1.f; it!=s.list1.l; ++it)
			{
				Iterator1 eit = it; ++eit;
				if( std::includes(s.list2.f,s.list2.l,it,eit) )
				{
					w |= 1<<bits;
				}
				++bits;
				if( bits == WORD_BITS ) { writeBits(w,bits); bits=0; }
			}
			writeBits(w,bits);
			return (*this);
		}

		template<class... Types>
		inline OutputBitStream& operator << (const std::tuple<Types...>& t)
		{
			tuple::apply( t , StreamOutputOp<OutputBitStream>(*this) );
			return (*this);
		}

		std::ostream& stream;
		std::ostream& dbgstream;
		uint64_t buffer[BUFFER_SIZE];
		uint64_t streampos;
		uint32_t cursor_word;
		uint32_t cursor_bit;
	};

} } // namespace

#endif


