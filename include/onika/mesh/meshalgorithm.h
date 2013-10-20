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


template<class VertexContainer, class IdType>
inline auto vertexDistance( const VertexContainer& vertices, IdType a, IdType b )
ONIKA_AUTO_RET( onika::math::distance(vertices[a],vertices[b]) )

template<class VertexContainer, class IdType>
inline auto edgeLength( const VertexContainer& vertices, const std::tuple<IdType,IdType>& edge )
ONIKA_AUTO_RET( vertexDistance(vertices,std::get<0>(edge),std::get<1>(edge)) )


template<class VertexContainer>
struct EdgeLengthCompare
{
	inline EdgeLengthCompare(const VertexContainer& v) : vertices(v) {}
	
	template<class IdType>
	inline bool operator () ( const std::tuple<IdType,IdType>& E1, const std::tuple<IdType,IdType>& E2) const
	{
		return edgeLength(vertices,E1) < edgeLength(vertices,E2);
	}

	const VertexContainer& vertices;
};

template<class VertexContainer, class CellContainer, class c2e_traits>
struct CellMinEdgeLengthCompare
{
	inline CellMinEdgeLengthCompare(const VertexContainer& _v, const CellContainer& _c) : cells(_c), vertices(_v) {}

	template<class IdType>
	inline bool operator () ( IdType cellA, IdType cellB ) const
	{
		if( cellA == cellB ) return false;
		IdType nedgesA = c2e_traits::getCellNumberOfEdges(cells,cellA);
		IdType nedgesB = c2e_traits::getCellNumberOfEdges(cells,cellB);
		if( nedgesA <= 0 ) return false;
		if( nedgesB <= 0 ) return true;
		auto edge = c2e_traits::getCellEdge(cells,cellB,0);
		auto elen = edgeLength(vertices,edge);
		for(IdType e=1;e<nedgesB;++e)
		{
			auto l = edgeLength(vertices,c2e_traits::getCellEdge(cells,cellB,e));
			if( l < elen ) elen = l;
		}
		for(IdType e=0;e<nedgesA;++e)
		{
			auto l = edgeLength(vertices,c2e_traits::getCellEdge(cells,cellA,e));
			if( l < elen ) return true;
		}
		return false;
	}
	const VertexContainer& vertices;
	const CellContainer& cells;
};

} } // end of namespace

#endif



//==============================================
//============= UNIT TEST ======================
//==============================================
#ifdef onika_mesh_meshalgorithm_TEST

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <set>

#include "onika/vtk/readvtkascii.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/container/sequence.h"
#include "onika/tuple.h"

ONIKA_USE_MATH;
ONIKA_USE_TUPLE_MATH;
ONIKA_USE_TUPLE_OSTREAM

struct Mesh
{
	typedef struct { double x[3]; } VertexPos;
	typedef std::vector<int> CellContainer;
	typedef std::vector< std::tuple<double,double,double> > VertexContainer;

	inline Mesh() {}

	inline void addVertex(const VertexPos& p)
	{
		vertices.push_back( std::make_tuple(p.x[0],p.x[1],p.x[2]) );
	}
	
	template<class VertIdIterator>
	inline void addCell(int np, VertIdIterator it)
	{
		if( np != 4 ) return;
		for(int i=0;i<np; ++i, ++it) cells.push_back( *it );
	}

	CellContainer cells;
	VertexContainer vertices;
};

typedef onika::mesh::smesh_c2v_basic_traits< std::vector<int>, 3 > SMeshC2VBase;
typedef onika::mesh::smesh_c2e_basic_traits<SMeshC2VBase> Cell2EdgesTraits;
typedef onika::mesh::CellMinEdgeLengthCompare<Mesh::VertexContainer,Mesh::CellContainer,Cell2EdgesTraits> CellCompare;
typedef onika::container::SequenceIterator<int> Sequence;
using onika::mesh::edgeLength;

int main()
{
	std::string fname;
	std::cout<<"File ? "; std::cout.flush();
	std::cin>>fname;
	std::ifstream ifile(fname.c_str());
	if( !ifile )
	{
		std::cerr<<"Failed to open "<<fname<<"\n";
		return 1;
	}

	Mesh mesh;
	onika::vtk::readVtkAsciiMesh(ifile,mesh);
	int ncells = SMeshC2VBase::getNumberOfCells(mesh.cells);
	CellCompare shortestEdgeOrder( mesh.vertices, mesh.cells );
	std::set<int,CellCompare> orderedCells( Sequence(0), Sequence(ncells), shortestEdgeOrder ) ;
	int cell = * orderedCells.begin();
	int nedges = Cell2EdgesTraits::getCellNumberOfEdges(mesh.cells,cell);
	std::cout<<"Cell with ortest edge : "<<cell<<" has "<<nedges<<" edges :\n";
	for(int i=0;i<nedges;i++)
	{
		auto edge = Cell2EdgesTraits::getCellEdge(mesh.cells,cell,i);
		std::cout<<"edge "<<edge<<" length = "<<edgeLength(mesh.vertices,edge)<<"\n";
	}
	
	std::cout<<"other cell ? "; std::cout.flush();
	std::cin>>cell;
	nedges = Cell2EdgesTraits::getCellNumberOfEdges(mesh.cells,cell);
	std::cout<<"Cell "<<cell<<" has "<<nedges<<" edges :\n";
	for(int i=0;i<nedges;i++)
	{
		auto edge = Cell2EdgesTraits::getCellEdge(mesh.cells,cell,i);
		std::cout<<"edge "<<edge<<" length = "<<edgeLength(mesh.vertices,edge)<<"\n";
	}	return 0;
}
#endif

