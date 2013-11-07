#ifndef __onika_codec_types_h
#define __onika_codec_types_h

#include "onika/debug/dbgassert.h"
#include "onika/debug/dbgmessage.h"

namespace onika { namespace codec {

	// Here, list means iterator based interval in a sequence/container
	template<typename Iterator>
	struct List
	{
		inline List(const Iterator& _f, const Iterator& _l) : f(_f), l(_l) {}
		Iterator f,l;
	};
	template<typename Iterator>
	inline List<Iterator> list(Iterator f,Iterator l) { return List<Iterator>(f,l); }


	/* 1) Both sets MUST be sorted, in the purpose of disambiguous reading.
	 * 2) Iterator2 may be an insert iterator for reading pruposes.
	 * 2bis) or a normal iterator in a container with enough space left to store elements
	 */	 	 
	template<typename Iterator1, typename Iterator2>
	struct Subset
	{
		inline Subset(Iterator1 _sf, Iterator1 _sl, Iterator2 _ssf, Iterator2 _ssl)
			: list1(_sf,_sl), list2(_ssf,_ssl) {}
		inline Subset(Iterator1 _sf, Iterator1 _sl, Iterator2 _ssf)
			: list1(_sf,_sl) {}
		List<Iterator1> list1;
		List<Iterator2> list2;
	};
	template<typename Iterator1, typename Iterator2>
	inline Subset<Iterator1,Iterator2> subset(Iterator1 _sf, Iterator1 _sl, Iterator2 _ssf, Iterator2 _ssl)
	{ return Subset<Iterator1,Iterator2>(_sf,_sl,_ssf,_ssl); }
	template<typename Iterator1, typename Iterator2>
	inline Subset<Iterator1,Iterator2> subset(Iterator1 _sf, Iterator1 _sl, Iterator2 _ssf)
	{ return Subset<Iterator1,Iterator2>(_sf,_sl,_ssf,_ssf); }

	// x must be in the range [min,max] (inclusive)
	template<typename T>
	struct BoundedValue
	{
		inline BoundedValue() {}
		inline BoundedValue(const BoundedValue& bv) : x(bv.x), min(bv.min), max(bv.max) {}
		inline BoundedValue& operator = (const BoundedValue& bv) { x=bv.x; min=bv.min; max=bv.max; }
		inline BoundedValue(const T& _min, const T& _max) : min(_min), max(_max)
		{
			debug::dbgassert(min<=max);
		}
		inline BoundedValue(const T& _x, const T& _min, const T& _max) : x(_x), min(_min), max(_max)
		{
			debug::dbgassert(x>=min && x<=max);
		}
		T x,min,max;
	};
	template<typename T>
	inline BoundedValue<T> bounded_value(const T& _x, const T& _min, const T& _max){ return BoundedValue<T>(_x,_min,_max); }
	template<typename T>
	inline BoundedValue<T> bounded_value(const T& _min, const T& _max){ return BoundedValue<T>(_min,_max); }

	template<typename Iterator>
	struct BoundedIntegerSet
	{
		inline BoundedIntegerSet(int64_t _min, int64_t _max,int64_t _n) : min(_min), max(_max),n(_n) {}
		inline BoundedIntegerSet(int64_t _min, int64_t _max,Iterator _f,Iterator _l)
		: min(_min),max(_max),list(_f,_l),n(-1) {}
		int64_t min,max,n;
		List<Iterator> list;
	};
	template<typename Iterator>
	inline BoundedIntegerSet<Iterator> bounded_integer_set(int64_t _min, int64_t _max, Iterator first, Iterator last)
	{ return BoundedIntegerSet<Iterator>(_min,_max,first,last); }

	template<typename T, typename T2>
	inline BoundedValue<T2>
	bounded_delta(const T& avg, const T2& delta, const T& low, const T& high)
	{
		debug::dbgassert(avg>=low);
		debug::dbgassert(avg<=high);
		T2 dlow = low - avg;
		T2 dhigh = high - avg;
		return BoundedValue<T2>( delta, dlow, dhigh );
	}

} } // end of namespace

#endif // __onika_codec_types_h


// ======================== UNIT TEST =======================
#endif

