#ifndef __onika_vec_h
#define __onika_vec_h

#include "onika/aabb.h"
#include <cmath>
#include <numeric>
#include <limits>
#include <type_traits>

namespace onika
{
	template<unsigned int _NDim, typename _CoordType>
	struct Vec
	{
		enum { NDim = _NDim };
		typedef _CoordType CoordType;
		typedef double LengthType;

		inline bool operator < (const Vec& v)
		{
			for(int i=0;i<NDim;i++)
			{
				if( x[i] < v.x[i] ) return true;
				else if( x[i] > v.x[i] ) return false;
			}
			return false;
		}

		inline CoordType& operator [] (unsigned int d) { return x[d]; }
		inline const CoordType& operator [] (unsigned int d) const { return x[d]; }

#define BINARY_OP(op) \
		inline Vec operator op (const Vec& v) const \
		{ 	Vec r; \
			for(int i=0;i<NDim;i++) { r.x[i] = x[i] op v.x[i]; } \
			return r; }
		BINARY_OP(-)
		BINARY_OP(+)
		BINARY_OP(*)
		BINARY_OP(/)
#undef BINARY_OP

#define BOOL_OP(op,rop) \
		inline bool operator op (const Vec& v) const \
		{	bool r = x[0] op v.x[0]; \
			for(int i=1;i<NDim;i++) { r = r rop (x[i] op v.x[i]); } \
			return r; }
		BOOL_OP(<,&&)
		BOOL_OP(<=,&&)
		BOOL_OP(>,&&)
		BOOL_OP(>=,&&)
#undef BOOL_OP

		inline LengthType norm2()
		{
			LengthType l = 0;
			for(unsigned int i=0;i<NDim;i++)
			{
				LengthType c=x[i];
				l += c*c;
			}
			return l;
		}

		inline LengthType norm()
		{
			return sqrt(norm2());
		}

		CoordType x[NDim];
	};

	template<typename T> inline Vec<2,T> vec(const T& x1, const T& x2) { Vec<2,T> v; v.x[0]=x1; v.x[1]=x2; return v; }
	template<typename T> inline Vec<3,T> vec(const T& x1, const T& x2, const T& x3) { Vec<3,T> v; v.x[0]=x1; v.x[1]=x2; v.x[2]=x3; return v; }

}


//==========================================
//=== STL type traits specialization     ===
//==========================================
#include <type_traits>
namespace std {
	template<unsigned int D, typename T>
	struct make_signed< onika::Vec<D,T> > { typedef onika::Vec< D , typename std::make_signed<T>::type > type; };
}


//==========================================
//=== STL limits specialization          ===
//==========================================
#include <limits>
namespace std {
	template<unsigned int D, typename T>
	struct numeric_limits< onika::Vec<D,T> >
	{
		static inline onika::Vec<D,T> min()
		{
			onika::Vec<D,T> v;
			for(unsigned int i=0;i<D;++i) v.x[i] = numeric_limits<T>::min() ;
			return v;
		}  

		static inline onika::Vec<D,T> max()
		{
			onika::Vec<D,T> v;
			for(unsigned int i=0;i<D;++i) v.x[i] = numeric_limits<T>::max() ;
			return v;
		}  
	};
}


//==========================================
//=== value_traits specialization        ===
//==========================================
#include "onika/valuetraits.h"
namespace onika {
	template<unsigned int D, typename T>
	struct value_traits< Vec<D,T> >
	{
		static inline Vec<D,T> min()
		{
			Vec<D,T> v;
			for(unsigned int i=0;i<D;++i) v.x[i] = value_traits<T>::min() ;
			return v;
		}
	  
		static inline Vec<D,T> max()
		{
			Vec<D,T> v;
			for(unsigned int i=0;i<D;++i) v.x[i] = value_traits<T>::max() ;
			return v;
		}  
	};
}

// ==========================================================
// =================== Unit Test ============================
// ==========================================================
#ifdef onika_vec_TEST

#include <iostream>

int main()
{
	onika::Vec<3,int> v;
	std::cin>>v.x[0]>>v.x[1]>>v.x[2];
	std::cout<<"norm = "<<v.norm()<<"\n";
	return 0;
}

#endif // end of unit test

#endif // end of vec.h

