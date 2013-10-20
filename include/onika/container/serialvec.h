#ifndef __onika_container_serialvec_h
#define __onika_container_serialvec_h

#include <cstdint>
#include <stdlib.h>

#include "onika/debug/dbgassert.h"
#include "onika/container/iterator.h"
#include "onika/vec.h"
#include "onika/language.h"

namespace onika { namespace container {

template<typename _BaseContainer, unsigned int _N>
struct SerialVec
{
	enum { NComp = _N };
	typedef _BaseContainer BaseContainer;
	typedef typename BaseContainer::value_type ElementType;
	typedef Vec<NComp,ElementType> ZippedValueType;

	// generic container API
	ONIKA_CONTAINER_ACCESS_DEF(SerialVec,ZippedValueType);
	ONIKA_CONTAINER_PUSH_BACK_DEF(SerialVec,ZippedValueType);

	inline void resize(size_t n, const value_type& ival)
	{
		size_t oldN = size();
		resize(n);
		for(size_t i=oldN;i<n;++i) for(int c=0;c<NComp;++c) { data[i*NComp+c]=ival.x[c]; }
	}
	inline void resize(size_t n)
	{
		data.resize(n*NComp);
	}
	inline size_t size() const { return data.size()/NComp; }
	inline void set(size_t i, const value_type& val)
	{
		for(int c=0;c<NComp;++c) data[i*NComp+c] = val.x[i];
	}
	inline value_type get(size_t i) const
	{
		value_type r;
		for(int c=0;c<NComp;++c) r.x[i] = data[i*NComp+c];
		return r;
	}


	BaseContainer data;
};


} } // end of namespace


//==========================================
//=== container algorithm specialization ===
//==========================================
#include "onika/container/algorithm.h"
namespace onika { namespace container {

	template<typename _B, unsigned int _N> inline auto 
	min_value(const SerialVec<_B,_N>& c) -> typename SerialVec<_B,_N>::value_type
	{
		typename SerialVec<_B,_N>::value_type r;
		auto x = min_value( c.data );
		for(unsigned int i=0;i<_N;i++) r[i] = x;
		return r;
	}

	template<typename _B, unsigned int _N> inline auto 
	max_value(const SerialVec<_B,_N>& c) -> typename SerialVec<_B,_N>::value_type
	{
		typename SerialVec<_B,_N>::value_type r;
		auto x = max_value( c.data );
		for(unsigned int i=0;i<_N;i++) r[i] = x;
		return r;
	}

	template<typename _B, unsigned int _N> inline auto 
	memory_bytes(const SerialVec<_B,_N>& c)
	ONIKA_AUTO_RET( memory_bytes(c.data) );

} }


//==========================================
//=== codec method specialization        ===
//==========================================
#include "onika/codec/types.h"
#include "onika/codec/encoding.h"
namespace onika { namespace codec {

	template<typename _B, unsigned int _N> inline auto 
	wavelet_enc(container::SerialVec<_B,_N>& c, size_t a,size_t b)
	-> Vec< _N , BoundedValue< ONIKA_MAKE_SIGNED(typename _B::value_type) > >
	{
		Vec< _N , BoundedValue< ONIKA_MAKE_SIGNED(typename _B::value_type) > > r;
		for(int i=0;i<_N;++i) { r[i] = wavelet_enc(c.data,a*_N+i,b*_N+i); }
		return r;
	}

} } // end of onika::codec



//==========================================
//=== STL algorithm specialization       ===
//==========================================
#include <algorithm>
namespace std
{
	// std::swap
	template<typename _B, unsigned int _N> inline
	void swap(
		onika::container::ElementAccessorT< onika::container::SerialVec<_B,_N> > A,
		onika::container::ElementAccessorT< onika::container::SerialVec<_B,_N> > B )
	{
		onika::container::SerialVec<_B,_N> & svec = A.v;
		onika::debug::dbgassert( ( & svec ) == ( & B.v ) );
		for(int i=0;i<_N;++i) { std::swap( svec.data[A.i] , svec.data[B.i] ); }
	}

	// std::iter_swap
	template<typename _B, unsigned int _N> inline
	void iter_swap(
		onika::container::IteratorT< onika::container::SerialVec<_B,_N> > A,
		onika::container::IteratorT< onika::container::SerialVec<_B,_N> > B )
	{
		onika::container::SerialVec<_B,_N> & svec = A.vec;
		onika::debug::dbgassert( ( & svec ) == ( & B.vec ) );
		std::swap( svec.data[A.i] , svec.data[B.i] );
	}
}



// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#ifdef onika_container_serialvec_TEST

#include "onika/vtk/readvtkascii.h"
#include "onika/vec.h"

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct FakeMesh
{
	typedef onika::Vec<3,double> VertexPos;
	inline void addCell(int n, const int* v) { }
	inline void addVertex(const VertexPos& p)
	{
		values.push_back(p);
	}
	onika::container::SerialVec<vector<double>,3> values;
};

int main(int argc, char* argv[])
{
	bool autoMode = argc > 1 && string("-a") == argv[1];
	std::string fname = "data/tetraMesh.vtk";
	if( !autoMode )
	{
		cout<<"File ? "; cout.flush();
		cin>>fname;
	}
	ifstream ifile(fname.c_str());
	if( !ifile )
	{
		cerr<<"Failed to open "<<fname<<endl;
		return 1;
	}
	FakeMesh mesh;
	onika::vtk::readVtkAsciiMesh(ifile,mesh);

	return 0;
}

#endif // end of unit test


#endif // end of serialvec.h

