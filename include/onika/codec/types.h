#ifndef __onika_codec_types_h
#define __onika_codec_types_h

#include "onika/debug/dbgassert.h"
#include "onika/language.h"

namespace onika { namespace codec {


	/* 1) Both sets MUST be sorted to ensure unambiguous reading.
	 * 2) Iterator2 may be an insert iterator for reading purposes.
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

	template<typename Iterator1, typename Iterator2>
	static inline auto subset(Iterator1 rsf, Iterator1 rsl, Iterator2 ssf)
	ONIKA_AUTO_RET( Subset<Iterator1,Iterator2>(rsf,rsl,ssf,ssf) )

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
	template<typename T>
	static inline auto bounded_value(const T& l, const T& h)
	ONIKA_AUTO_RET( BoundedValue<T>(l,h,l) )

	template<typename T>
	static inline auto bounded_value(const T& l, const T& h)
	ONIKA_AUTO_RET( BoundedValue<T>(l,h,l) )

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
		inline UUIPair(uint64_t _i,uint64_t _j,uint64_t m)
		: i(_i), j(_j), maxvalue(m) {}
		inline UUIPair(uint64_t m)
		: i(0), j(0), maxvalue(m) {}
		uint64_t i,j,maxvalue;
	};
	static inline auto uuipair(uint64_t i,uint64_t j,uint64_t m)
	ONIKA_AUTO_RET( UUIPair(i,j,m) )
	static inline auto uuipair(uint64_t m)
	ONIKA_AUTO_RET( UUIPair(m) )
} } // end of namespace


#endif // __onika_codec_types_h


