#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/cell2edge.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/mesh/meshalgorithm.h"
#include "onika/container/sequence.h"
#include "onika/debug/dbgassert.h"
#include "onika/language.h"
#include "onika/codec/asciistream.h"
#include "onika/compress/edgecompress.h"

#include <vtkUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkDataArrayTemplate.h>

using std::cout;
using namespace onika::vtk;
using onika::mesh::edge_length_op;
using onika::mesh::cell_shortest_edge_less;
using onika::mesh::make_smesh_c2e;
using onika::mesh::ordered_cell_set;
using onika::tuple::types;

template<int... I>
using integers = onika::tuple::indices<I...>;

// convinient operators for std::stream
template<class... T> inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t ) { onika::tuple::print( out, t ); return out; }
template<class T> inline std::ostream& operator << ( std::ostream& out, onika::container::ConstElementAccessorT<T> t ) { out << t.get(); return out; }
template<class T> inline std::ostream& operator << ( std::ostream& out, onika::container::ElementAccessorT<T> t ) { out << t.get(); return out; }

#ifndef UGRID_DESC
#include "testdata.h"
#endif

struct UGridDataPointers
{
	void* mesh;
	void** cellArrayPtrs;
	size_t* cellArraySizes;
	void** pointArrayPtrs;
	size_t* pointArraySizes;
	size_t meshSize;
};

bool vtkUGridSMeshCompress(vtkDataObject* data, int nedges, const std::string& outfname)
{
	if( data == 0 ) return false;
//	cout<<"OutputDataObject:\n";
//	data->PrintSelf(cout,vtkIndent(0));

	UGridWrapper<UGRID_DESC> wrapper( data );

	// wrap mesh arrays
	auto cellValues = wrapper.cellValues(); // cell centered values
	auto vertices = wrapper.vertices(); // vertex position and values
	auto cells = wrapper.cells(); // cell-to-vertex connectivity

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(data);
	vtkIdType nverts = ugrid->GetNumberOfPoints();
	vtkIdType nCells = ugrid->GetNumberOfCells();

	// wraps direct (cell to vertex) and build reverse (vertex to cell) connectivity
	auto c2v = wrap_ugrid_smesh_c2v( cells, ONIKA_CONST(3) );
	auto v2c = make_v2c( c2v , nverts );
	onika::debug::dbgassert( v2c.checkConsistency() );
	cout<<nverts<<" vertices, "<<nCells <<" cells, mem="<<onika::container::memory_bytes(cells)<<"\n";

	// build edge length metric and shortest edge based cell ordering
	// when vertices have a complex type i.e. tuple of values, each of which can be a tuple,
	// edge_length_op takes the first element and compute the distance based on the first element only.
	// this is why it is important to place vertex coordinates first in the vertex definition
	auto edgeLength = edge_length_op(vertices);
	auto c2e = make_smesh_c2e(c2v);
	auto shortestEdgeOrder = cell_shortest_edge_less( c2e, edgeLength);
	auto orderedCells = ordered_cell_set(nCells, shortestEdgeOrder);

	std::ofstream ofile(outfname.c_str());
    onika::codec::AsciiStream out(ofile);
	cout<<"\n-------------- start compressing ----------------\n";
	for(int c=0;c<nedges;c++)
	{
		auto minCell = * orderedCells.begin();
		int minCellEdges = c2e.getCellNumberOfEdges(minCell);
		auto edge = c2e.getCellEdge(minCell,0);
		for(int i=1;i<minCellEdges;i++)
		{
			auto edge2 = c2e.getCellEdge(minCell,i);
			if( edgeLength(edge2) < edgeLength(edge) ) edge = edge2;
		}
		cout<<"Cell #"<<minCell<<", edge "<<edge<<" length = "<<edgeLength(edge)<<"\n";

		onika::compress::smeshEdgeCollapseEncode(v2c, vertices, cellValues, edge, out);
	}
	onika::debug::dbgassert( v2c.checkConsistency() );

	return 0;
}
