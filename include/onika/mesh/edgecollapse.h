#ifndef __onika_mesh_edgecollapse_h
#define __onika_mesh_edgecollapse_h

#include <algorithm>
#include "onika/debug/dbgassert.h"
#include "onika/container/algorithm.h" // for NullContainer

namespace onika { namespace mesh {

	// =================================================================================
	// ======================== smeshEdgeCollapseProlog ================================
	// =================================================================================
	// === contraction of edge a,b : b vanishes to a. a position is unchanged.
	// === b is swaped with the last vertex.
	// === Works on simplicial cells only.
	//
	// TODO: rajouter tests avec cas pathologiques : vertex a == vertices.size()-1, etc.
	//
	template<class Mesh, class IdType, class Iterator1, class Iterator2, class Iterator3>
	inline void smeshEdgeCollapseProlog(
		Mesh& mesh, 				     // Mesh is a ReverC2V instance or derived class
		IdType a, IdType b,			     // indices of vertices to merge
		Iterator1 adjCells_begin, IdType& nAdjCells, // cells adjacent to a' after edge collapse
		Iterator2 dispCells_begin, IdType& nDisp,    // cells that have a vertex displaced from b to a
		Iterator3 removedCells_begin, IdType& nEdgeAdjCells // cells to update to collapse b onto a
		)
		//const
	{
		IdType nCells = mesh.getNumberOfCells();

		IdType adjCellsA[Mesh::MAX_ADJACENT_CELLS], nca=0;
		mesh.getVertexAdjacentCells(a,adjCellsA,nca);
		std::sort(adjCellsA,adjCellsA+nca);

		IdType adjCellsB[Mesh::MAX_ADJACENT_CELLS], ncb=0;
		mesh.getVertexAdjacentCells(b,adjCellsB,ncb);
		std::sort(adjCellsB,adjCellsB+ncb);

		// cells to remove
		Iterator3 removedCellsEnd = std::set_intersection(adjCellsA,adjCellsA+nca,adjCellsB,adjCellsB+ncb,removedCells_begin);
		nEdgeAdjCells = removedCellsEnd - removedCells_begin; // (!) might be inefficient

		// cells to update ( b -> a )
		Iterator2 dispCellsEnd = std::set_difference(adjCellsB,adjCellsB+ncb,removedCells_begin,removedCellsEnd,dispCells_begin);
		nDisp = dispCellsEnd - dispCells_begin;

		// cells adjacent to a'
		IdType adjCellsAB[Mesh::MAX_ADJACENT_CELLS] ;
		IdType nAdjAB = std::set_union(adjCellsA,adjCellsA+nca,adjCellsB,adjCellsB+ncb,adjCellsAB) - adjCellsAB;
		nAdjCells = std::set_difference(adjCellsAB,adjCellsAB+nAdjAB,removedCells_begin,removedCellsEnd,adjCells_begin) - adjCells_begin;

		debug::dbgassert( std::includes(adjCells_begin,adjCells_begin+nAdjCells,dispCells_begin,dispCellsEnd) );
	}


	template<class Mesh, class IdType, class Iterator1, class Iterator2, class Iterator3, class Iterator4, class Container5>
	inline void smeshEdgeCollapseCoCells(
		Mesh& mesh,
		IdType a, IdType b,
		Iterator1 adjCells_begin, IdType nAdjCells, // cells adjacent to a' after edge collapse
		Iterator2 dispCells_begin, IdType nDisp, // cells that have a vertex displaced from b to a
		Iterator3 removedCells_begin, IdType nEdgeAdjCells, // cells to update to collapse b onto a

		Iterator4 removedCoCells_begin,  // removedCoCells size >= nEdgeAdjCells
		Container5 removedCellVertices // removedCellVertices size >= nEdgeAdjCells*(NDim-1)
		)
		//const
	{
		constexpr unsigned int NDim = Mesh::traits::NDim;
				
		// =============== build list of removed cells' co-cells ================
		// ==> work for simplices only <==
		Iterator4 removedCoCells_it = removedCoCells_begin;
		IdType nRemovedCoCells = 0;
		Iterator3 removedCells_it = removedCells_begin;
		Iterator2 dispCells_it;
		for(IdType ci=0;ci<nEdgeAdjCells; ++ci, ++removedCells_it)
		{
			IdType c = *removedCells_it;
			IdType coCell = c; // refering to itself as a co-cell means no co-cell
			IdType nv = mesh.getCellNumberOfVertices(c);
			debug::dbgassert( nv == (NDim+1) );
			IdType np=0;
			for(IdType i=0;i<nv;i++)
                        {
				IdType v = mesh.getCellVertexId(c,i);
				if( v!=a && v!=b ) removedCellVertices[ ci*(NDim-1) + (np++) ] = v;
			}
			debug::dbgassert( np == (nv-2) ); // because it's adjacent to both a & b
			// look for a cell in disp cells that has exactly NDim-1 vertices in p array
			dispCells_it = dispCells_begin;
			for(IdType dc=0; (dc<nDisp) && (coCell==c) ; ++dc, ++dispCells_it)
			{
				IdType c2 = *dispCells_it;
				IdType nshared=0;
				IdType nv2 = mesh.getCellNumberOfVertices(c2);
				for(IdType i=0;i<nv2;i++)
				{
					IdType v = mesh.getCellVertexId(c2,i);
					if( v!=a && v!=b )
					{
						bool found=false;
						for(IdType j=0;j<np && !found;j++)
						{
							if( v == removedCellVertices[ci*(NDim-1)+j] ) found=true;
						}
						if(found) ++nshared;
					}
				}
				if( nshared == (NDim-1) )
				{
					// debug::dbgmessage() << "cell #"<<c<<" has co-cell #"<<c2<<"\n";
					coCell = c2;
				}
			}
			*removedCoCells_it = coCell; ++removedCoCells_it; ++nRemovedCoCells;
		}
		debug::dbgassert( nRemovedCoCells == nEdgeAdjCells );
	}
	
        // =================================================================================
        // ======================== smeshEdgeCollapseUnsafe ================================
        // =================================================================================
	// === calling this method renders mesh inconsistent until a call to smeshEdgeCollapseEpilog
	//
	template<class Mesh, class IdType, class Iterator>
	inline void smeshEdgeCollapseUnsafe(
			Mesh& mesh,
			IdType a, IdType b,
			Iterator adjCells_begin, IdType nAdjCells, // IN/OUT: cells adjacent to a' after edge collapse
			Iterator dispCells_begin, IdType nDisp, // IN/OUT: cells that have a vertex displaced from b to a
			Iterator removedCells_begin, IdType nEdgeAdjCells // IN:
			)
	{
		// delete vertex b
		IdType nCells = mesh.getNumberOfCells();

		// remove references to removed cells
		Iterator removedCells_it = removedCells_begin;
		for( IdType ci=0 ; ci<nEdgeAdjCells ; ++removedCells_it, ++ci )
		{
			mesh.removeReferencesToCell( *removedCells_it );
		}
		Iterator removedCells_end = removedCells_it;

		// remove references to displaced cells
		Iterator dispCells_it = dispCells_begin;
		for(IdType ci=0;ci<nDisp;++ci,++dispCells_it)
		{
			mesh.removeReferencesToCell( *dispCells_it );
		}
		Iterator dispCells_end = dispCells_it;

		// replace references to Vertex b by references to Vertex a in displaced cells
		// add free'd space of b's v2c to a
		// devrait etre regroupe et remplace par un appel de fonction
		mesh.vertexAdjacentCellClear(b);
		mesh.vertexAdjacentCellChain(a,b);
		for( dispCells_it=dispCells_begin ; dispCells_it!=dispCells_end ; ++dispCells_it )
		{
			IdType cellId = *dispCells_it;
			IdType ncellverts = mesh.getCellNumberOfVertices(cellId);
			for(IdType cv=0;cv<ncellverts;cv++) 
			{
				if( mesh.getCellVertexId(cellId,cv) == b )
				{
					mesh.setCellVertexId(cellId,cv,a);
				}
			}
		}

		// add back references to displaced cells
		for(dispCells_it=dispCells_begin;dispCells_it!=dispCells_end;++dispCells_it)
		{
			mesh.addReferencesToCell( *dispCells_it );
		}

	}

        // =================================================================================
        // ======================== smeshEdgeCollapseEpilog ================================
        // =================================================================================
	// === changes order of elements to go back to a consistent state
	// === (no void elements left in the middle)
	template<class Mesh, class VertexContainer, class CellContainer, class IdType, class Iterator>
	inline void smeshEdgeCollapseEpilog(
		Mesh& mesh, VertexContainer& vertices, CellContainer& cells,
		IdType a, IdType b, // cells that have a vertex displaced from b to a
		Iterator removedCells_begin, IdType nRemoved) // cells to update to collapse b onto a
	{
		// mark removed elements as erased
		mesh.eraseVertex(b);
		Iterator removedCells_it = removedCells_begin;
		for(IdType ci=0 ; ci<nRemoved ; ++removedCells_it, ++ci )
		{
			mesh.eraseCell( *removedCells_it );
		}

		// place erased elements at the end
		IdType lastVertex = mesh.getNumberOfVertices()-1;
		mesh.moveVertexTo(lastVertex, b);
		vertices[b] = vertices[lastVertex];
		IdType nCells = mesh.getNumberOfCells();
		removedCells_it = removedCells_begin;
		for(IdType ci=0; ci<nRemoved; ++ci, ++removedCells_it)
		{
			mesh.moveCellTo(nCells-nRemoved+ci,*removedCells_it);
			cells[*removedCells_it] = cells[nCells-nRemoved+ci];
		}

		// shrink cell and vertex arrays
		mesh.removeLastNCells(nRemoved);
		cells.resize( cells.size() - nRemoved );

		mesh.removeLastVertex();
		vertices.pop_back();
	}

	template<class Mesh, class IdType, class Iterator>
	inline void smeshEdgeCollapseEpilog(
		Mesh& mesh,
		IdType a, IdType b, // cells that have a vertex displaced from b to a
		Iterator removedCells_begin, IdType nRemoved) // cells to update to collapse b onto a
	{
		container::NullContainer vertices( mesh.getNumberOfVertices() );
		container::NullContainer cells( mesh.getNumberOfCells() );
		smeshEdgeCollapseEpilog( mesh, vertices, cells, a, b, removedCells_begin, nRemoved );
	}

	// =================================================================================
        // ============================ edgeCollapse =======================================
        // =================================================================================
	template<class Mesh, class IdType>    	
	inline void smeshEdgeCollapse(Mesh& mesh, IdType a, IdType b)
	{
		constexpr unsigned int MAX_ADJACENT_CELLS = Mesh::MAX_ADJACENT_CELLS;
		IdType adjCells[MAX_ADJACENT_CELLS];
		IdType dispCells[MAX_ADJACENT_CELLS];
		IdType removedCells[MAX_ADJACENT_CELLS];
		IdType nEdgeAdjCells=0,nAdjCells=0,nDisp=0;
		smeshEdgeCollapseProlog(mesh,a,b,adjCells,nAdjCells,dispCells,nDisp,removedCells,nEdgeAdjCells);
		smeshEdgeCollapseUnsafe(mesh,a,b,adjCells,nAdjCells,dispCells,nDisp,removedCells,nEdgeAdjCells);
		smeshEdgeCollapseEpilog(mesh,a,b,removedCells,nEdgeAdjCells);
	}


} } // end of namespace






//==============================================
//============= UNIT TEST ======================
//==============================================
#ifdef onika_mesh_edgecollapse_TEST

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/vertex2cell.h"
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
typedef onika::mesh::ReverseC2V<MyC2VWrapper, std::vector<int>, std::vector<unsigned int> > V2C;

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
	V2C v2c( mesh.cells, mesh.nverts ); 
	//onika::debug::dbgassert( v2c.checkConsistency(mesh.nverts) );
	
	std::cout<<v2c.getNumberOfVertices()<<" vertices, "<< v2c.getNumberOfCells()<<" cells, mem="<<v2c.getMemoryBytes()<<"\n";

	int v=0;
	std::cout<<"\nVertex A ? "; std::cout.flush();
	std::cin>>v;
	int adjacentCells[64], nAdj=0;
	v2c.getVertexAdjacentCells( v, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	int adjacentVertices[64], nAdjV=0;
	v2c.getVertexAdjacentVertices( v, adjacentVertices,  nAdjV );
	std::cout<<nAdjV<<" adjacent vertices : "; 
	for(int i=0;i<nAdjV;i++) { std::cout<<adjacentVertices[i]<<' '; } std::cout<<"\n";

	int vb=0;
	std::cout<<"\nVertex B ? "; std::cout.flush();
	std::cin>>vb;
	v2c.getVertexAdjacentCells( vb, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	v2c.getVertexAdjacentVertices( vb, adjacentVertices,  nAdjV );
	std::cout<<nAdjV<<" adjacent vertices : "; 
	for(int i=0;i<nAdjV;i++) { std::cout<<adjacentVertices[i]<<' '; } std::cout<<"\n";


	v2c.getEdgeAdjacentCells( v, vb, adjacentCells,  nAdj );
	std::cout<<nAdj<<" edge adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	onika::debug::dbgassert( v2c.checkConsistency() );

	std::cout<<"collapse edge "<<v<<"-"<<vb<<"\n";
	int dispCells[64], nDispCells, removedCells[64], nRemoved;
	
	onika::mesh::smeshEdgeCollapseProlog(v2c,v,vb,adjacentCells,nAdj,dispCells,nDispCells,removedCells,nRemoved);
	
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";
	
	std::cout<<nDispCells<<" displaced cells : "; 
	for(int i=0;i<nDispCells;i++) { std::cout<<dispCells[i]<<' '; } std::cout<<"\n";
	
	std::cout<<nRemoved<<" removed cells : "; 
	for(int i=0;i<nRemoved;i++) { std::cout<<removedCells[i]<<' '; } std::cout<<"\n";

	int removedCoCells[64];
	int removedCellsVertices[64*4];
	onika::mesh::smeshEdgeCollapseCoCells(v2c,v,vb,adjacentCells,nAdj,dispCells,nDispCells,removedCells,nRemoved,removedCoCells,removedCellsVertices);

	std::cout<<nRemoved<<" removed co-cells : "; 
	for(int i=0;i<nRemoved;i++) { std::cout<<removedCoCells[i]<<' '; } std::cout<<"\n";

	// actually do the whole edge contraction
	onika::mesh::smeshEdgeCollapse(v2c, v, vb);

	onika::debug::dbgassert( v2c.checkConsistency() );

        std::cout<<std::endl<<"PASSED"<<std::endl;
	return 0;
}
#endif // end of unit test

#endif // end of file
