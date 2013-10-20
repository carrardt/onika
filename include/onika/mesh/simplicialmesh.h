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
template<class _ContainerType, unsigned int _NDim>
struct smesh_c2v_basic_traits
{
	typedef _ContainerType ContainerType;
	typedef typename ContainerType::value_type IdType;

	// Simplex dimension
	static constexpr unsigned int NDim = _NDim;
	static constexpr IdType NVerts = NDim+1;

	// =================================================================================
	// ============================ read only access ===================================
	// =================================================================================
	static constexpr IdType getMaxCellVertices(const ContainerType& c) { return NVerts; }
	static inline IdType getNumberOfCells(const ContainerType& c) { return c.size()/NVerts; }
        static inline IdType getTotalNumberOfCellVertices(const ContainerType& c) { return c.size(); }
	static inline IdType getCellNumberOfVertices(const ContainerType& c, IdType i) { return NVerts; }
	static inline IdType getCellVertexId(const ContainerType& c, IdType cell, IdType i) { return c[cell*NVerts+i]; }


	// =================================================================================
	// ============================ write access =======================================
	// =================================================================================
	static inline void setCellVertexId(ContainerType& c, IdType cell, IdType i, IdType j) { c[ (cell*NVerts) + i ] = j; }
	static inline void setCellNumberOfVertices(ContainerType& c, IdType cell, IdType n)  // you _cannot_ modify number of vertices
	{
		debug::dbgassert( cell>=0 && cell<getNumberOfCells(c) );
		debug::dbgassert( n == NVerts );
	}
	static inline void eraseCell(ContainerType& c, IdType cell)  {}
	static inline void eraseVertex(ContainerType& c, IdType cell, IdType vert)  {}
	static inline void swapVertices(ContainerType& c, IdType cA, IdType vA, IdType cB, IdType vB)
	{
		std::swap( c[ (cA*(NDim+1)) + vA ] , c[ (cB*(NDim+1)) + vB ] );
	}
	static inline void moveVertexTo(ContainerType& c, IdType cA, IdType vA, IdType cB, IdType vB)
	{
		if( (cA != cB) || (vA != vB) )
		{
			c[ (cB*NVerts) + vB ] = c[ (cA*NVerts) + vA ] ;
			eraseVertex(c, cA, vA );
		}
	}
	
	static inline void removeLastNCells(ContainerType& c, IdType n)
	{
		c.resize( c.size() - n * (NDim+1) );
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

#ifdef onika_mesh_simplicialmesh_TEST
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "onika/vec.h"
#include "onika/vtk/readvtkascii.h"
#include "onika/mesh/meshalgorithm.h"

typedef onika::Vec<3,double> vec3;

int main()
{
	std::cout<<" 2-simplex number of vertices : "<< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,2>::NVerts << "\n";
	std::cout<<" 2-simplex number of edges : "<< onika::mesh::smesh_c2e_basic_traits< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,2> >::NEdges << "\n";
	std::cout<<" 3-simplex number of vertices : "<< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,3>::NVerts << "\n";
	std::cout<<" 3-simplex number of edges : "<< onika::mesh::smesh_c2e_basic_traits< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,3> >::NEdges << "\n";
	std::cout<<" 4-simplex number of vertices : "<< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,4>::NVerts << "\n";
	std::cout<<" 4-simplex number of edges : "<< onika::mesh::smesh_c2e_basic_traits< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,4> >::NEdges << "\n";

        std::cout<<std::endl<<"PASSED"<<std::endl;
        return 0;
}

#endif // end of unit test

#endif // end of file
