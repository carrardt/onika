#ifndef __onika_mesh_meshalgorithm_h
#define __onika_mesh_meshalgorithm_h

#include "onika/tuple.h"
#include "onika/language.h"
#include "onika/mathfunc.h"

namespace onika { namespace mesh {

// vertex ordering from vertex ids, using vertex values
template<class VertexContainer>
struct VertexLexicalOrdering
{
	inline VertexLexicalOrdering(const VertexContainer& _c) : c(_c) {}
	inline bool operator () (size_t a, size_t b) const
	{
		typename VertexContainer::value_type va=c[a], vb=c[b];
		return onika::tuple::lexical_order( va , vb );
	}
	const VertexContainer& c;
};

template<class VertexContainer>
inline VertexLexicalOrdering<VertexContainer> vertex_lexical_order(const VertexContainer& verts) { return VertexLexicalOrdering<VertexContainer>(verts); }

template<class C2V, class VertexContainer>
struct CellLexicalOrder
{
	typedef typename C2V::IdType IdType;
	typedef typename VertexContainer::value_type VertexPos;
	inline CellLexicalOrder(const C2V& _m, const VertexContainer& _v) : mesh(_m), vertices(_v) {}
	bool operator () (IdType a, IdType b)
	{
		IdType nva = mesh.getCellNumberOfVertices(a);
		IdType nvb = mesh.getCellNumberOfVertices(b);
		if( nva < nvb ) return true;
		else if( nvb < nva ) return false;
		for(IdType i=0;i<nva;i++)
		{
			IdType va = mesh.getCellVertexId(a,i);
			IdType vb = mesh.getCellVertexId(b,i);
			VertexPos pa = vertices[va];
			VertexPos pb = vertices[vb];
			if( tuple::lexical_order(pa,pb) ) return true;
			else if( tuple::lexical_order(pb,pa) ) return false;
		}
		return false;
	}
	const C2V& mesh;
	const VertexContainer& vertices;
};

template<class C2V, class VertexContainer>
inline CellLexicalOrder<C2V,VertexContainer> cell_lexical_order(const C2V& _m, const VertexContainer& _v) { return CellLexicalOrder<C2V,VertexContainer>(_m,_v); }

template<typename Vector>
struct IndexedValueCompare
{
	inline IndexedValueCompare(const Vector& _m) : vec(_m) {}
	template<class Integer>
	inline bool operator () (Integer a, Integer b) const
	{
		return vec[a] < vec[b];
	}
	const Vector& vec;
};

template<typename Vector>
inline IndexedValueCompare<Vector> indexed_value_compare(const Vector&v) { return IndexedValueCompare<Vector>(v); }

// operator that returns an edge length given 2 vertex indices
template<class VertexContainer>
struct EdgeLengthOp
{
	const VertexContainer& vertices;

	inline EdgeLengthOp(const VertexContainer& v) : vertices(v) {}

	template<class IdType>
	inline auto operator () ( const std::tuple<IdType,IdType>& edge ) const
	ONIKA_AUTO_RET( onika::math::distance( vertices[std::get<0>(edge)] , vertices[std::get<1>(edge)] ) )
};

template<class c2e_traits, class EdgeLengthFunc>
struct CellMinEdgeLengthCompare
{
	using CellContainer = typename c2e_traits::ContainerType;

	inline CellMinEdgeLengthCompare(const CellContainer& _c, const EdgeLengthFunc& el) : cells(_c), edgeLength(el) {}

	template<class IdType>
	inline bool operator () ( IdType cellA, IdType cellB ) const
	{
		if( cellA == cellB ) return false;
		IdType nedgesA = c2e_traits::getCellNumberOfEdges(cells,cellA);
		IdType nedgesB = c2e_traits::getCellNumberOfEdges(cells,cellB);
		if( nedgesA <= 0 ) return false;
		if( nedgesB <= 0 ) return true;
		auto edge = c2e_traits::getCellEdge(cells,cellB,0);
		auto elen = edgeLength(edge);
		for(IdType e=1;e<nedgesB;++e)
		{
			auto l = edgeLength( c2e_traits::getCellEdge(cells,cellB,e) );
			if( l < elen ) elen = l;
		}
		for(IdType e=0;e<nedgesA;++e)
		{
			auto l = edgeLength( c2e_traits::getCellEdge(cells,cellA,e) );
			if( l < elen ) return true;
		}
		return false;
	}

	const CellContainer& cells;
	EdgeLengthFunc edgeLength;
};

template<class c2e_traits>
struct CellShortestEdge
{
	template<class EdgeLengthFunc>
	static inline auto less(const typename c2e_traits::ContainerType& cells, const EdgeLengthFunc& el)
	ONIKA_AUTO_RET( CellMinEdgeLengthCompare<c2e_traits,EdgeLengthFunc>(cells,el) )
};

} } // end of namespace

#endif


