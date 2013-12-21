#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/debug/dbgassert.h"
#include "onika/container/iterator.h"
#include "onika/container/tuplevec.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/vertex2cell.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/mesh/meshalgorithm.h" // cellminedgelength
#include "onika/tuple.h"
#include "onika/mathfunc.h"
#include "onika/language.h"  // ONIKA_AUTO_RET macro
#include "onika/container/sequence.h" // Sequence iterator

#include <tuple>
#include <string>
#include <set>
#include <iostream>
#include <fstream>

// VTK-onika bridge types
#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"

// VTK dependencies
#include <vtkUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

// TODO: progressively get rid of the need to buil types prior to use them.
// everything would be much more readable and easy to use with in-function, auto declared variables
// initialized with template functions.

// Vertex-Cell 2-way connectivity
typedef onika::container::ArrayWrapper<vtkIdType> MyCellContainer;
typedef onika::mesh::smesh_c2v_basic_traits<MyCellContainer,3,1> MyC2VBasicTraits;
typedef onika::mesh::c2v_traits<MyC2VBasicTraits> MyC2VTraits;
typedef onika::mesh::C2VWrapper<MyC2VTraits> MyC2VWrapper;
typedef onika::mesh::ReverseC2V<MyC2VWrapper, std::vector<int>, std::vector<unsigned int> > V2C;

// Edge view
typedef onika::mesh::smesh_c2e_basic_traits<MyC2VBasicTraits> Cell2EdgesTraits;

// Cell Values Adapter
typedef onika::container::ArrayWrapper<double> MyCellValueContainer;

// algorithms applied to mesh elements
using onika::mesh::edge_length_op;
using onika::mesh::CellShortestEdge;
using onika::container::SequenceIterator;

template<class Integer, class CellCompare>
inline auto ordered_cell_set(Integer nCells, CellCompare order)
ONIKA_AUTO_RET( std::set<Integer,CellCompare>(SequenceIterator<Integer>(0), SequenceIterator<Integer>(nCells), order) )

// ostream operator for tuples
template<class... T>
inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
{
	onika::tuple::print( out, t );
	return out;
}

using namespace onika::vtk;

bool onikaEncodeMesh(vtkUnstructuredGrid* input, vtkUnstructuredGrid* output, int nedges, const std::string& outputFileName)
{
	std::ofstream ofile(outputFileName);
	if(!ofile)
	{
		std::cerr<<"Couldn't open '"<<outputFileName<<"' for writing\n";
		return false;
	}

	std::cout<<"Output = "<<outputFileName<<"\n";
	std::cout<<"Edges = "<<nedges<<"\n";

	if( input == 0 ) return false;
	if( ! allCellsAreTetras(input) )
	{
		cout<<"Not a simplicial mesh\n";
		return false;
	}

	// build 2-way cell-vertex connectivity adpater
	vtkIdType* cellsPtr = input->GetCells()->GetData()->GetPointer(0);
	size_t cellsSize = input->GetCells()->GetData()->GetNumberOfTuples();
	MyCellContainer cells = { cellsPtr+1/*first is cell size*/, cellsSize, cellsSize };
	vtkIdType nVerts = input->GetNumberOfPoints();
	V2C v2c( cells, nVerts );
	onika::debug::dbgassert( v2c.checkConsistency() );
	
	int nCells = v2c.getNumberOfCells();
	std::cout<<v2c.getNumberOfVertices()<<" vertices, "<< v2c.getNumberOfCells()<<" cells, mem="<<v2c.getMemoryBytes()<<"\n";

	// build vertices adapter
	cout<<"Points: "<<input->GetPoints()->GetData()->GetClassName()<<"\n";
	vtkFloatArray* vtkpoints = vtkFloatArray::SafeDownCast(input->GetPoints()->GetData());
	if( vtkpoints == 0 )
	{
		cout<<"Points array is not a vtkFloatArray\n";
		return false;
	}
	if( vtkpoints->GetNumberOfComponents() != 3 )
	{
		cout<<"Points array doesn't have 3 components\n";
	}

	//auto vertices = wrap_vtkarray_tuple<3>( vtkpoints ); // slightly slower
	auto vertices = wrap_vtkarray_tuple_rev<3>( vtkpoints );

	auto edgeLength = edge_length_op(vertices);
	auto shortestEdgeOrder = CellShortestEdge<Cell2EdgesTraits>::less( cells, edgeLength );
	auto orderedCells = ordered_cell_set(nCells, shortestEdgeOrder);
#if 0
	for( auto c : orderedCells )
	{
		cout<<"cell "<<c<<" :";
		int nCellEdges = Cell2EdgesTraits::getCellNumberOfEdges(cells,c);
		for(int i=0;i<nCellEdges;i++)
		{
			auto edge = Cell2EdgesTraits::getCellEdge(cells,c,i);
			cout<<" "<<edge;
		}
		cout<<"\n";
	}
#endif

	int minCell = * orderedCells.begin();
	int minCellEdges = Cell2EdgesTraits::getCellNumberOfEdges(cells,minCell);
	std::cout<<"Cell with shortest edge is #"<<minCell<<" and has "<<minCellEdges<<" edges :\n";
	auto edge = Cell2EdgesTraits::getCellEdge(cells,minCell,0);
	for(int i=1;i<minCellEdges;i++)
	{
		auto edge2 = Cell2EdgesTraits::getCellEdge(cells,minCell,i);
		if( edgeLength(edge2) < edgeLength(edge) ) edge = edge2;
	}
	cout<<"edge "<<edge<<" length = "<<edgeLength(edge)<<"\n";

	vtkIdType adjacentCells[64], nAdj=0;
	v2c.getEdgeAdjacentCells( std::get<0>(edge), std::get<1>(edge), adjacentCells,  nAdj );
	std::cout<<nAdj<<" edge adjacent cells : ";
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	// add scalars to vertices
	vtkDataArray* vertexScalarsArray = input->GetPointData()->GetArray("Temp");
	if( vertexScalarsArray == 0 )
	{
		cout<<"Point scalars not found\n";
		return false;
	}
	vtkFloatArray * vertexScalarsFloatArray = vtkFloatArray::SafeDownCast( vertexScalarsArray );
	if( vertexScalarsFloatArray == 0 )
	{
		cout<<"Point scalars is not a vtkFloatArray\n";
		return false;
	}
	auto vertexScalars = wrap_vtkarray( vertexScalarsFloatArray );
	cout<<"Vertex scalars:";
	for( auto x : vertexScalars ) { cout<<' '<<x; }
	cout<<"\n";

	// wrap vertex and cell scalars, create output stream and start compressing
        //onika::codec::AsciiStream out(ofile);
        //onika::compress::smeshEdgeCollapseEncode( v2c, vertices, cellScalars, v, vb, out );

        //onika::debug::dbgassert( v2c.checkConsistency() );

    cout<<"\nDONE\n";
    return true;
}


