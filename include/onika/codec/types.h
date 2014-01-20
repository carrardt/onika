#ifndef __onika_codec_types_h
#define __onika_codec_types_h

#include "onika/debug/dbgassert.h"
#include "onika/language.h"

namespace onika { namespace codec {


	/* 1) Both sets MUST be sorted, in the purpose of disambiguous reading.
	 * 2) Iterator2 may be an insert iterator for reading pruposes.
	 * 2bis) or a normal iterator in a container with enough space left to store elements
	 */	 	 
	template<typename Iterator1, typename Iterator2>
	struct Subset
	{
		inline Subset(Iterator1 _rsf, Iterator1 _rsl, Iterator2 _ssf, Iterator2 _ssl)
			: rsf(_rsf), rsl(_rsl), ssf(_ssf), ssl(_ssl) {}
		Iterator1 rsf,rsl;
		Iterator2 ssf,ssl;
	};
	template<typename Iterator1, typename Iterator2>
	static inline auto subset(Iterator1 rsf, Iterator1 rsl, Iterator2 ssf, Iterator2 ssl)
	ONIKA_AUTO_RET( Subset<Iterator1,Iterator2>(rsf,rsl,ssf,ssl) )

	// x must be in the range [min,max] (inclusive)
	template<typename T>
	struct BoundedValue
	{
		inline BoundedValue(const T& l, const T& h, const T& v) : low(l), high(h), x(v)
		{
			debug::dbgassert(low<=high);
		}
		T low,high,x;
	};
	template<typename T>
	static inline auto bounded_value(const T& l, const T& h,const T& v)
	ONIKA_AUTO_RET( BoundedValue<T>(l,h,v) )

	// unordered set of integers in the range [0,maxvalue]
	template<class Iterator>
	struct UUISet
	{
		inline UUISet(Iterator f, Iterator l, uint64_t m)
		: first(f), nvalues(std::distance(f,l)), maxvalue(m) {}
		inline UUISet(Iterator f,uint64_t n,uint64_t m)
		: first(f), nvalues(n), maxvalue(m) {}
		Iterator first;
		uint64_t nvalues;
		uint64_t maxvalue;
	};
	template<class Iterator>
	static inline auto uuiset(Iterator f,Iterator l,uint64_t m)
	ONIKA_AUTO_RET( UUISet<Iterator>(f,l,m) )

	template<class Iterator>
	static inline auto uuiset(Iterator f,uint64_t n,uint64_t m)
	ONIKA_AUTO_RET( UUISet<Iterator>(f,n,m) )

	// unordered unique integer pair in the range[0,maxvalue]
	struct UUIPair
	{
		inline UUIPair(uint64_t _i1,uint64_t _i2,uint64_t m)
		: i1(_i1), i2(_i2), maxvalue(m) {}
		inline UUIPair(uint64_t m)
		: i1(0), i2(0), maxvalue(m) {}
		uint64_t i1,i2,maxvalue;
	};
	static inline auto uuipair(uint64_t i,uint64_t j,uint64_t m)
	ONIKA_AUTO_RET( UUIPair(i,j,m) )
	static inline auto uuipair(uint64_t m)
	ONIKA_AUTO_RET( UUIPair(m) )
} } // end of namespace

#endif // __onika_codec_types_h


