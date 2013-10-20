#ifndef __onika_container_quantizedvector_h
#define __onika_container_quantizedvector_h

#include "onika/container/nbitsvector.h"
#include <limits>
#include "onika/language.h"

namespace onika { namespace container {

	/*
	 * Warning, once low and high are set, you cannot assign values out of the range[low,high]
	 * unless you call copyFrom or clear to modify it
	 *
	 * Quantizer requirements :
	 *	1. preserves elements ordering
	 *
	 */
	template<typename _Quantizer>
	struct QuantizedVector
	{
		typedef _Quantizer Quantizer;
		typedef typename Quantizer::DecodedType DType;
		typedef NBitsVector<uint64_t> BaseContainer;

		// generic container API
		ONIKA_CONTAINER_ACCESS_DEF(QuantizedVector,DType);
		ONIKA_CONTAINER_PUSH_BACK_DEF(QuantizedVector,DType);

		template<typename Iterator>
		inline void copyFrom(Iterator first, Iterator last)
		{
			quantizer.initialize(first,last);
			data.clear( quantizer.getNBits() );
			resize(last-first);
			std::copy(first,last,begin());
		}

		inline void clear()
		{
			data.clear( quantizer.getNBits() );
		}

		inline void set(size_t i, DType x)
		{
			data.set( i, quantizer.encode(x) );
		}

		inline DType get(size_t i) const
		{
			return quantizer.decode( data.get(i) );
		}

		inline size_t size() const { return data.size(); }
		inline void resize(size_t n, double x) { data.resize(n,quantizer.encode(x)); }
		inline void resize(size_t n) { data.resize(n); }

		BaseContainer data;
		Quantizer quantizer;
	};

} } // end of onika::container


//==========================================
//=== container algorithm specialization ===
//==========================================
#include "onika/container/algorithm.h"
namespace onika { namespace container {

	template<typename _Q>
	inline auto min_value(const QuantizedVector<_Q>& c) 
	ONIKA_AUTO_RET( c.quantizer.getLowerBound() )

	template<typename _Q>
	inline auto max_value(const QuantizedVector<_Q>& c) 
	ONIKA_AUTO_RET( c.quantizer.getHigherBound() )

	template<typename _Q>
	inline size_t memory_bytes(const QuantizedVector<_Q>& c)
	{
		return sizeof(QuantizedVector<_Q>) - sizeof(typename QuantizedVector<_Q>::BaseContainer) 
		+ memory_bytes(c.data);
	}

} } // end of onika::container


//==========================================
//=== codec method specialization        ===
//==========================================
#include "onika/codec/types.h"
#include "onika/codec/encoding.h"
namespace onika { namespace codec {

	template<typename _Q> inline auto
	wavelet_enc(container::QuantizedVector<_Q>& c, size_t a,size_t b) -> BoundedValue<int64_t>
	{
		return wavelet_enc(c.data,a,b); // si la reconstruction sans perte n'est pas garantie, n'est-ce pas dangereux ?
	}

	template<typename _Q> inline auto
	value_enc(container::QuantizedVector<_Q>& c, size_t i)
	ONIKA_AUTO_RET( value_enc(c.data,i) )

	template<typename _Q> inline auto
	bounded_value_enc( 	container::QuantizedVector<_Q>& c,
				size_t i,
				typename container::QuantizedVector<_Q>::value_type Min,
				typename container::QuantizedVector<_Q>::value_type Max )
	-> BoundedValue<uint64_t>
	{
		uint64_t qmin = c.quantize(Min);
		uint64_t qmax = c.quantize(Max);
		uint64_t qv = c.data[i];
		return bounded_value(qv,qmin,qmax);
	}

	template<typename _Q> inline auto
	delta_enc(container::QuantizedVector<_Q>& c, size_t a, size_t b)
	ONIKA_AUTO_RET( delta_enc(c.data,a,b) )

	template<typename _Q> inline auto
	bounded_delta_enc(	container::QuantizedVector<_Q>& c,
				size_t a, 
				size_t b, 
				typename container::QuantizedVector<_Q>::value_type Min, 
				typename container::QuantizedVector<_Q>::value_type Max )
	-> BoundedValue<int64_t>
	{
		uint64_t qmin = quantize(Min);
		uint64_t qmax = quantize(Max);
		return bounded_delta_enc(c.data,a,b,qmin,qmax);
	}

} } // end of onika::codec


//==========================================
//=== STL algorithm specialization       ===
//==========================================
#include <algorithm>
namespace std
{
	// std::swap
	template<typename _Q>
	inline void swap(
		onika::container::ElementAccessorT< onika::container::QuantizedVector<_Q> > A,
		onika::container::ElementAccessorT< onika::container::QuantizedVector<_Q> > B )
	{
		onika::container::QuantizedVector<_Q> & qvec = A.v;
		onika::debug::dbgassert( ( & qvec ) == ( & B.v ) );
		std::swap( qvec.data[A.i] , qvec.data[B.i] );
	}

	template<typename _Q>
	inline void iter_swap(
		onika::container::IteratorT< onika::container::QuantizedVector<_Q> > A,
		onika::container::IteratorT< onika::container::QuantizedVector<_Q> > B )
	{
		onika::container::QuantizedVector<_Q> & qvec = A.vec;
		onika::debug::dbgassert( ( & qvec ) == ( & B.vec ) );
		std::swap( qvec.data[A.i] , qvec.data[B.i] );
	}

#define ONIKA_QVEC_STL_SORT_ADAPTOR(_F) \
	template<typename _Q> \
	inline void _F( \
		onika::container::IteratorT< onika::container::QuantizedVector<_Q> > itA, \
		onika::container::IteratorT< onika::container::QuantizedVector<_Q> > itB ) \
	{ \
		onika::container::QuantizedVector<_Q> & qvec = itA.vec; \
		onika::debug::dbgassert( ( & qvec ) == ( & itB.vec ) ); \
		size_t ai = itA.i; \
		size_t bi = itB.i; \
		std::_F(qvec.data.begin()+ai , \
			qvec.data.begin()+bi /*, [qvec](size_t a, size_t b) -> bool { return qvec.get(a) < jvec.get(b); }*/ ); \
	} \
	\
	template<typename _Q, typename CompF> \
	inline void _F( \
		onika::container::IteratorT< onika::container::QuantizedVector<_Q> > itA, \
		onika::container::IteratorT< onika::container::QuantizedVector<_Q> > itB, \
		CompF comp  ) \
	{ \
		onika::container::QuantizedVector<_Q> & qvec = itA.vec; \
		onika::debug::dbgassert( ( & qvec ) == ( & itB.vec ) ); \
		size_t ai = itA.i; \
		size_t bi = itB.i; \
		std::_F(qvec.m_indices.begin()+ai , \
			qvec.m_indices.begin()+bi , \
			[qvec,comp](size_t a, size_t b) -> bool { comp( qvec.get(a) , qvec.get(b) ); } ); \
	}

	ONIKA_QVEC_STL_SORT_ADAPTOR(sort)
	ONIKA_QVEC_STL_SORT_ADAPTOR(push_heap)
	ONIKA_QVEC_STL_SORT_ADAPTOR(pop_heap)
	ONIKA_QVEC_STL_SORT_ADAPTOR(make_heap)
	ONIKA_QVEC_STL_SORT_ADAPTOR(sort_heap)
} // end of namespace std



// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#ifdef onika_container_quantizedvector_TEST

#include "onika/vtk/readvtkascii.h"
#include "onika/quantize/lossydoublequantizer.h"
#include "onika/vec.h"

#include <iostream>
#include <fstream>
using namespace std;

typedef onika::quantize::LossyDoubleQuantizer Quantizer;
typedef onika::container::QuantizedVector<Quantizer> QRealVector;

struct FakeMesh
{
	typedef onika::Vec<3,double> VertexPos;
	FakeMesh(double l, double h, int nb=52)
	{
		values.data.clear(nb);
		values.quantizer.low=l;
		values.quantizer.high=h;
		values.quantizer.nbits=nb;
	}
	inline void addCell(int n, const int* v) { }
	inline void addVertex(const VertexPos& p)
	{
		values.push_back(p[0]);
		values.push_back(p[1]);
		values.push_back(p[2]);
	}
	QRealVector values;
};

int main(int argc, char* argv[])
{
	bool autoMode = argc > 1 && string("-a") == argv[1];
	std::string fname = "data/tetraMesh.vtk";
	double Min=-10.0, Max=10.0;
	int nbits=52;
	if( !autoMode )
	{
		cout<<"File ? "; cout.flush();
		cin>>fname;
		cout<<"Min Max ? "; cout.flush();
		cin>>Min>>Max;
		cout<<"NBits ? "; cout.flush();
		cin>>nbits;
	}

	ifstream ifile(fname.c_str());
	if( !ifile )
	{
		cerr<<"Failed to open "<<fname<<endl;
		return 1;
	}

	FakeMesh mesh(Min,Max,nbits);
	onika::vtk::readVtkAsciiMesh(ifile,mesh);
	
	return 0;
}
#endif // end of unit test

#endif // end of quantizedvector.h

