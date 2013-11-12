#ifndef __onika_codec_bitstream_h
#define __onika_codec_bitstream_h

#include "onika/codec/encoding.h"
#include "onika/codec/types.h"
#include "onika/codec/debugstream.h"
#include "onika/vec.h"
#include "onika/poweroftwo.h"
#include <iostream>
#include <vector>
#include <algorithm>

namespace onika { namespace codec {

	struct BitStream
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

		inline BitStream(std::ostream& s, std::ostream& dbg) : stream(s), dbgstream(dbg), words(0), bits(0), streampos(0) {}
		inline ~BitStream()
		{
			if(bits>0) { ++words; bits=0; }
			flush();
		}

		inline DebugStream debug() { return DebugStream(dbgstream<<'@'<<totalBits()<<" : "); }

		inline void flush()
		{
			stream.write((const char*)buffer,words*sizeof(Word));
			streampos += words*WORD_BITS;
			if(bits>0)
			{
				buffer[0] = buffer[words];
			}
			words = 0;
		}
		
		inline uint64_t totalBits() { return streampos+words*WORD_BITS+bits; }

		inline void writeBits(Word x, int n)
		{
			if( words > (BUFFER_SIZE-2) ) { flush(); }
			int freebits = WORD_BITS - bits;
			int padbits = ( n > freebits ) ? freebits : n;
			Word mask = (1<<padbits) - 1 ;
			buffer[words] &= ~ ( mask << bits );
			buffer[words] |= ( x & mask ) << bits;
			bits += padbits;
			if( bits >= WORD_BITS ) { bits -= WORD_BITS; ++words; }
			n -= padbits;
			if( n > 0 )
			{
				x >>= padbits;
				mask = (1<<n) - 1 ;
				buffer[words] &= ~ ( mask << bits );
				buffer[words] |= ( x & mask ) << bits;
				bits += n;
			}
			if( bits >= WORD_BITS ) { bits -= WORD_BITS; ++words; }
		}

		template<typename DataType>
		inline BitStream& operator << (const DataType& x)
		{
			writeBits(x,sizeof(DataType)*8);
			return *this;
		}

		inline BitStream& operator << (const std::string& s)
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

		template<unsigned int NDim, typename CType>
		inline BitStream& operator << (const Vec<NDim,CType>& v)
		{
			for(unsigned int i=0;i<NDim;i++)
			{
				return (*this) << v[i];
			}
			return (*this);
		}

		template<typename T1, typename T2>
		inline BitStream& operator << (const std::pair<T1,T2>& v)
		{
			return (*this) << v.first << v.second;
		}

		template<typename Iterator>
		inline BitStream& operator << (const List<Iterator>& l)
		{
			for(Iterator i=l.f;i!=l.l;++i) { (*this)<<(*i); }
			return *this;
		}

		template<typename T>
		inline BitStream& operator << (const std::vector<T>& l)
		{
			return (*this) << List< typename std::vector<T>::const_iterator >(l.begin(),l.end());
		}

		template<unsigned int D, typename T>
		inline BitStream& operator << (const BoundedValue<Vec<D,T> >& value)
		{
			for(int i=0;i<D;++i)
			{
				(*this) << bounded_value(value.x[i],value.min[i],value.max[i]);
			}
			return (*this);
		}

		template<typename T>
		inline BitStream& operator << (const BoundedValue<T>& i)
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
		inline BitStream& operator << (const BoundedValue<T>& i) \
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

		inline BitStream& operator << (const BoundedValue<float>& i){ return (*this)<<i.x; }
		inline BitStream& operator << (const BoundedValue<double>& i){ return (*this)<<i.x; }

		template<typename Iterator>
		inline BitStream& operator << (const BoundedIntegerSet<Iterator>& s)
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
		inline BitStream& operator << (const Subset<Iterator1,Iterator2>& s)
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

		std::ostream& stream;
		std::ostream& dbgstream;
		uint64_t buffer[BUFFER_SIZE];
		uint64_t streampos;
		uint32_t words;
		uint32_t bits;
	};

} } // namespace

#endif


