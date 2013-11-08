#include "onika/mesh/vertex2cell.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "onika/vtk/readvtkascii.h"
#include "onika/mesh/cell2vertex.h"
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

	int ca=0, cb=1;
	std::cout<<"Cell A ? "; std::cout.flush();
	std::cin>>ca;
	for(int i=0;i<v2c.getCellNumberOfVertices(ca);i++) std::cout<<v2c.getCellVertexId(ca,i)<<' ';
	std::cout<<"\nCell B ? "; std::cout.flush();
	std::cin>>cb;
	for(int i=0;i<v2c.getCellNumberOfVertices(cb);i++) std::cout<<v2c.getCellVertexId(cb,i)<<' ';
	std::cout<<"\n";
	v2c.swapCells( ca, cb);

	int v=0;
	std::cout<<"\nVertex A ? "; std::cout.flush();
	std::cin>>v;
	int adjacentCells[64], nAdj=0;
	v2c.getVertexAdjacentCells( v, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";
	int vb=0;
	std::cout<<"\nVertex B ? "; std::cout.flush();
	std::cin>>vb;
	v2c.getVertexAdjacentCells( vb, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	v2c.getEdgeAdjacentCells( v, vb, adjacentCells,  nAdj );
	std::cout<<nAdj<<" edge adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	onika::debug::dbgassert( v2c.checkConsistency() );

        std::cout<<std::endl<<"PASSED"<<std::endl;
        return 0;
}

 // end of file
