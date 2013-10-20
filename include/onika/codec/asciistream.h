#ifndef __onika_asciistream_h
#define __onika_asciistream_h

#include "onika/codec/types.h"
#include "onika/codec/debugstream.h"
#include "onika/vec.h"
#include "onika/tuple.h"
#include <iostream>
#include <map>

namespace onika { namespace codec {

	struct AsciiStream
	{
		inline AsciiStream(std::ostream& s) : stream(s), sep(',') {}

		inline DebugStream debug() { return DebugStream(stream<<"DEBUG: "); }

		inline void flush() { stream.flush(); }

		template<typename T>
		inline AsciiStream& operator << (const T& x)
		{
			stream << x;
			return *this;
		}

		template<typename Iterator>
		inline AsciiStream& operator << (const List<Iterator>& l)
		{
			Iterator i=l.f;
			if(i!=l.l) { (*this)<<(*i); ++i; }
			for(;i!=l.l;++i) { (*this)<<sep<<(*i); }
			return *this;
		}
	
		template<unsigned int NDim, typename CType>
		inline AsciiStream& operator << (const Vec<NDim,CType>& v)
		{
			(*this)<<"Vec(";
			(*this) << list( v.x, v.x+NDim );
			(*this)<<")";
			return (*this);
		}

		template<typename T>
		inline AsciiStream& operator << (const BoundedValue<T>& i)
		{
			return (*this)<<"BoundedValue["<<i.min<<","<<i.max<<"]("<<i.x<<")\n";
		}

		template<typename Iterator>
		inline AsciiStream& operator << (const BoundedIntegerSet<Iterator>& s)
		{
			return (*this)<<"BoundedIntegerSet["<<s.min<<","<<s.max<<"]("<<s.list<<")\n";
		}

		template<typename T1, typename T2>
		inline AsciiStream& operator << (const std::pair<T1,T2>& v)
		{
			return (*this) <<"Pair(" << v.first << sep << v.second << ")\n";
		}

		template<typename Iterator1,typename Iterator2>
		inline AsciiStream& operator << (const Subset<Iterator1,Iterator2>& s)
		{
			return (*this)<<"Subset["<<s.list1<<"]("<<s.list2<<")\n";
		}

		template<class... Types>
		inline AsciiStream& operator << (const std::tuple<Types...>& t)
		{
			tuple::print( *this , t );
			stream<<'\n';
			return (*this);
		}

		std::ostream& stream;
		char sep;
	};

} } // namespace

#endif

// ====================================================
// =============== UNIT TEST ==========================
// ====================================================
#ifdef onika_codec_asciistream_TEST

#include <iostream>

int main()
{
	onika::codec::AsciiStream out(std::cout);
	int set1[10] = {10,9,8,7,6,5,4,3,2,1};
	int set2[10], n=0;

	out<< onika::codec::list(set1,set1+10) << "\n";

	std::cout<<"subset ? "; std::cout.flush();
	do { std::cin>>set2[n++]; } while(set2[n-1]>=0);
	--n;

	out<< onika::codec::subset(set1,set1+10,set2,set2+n);

	out<< onika::codec::bounded_integer_set(1,10,set1,set1+5);

	// just a compile test
	out<< onika::codec::bounded_value(124,100,150);

	return 0;
}

#endif


