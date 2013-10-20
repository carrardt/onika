#ifndef __onika_mesh_cell2vertex_h
#define __onika_mesh_cell2vertex_h

#include <algorithm>
#include "onika/container/algorithm.h"
#include "onika/debug/dbgassert.h"


namespace onika { namespace mesh {

/*
Notes:
	je ne sais pas ou sont stockés les indices des vertex ni dans quel ordre, d'ou le swapVertices
*/
// here just as an exemple. ContrainerType and IdType types shoud be defined. all these methods should be present also
struct example_c2v_basic_traits
{
	typedef int* ContainerType;
	typedef int IdType;
	// =================================================================================
	// ============================ read only access ===================================
	// =================================================================================
	static constexpr IdType getMaxCellVertices(const ContainerType& c) { return 4; }
	static inline IdType getNumberOfCells(const ContainerType& c) { return 100; }
        static inline IdType getTotalNumberOfCellVertices(const ContainerType& c) { return 100*4; }
	static inline IdType getCellNumberOfVertices(const ContainerType& c, IdType i) { return 4; }
	static inline IdType getCellVertexId(const ContainerType& c, IdType cell, IdType i) { return 0; }
	// =================================================================================
	// ============================ write access =======================================
	// =================================================================================
	static inline void setCellVertexId(ContainerType& c, IdType cell, IdType i, IdType j) {}
	static inline void setCellNumberOfVertices(ContainerType& c, IdType cell, IdType n)  {}
	static inline void eraseVertex(ContainerType& c, IdType cell, IdType v)  {}
	static inline void eraseCell(ContainerType& c, IdType cell)  {}
	static inline void swapVertices(ContainerType& c, IdType cA, IdType vA, IdType cB, IdType vB) { }
	static inline void moveVertexTo(ContainerType& c, IdType cA, IdType vA, IdType cB, IdType vB) {}
	static inline void removeLastNCells(ContainerType& c, IdType n) {}
};

/* complete traits from basic traits */
template<class c2v_basic_traits>
struct c2v_traits : public c2v_basic_traits
{
	typedef typename c2v_basic_traits::ContainerType ContainerType;
	typedef typename c2v_basic_traits::IdType IdType;

	// these 3 methods are here for convinience only, to associate a unique integer value to each pair (cell,vertex index in cell)
	static inline IdType getCellVertexIdLocation(const ContainerType& c,IdType cell, IdType i)
	{
		return c2v_basic_traits::getMaxCellVertices(c) * cell + i;
	}
	static inline IdType getMaxCellVertexIdLocation(const ContainerType& c)
	{
		return c2v_basic_traits::getMaxCellVertices(c) * c2v_traits::getNumberOfCells();
	}
	static inline IdType getCellFromVertexIdLocation(const ContainerType& c, IdType i)
	{
		return i / c2v_basic_traits::getMaxCellVertices(c);
	}

	static inline bool checkConsistency(const ContainerType& c2v, IdType nVertices)
	{
#ifdef _DEBUG
#define CHECK_PREDICATE(p) debug::dbgassert(p)
#else
#define CHECK_PREDICATE(p) if(!(p)) return false
#endif
		IdType nCells = c2v_basic_traits::getNumberOfCells(c2v);
		for(IdType i=0;i<nCells;i++)
		{
			IdType nCellVerts = c2v_basic_traits::getCellNumberOfVertices(c2v,i);
			for(IdType j=0;j<nCellVerts;j++)
			{
				IdType vertexId = c2v_basic_traits::getCellVertexId(c2v,i,j);
				CHECK_PREDICATE( vertexId>=0 && vertexId<nVertices );
			}
		}
#undef CHECK_PREDICATE
		return true;
	}

	// two cells are swapped
	static inline void swapCells(ContainerType& c, IdType i, IdType j)
	{
		debug::dbgassert( "Swapping different kind of cells is not yet supported" && c2v_traits::getCellNumberOfVertices(c,i) == c2v_traits::getCellNumberOfVertices(c,j) );
		IdType n = c2v_traits::getCellNumberOfVertices(c,i);
		for(IdType v=0;v<n;v++)
		{
			c2v_basic_traits::swapVertices(c, i,v, j,v);
		}
	}

	// cell i overwrites cell j. cell i becomes undefined/erased
	static inline void moveCellTo(ContainerType& c, IdType i, IdType j)
	{
		if( i != j )
		{
			IdType n = c2v_basic_traits::getCellNumberOfVertices(c,i);
			c2v_basic_traits::setCellNumberOfVertices(c,j,n);
			for(IdType v=0;v<n;v++)
			{
				c2v_basic_traits::moveVertexTo(c, i,v, j,v);
			}
			c2v_basic_traits::eraseCell(c,i);
		}
	}
};


template<class _traits>
struct C2VWrapper
{
	typedef _traits traits;
	typedef typename traits::ContainerType ContainerType;
	typedef typename traits::IdType IdType;
	inline C2VWrapper(ContainerType& c) : c2v(c) {}

	// base methods
	inline IdType getMaxCellVertices() const { return traits::getMaxCellVertices(c2v); }
	inline IdType getNumberOfCells() const { return traits::getNumberOfCells(c2v); }
        inline IdType getTotalNumberOfCellVertices() const { return traits::getTotalNumberOfCellVertices(c2v); }
	inline IdType getCellNumberOfVertices(IdType i) const { return traits::getCellNumberOfVertices(c2v,i); }
	inline IdType getCellVertexId(IdType cell,IdType i) const { return traits::getCellVertexId(c2v,cell,i); }	

	// extended methods
	inline IdType getCellVertexIdLocation(IdType cell, IdType i) const { return traits::getCellVertexIdLocation(c2v,cell,i); } 
	inline IdType getCellFromVertexIdLocation(IdType i) const { return traits::getCellFromVertexIdLocation(c2v,i); }
	inline bool checkConsistency(IdType nVertices) const { return traits::checkConsistency(c2v,nVertices); }
	inline size_t getMemoryBytes() const { return container::memory_bytes(c2v); }

	// modifying methods
	inline void setCellVertexId(IdType cell, IdType i, IdType j) { traits::setCellVertexId(c2v,cell,i,j); }
	inline void setCellNumberOfVertices(IdType cell, IdType n) { traits::setCellNumberOfVertices(c2v,cell,n); }
	inline void eraseCell(IdType cell) { traits::eraseCell(c2v,cell); }
	inline void swapVertices(IdType cA, IdType vA, IdType cB, IdType vB) { traits::swapVertices(c2v,cA,vA,cB,vB); }
	inline void moveVertexTo(IdType cA, IdType vA, IdType cB, IdType vB) { traits::moveVertexTo(c2v,cA,vA,cB,vB); }
	inline void swapCells(IdType i, IdType j) { traits::swapCells(c2v,i,j); }
	inline void moveCellTo(IdType i, IdType j) { traits::moveCellTo(c2v,i,j); }
	inline void removeLastNCells(IdType n) { traits::removeLastNCells(c2v,n); }

	// reference to object holding c2v
	ContainerType& c2v;
};

} } // end of namespace


// ==========================================================
// =================== Unit Test ============================
// ==========================================================

#ifdef onika_mesh_cell2vertex_TEST
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "onika/vtk/readvtkascii.h"
#include "onika/mesh/simplicialmesh.h"

struct Mesh
{
	typedef struct { double x[3]; } VertexPos;

	inline Mesh() : nverts(0) {}

	inline void addVertex(const VertexPos& p) { nverts++; }
	
	template<class VertIdIterator>
	inline void addCell(int np, VertIdIterator it)
	{
		if( np != 4 ) return;
		for(int i=0;i<np; ++i, ++it) cells.push_back( *it );
	}

	std::vector<int> cells;
	int nverts;
};

typedef onika::mesh::smesh_c2v_basic_traits< std::vector<int>, 3 > MyC2VBasicTraits;
typedef onika::mesh::c2v_traits< MyC2VBasicTraits > MyC2VTraits;
typedef onika::mesh::C2VWrapper<MyC2VTraits> MyC2VWrapper;

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
	MyC2VWrapper c2v( mesh.cells ); 
	onika::debug::dbgassert( c2v.checkConsistency(mesh.nverts) );
	
	std::cout<<mesh.nverts<<" vertices, "<< c2v.getNumberOfCells()<<" cells, mem="<<onika::container::memory_bytes(mesh.cells)<<"\n";

	int ca=0, cb=1;
	std::cout<<"Cell A ? "; std::cout.flush();
	std::cin>>ca;
	for(int i=0;i<c2v.getCellNumberOfVertices(ca);i++) std::cout<<c2v.getCellVertexId(ca,i)<<' ';
	std::cout<<"\nCell B ? "; std::cout.flush();
	std::cin>>cb;
	for(int i=0;i<c2v.getCellNumberOfVertices(cb);i++) std::cout<<c2v.getCellVertexId(cb,i)<<' ';
	std::cout<<"\n";
	c2v.swapCells(ca,cb);
	
	std::cout<<"Cell #"<<ca<<" : "; for(int i=0;i<c2v.getCellNumberOfVertices(ca);i++) std::cout<<c2v.getCellVertexId(ca,i)<<' '; std::cout<<"\n";
	std::cout<<"Cell #"<<cb<<" : "; for(int i=0;i<c2v.getCellNumberOfVertices(cb);i++) std::cout<<c2v.getCellVertexId(cb,i)<<' '; std::cout<<"\n";

        std::cout<<std::endl<<"PASSED"<<std::endl;
        return 0;
}

#endif // end of unit test

#endif // end of file

