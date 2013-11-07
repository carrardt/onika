#include "onika/mesh/cell2vertex.h"
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

 // end of file

