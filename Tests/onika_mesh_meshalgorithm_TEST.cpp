#include "onika/mesh/meshalgorithm.h"

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

