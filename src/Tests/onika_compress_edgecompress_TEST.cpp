#include "onika/compress/edgecompress.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <tuple>

#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/vertex2cell.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/vtk/readvtkascii.h"
#include "onika/codec/asciistream.h"
#include "onika/tuple.h"

struct Mesh
{
	typedef struct { double x[3]; } VertexPos;

	inline Mesh() : nverts(0) {}

	inline void addVertex(const VertexPos& p)
	{
		vertices.push_back( std::make_tuple( std::make_tuple(p.x[0],p.x[1],p.x[2]) , (double)nverts ) );
		nverts++;
	}
	
	template<class VertIdIterator>
	inline void addCell(int np, VertIdIterator it)
	{
		if( np != 4 ) return;
		for(int i=0;i<np; ++i, ++it) cells.push_back( *it );
	}

	inline size_t getNumberOfCells() const { return cells.size()/4; }
	inline size_t getNumberOfVertices() const { return vertices.size(); }

	template<class Iterator>
	void addCellScalars(const std::string& name, Iterator first, Iterator last)
	{
		onika::debug::dbgmessage() << "Add cell scalars "<<name<<"\n";
		cellScalars.clear();
		for(Iterator it=first; it!=last; ++it)
		{
			cellScalars.push_back( *it );
		}
	}

	template<class Iterator>
	void addVertexScalars(const std::string& name, Iterator first, Iterator last)
	{
		onika::debug::dbgmessage() << "Add vertex scalars "<<name<<"\n";
		size_t i=0;
		for(Iterator it=first; it!=last; ++it, ++i )
		{
			std::get<1>( vertices[i] ) = *it;
		}
		onika::debug::dbgassert( i == vertices.size() );
	}

	std::vector<int> cells;
	int nverts;
	std::vector< std::tuple< std::tuple<double,double,double> , double > > vertices;
	std::vector< double > cellScalars;
};

// define how connectivity is mapped to your data storage
typedef onika::mesh::smesh_c2v_basic_traits< std::vector<int>, 3 > MyC2VBasicTraits;
typedef onika::mesh::c2v_traits< MyC2VBasicTraits > MyC2VTraits;
typedef onika::mesh::C2VWrapper<MyC2VTraits> MyC2VWrapper;
typedef onika::mesh::ReverseC2V<MyC2VWrapper, std::vector<int>, std::vector<unsigned int> > V2C;

int main(int argc, char* argv[])
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand( seed );

	std::string fname="data/tetraMesh.vtk";
	bool autorun = false;

	for(int i=1;i<argc;++i)
	{
		if( std::string(argv[i]) == "-a" )
		{
			autorun = true;
		}
		if( std::string(argv[i]) == "-i" )
		{
			++i; fname = argv[i];
		}
	}
	
	std::cout<<"input = "<<fname<<"\n";

	std::ifstream ifile(fname.c_str());
	if( !ifile )
	{
		std::cerr<<"Failed to open "<<fname<<"\n";
		return 1;
	}

	Mesh mesh;
	onika::vtk::readVtkAsciiMesh(ifile,mesh);
	onika::vtk::readVtkAsciiScalars(ifile,mesh);
	
	V2C v2c( mesh.cells, mesh.nverts ); 
	//onika::debug::dbgassert( v2c.checkConsistency(mesh.nverts) );
	
	std::cout<<v2c.getNumberOfVertices()<<" vertices, "<< v2c.getNumberOfCells()<<" cells, mem="<<v2c.getMemoryBytes()<<"\n";

	int v=0;
	if( autorun ) { v = (int)( drand48()*(mesh.nverts-1) ); std::cout<<"Vertex A = "<<v<<"\n"; }
	else {
		std::cout<<"\nVertex A ? "; std::cout.flush();
		std::cin>>v;
	}
	
	int adjacentCells[64], nAdj=0;
	v2c.getVertexAdjacentCells( v, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	int adjacentVertices[64], nAdjV=0;
	v2c.getVertexAdjacentVertices( v, adjacentVertices,  nAdjV );
	std::cout<<nAdjV<<" adjacent vertices : "; 
	for(int i=0;i<nAdjV;i++) { std::cout<<adjacentVertices[i]<<' '; } std::cout<<"\n";

	int vb=0;
	if( autorun ) { vb = (int)( drand48()*(nAdjV-1) ); vb = adjacentVertices[vb]; std::cout<<"Vertex B = "<<vb<<"\n"; }
	else {
		std::cout<<"\nVertex B ? "; std::cout.flush();
		std::cin>>vb;
	}
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

	onika::codec::AsciiStream out(std::cout);
	onika::compress::smeshEdgeCollapseEncode( v2c, mesh.vertices, mesh.cellScalars, v, vb, out );

	onika::debug::dbgassert( v2c.checkConsistency() );

        std::cout<<std::endl<<"PASSED"<<std::endl;
	return 0;
}


 // end of file
