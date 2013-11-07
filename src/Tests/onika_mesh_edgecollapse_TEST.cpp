#include "onika/mesh/edgecollapse.h"

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
 // end of file
