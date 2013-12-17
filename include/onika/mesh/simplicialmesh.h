#ifndef __onika_simplicialmesh_h
#define __onika_simplicialmesh_h

#include "onika/debug/dbgassert.h"
#include "onika/codec/encoding.h"

namespace onika { namespace mesh {


/*
simple traits for simplicial cells of equal dimensions, with vertex ids packed serially into a container
exemple: for 3D case, cells are tetrahedras (4 vertices per cell). array will contain :
Cell1:Vertex1, Cell1:Vertex2, Cell1:Vertex3, Cell1:Vertex4, Cell2:Vertex1 ...
*/ 
template<class _ContainerType, unsigned int _NDim,int _CellGap=0>
struct smesh_c2v_basic_traits
{
	typedef _ContainerType ContainerType;
	typedef typename ContainerType::value_type IdType;

	// Simplex dimension
	static constexpr unsigned int NDim = _NDim;

	// number of vertices per cell
	static constexpr IdType NVerts = NDim+1;

	// empty vertex slots between two cells.
	// i.e. usefull for storage where you have NCellVerts,V1,V2...,Vn, NCellVerts, ...
	static constexpr IdType CellGap = _CellGap;

	// space between two cells
	static constexpr IdType CellStride = NVerts+CellGap;

	// =================================================================================
	// ============================ read only access ===================================
	// =================================================================================
	static constexpr IdType getMaxCellVertices(const ContainerType& c) { return NVerts; }
	static inline IdType getNumberOfCells(const ContainerType& c) { return c.size()/CellStride; }
    static inline IdType getTotalNumberOfCellVertices(const ContainerType& c) { return getNumberOfCells(c)*NVerts; }
	static inline IdType getCellNumberOfVertices(const ContainerType& c, IdType i) { return NVerts; }
	static inline IdType getCellVertexId(const ContainerType& c, IdType cell, IdType i) { return c[cell*CellStride+i]; }


	// =================================================================================
	// ============================ write access =======================================
	// =================================================================================
	static inline void setCellVertexId(ContainerType& c, IdType cell, IdType i, IdType j) { c[ (cell*CellStride) + i ] = j; }
	static inline void setCellNumberOfVertices(ContainerType& c, IdType cell, IdType n)  // you _cannot_ modify number of vertices
	{
		debug::dbgassert( cell>=0 && cell<getNumberOfCells(c) );
		debug::dbgassert( n == NVerts );
	}
	static inline void eraseCell(ContainerType& c, IdType cell)  {}
	static inline void eraseVertex(ContainerType& c, IdType cell, IdType vert)  {}
	static inline void swapVertices(ContainerType& c, IdType cA, IdType vA, IdType cB, IdType vB)
	{
		std::swap( c[ (cA*CellStride) + vA ] , c[ (cB*CellStride) + vB ] );
	}
	static inline void moveVertexTo(ContainerType& c, IdType cA, IdType vA, IdType cB, IdType vB)
	{
		if( (cA != cB) || (vA != vB) )
		{
			c[ (cB*CellStride) + vB ] = c[ (cA*CellStride) + vA ] ;
			eraseVertex(c, cA, vA );
		}
	}

	static inline void removeLastNCells(ContainerType& c, IdType n)
	{
		c.resize( c.size() - n * CellStride );
	}
};


/*
edge basic traits for simplicial meshes, built from cell-vertex traits
no write access, topology is fixed (simplices of equal dimensionality)
*/ 
template<class c2v_traits>
struct smesh_c2e_basic_traits
{
	typedef typename c2v_traits::ContainerType ContainerType;
	typedef typename ContainerType::value_type IdType;

	// Simplex dimension
	static constexpr unsigned int NDim = c2v_traits::NDim;
	static constexpr IdType NEdges = ((NDim+1)*(NDim))/2;

	// =================================================================================
	// ============================ read only access ===================================
	// =================================================================================
	static constexpr IdType getMaxCellEdges(const ContainerType& c) { return NEdges; }
	static inline IdType getNumberOfCells(const ContainerType& c) { return c2v_traits::getNumberOfCells(c); }
    static inline IdType getTotalNumberOfCellEdges(const ContainerType& c) { return getNumberOfCells(c) * NEdges; }
	static inline IdType getCellNumberOfEdges(const ContainerType& c, IdType i) { return NEdges; }
	static inline std::tuple<IdType,IdType> getCellEdge(const ContainerType& c, IdType cell, IdType e)
	{
		IdType v1,v2;
		codec::edge_dec( e, v1, v2 );
		v1 = c2v_traits::getCellVertexId(c,cell,v1);
		v2 = c2v_traits::getCellVertexId(c,cell,v2);
		return std::make_tuple(v1,v2);
	}
};

} } // namspace onika


// ==========================================================
// =================== Unit Test ============================
// ==========================================================

#endif // end of file
