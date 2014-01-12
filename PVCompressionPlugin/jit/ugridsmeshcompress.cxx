#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/cell2edge.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/mesh/meshalgorithm.h"
#include "onika/container/sequence.h"
#include "onika/debug/dbgassert.h"
#include "onika/language.h"
#include "onika/codec/asciistream.h"
#include "onika/compress/edgecompress.h"
#include "onika/sysio.h"

#include "ugriddesc.h"
#include "vtkugridtetrawrapper.h"

using namespace onika::vtk;
using onika::mesh::edge_length_op;
using onika::mesh::cell_shortest_edge_less;
using onika::mesh::make_smesh_c2e;
using onika::mesh::ordered_cell_set;
using onika::tuple::types;
using onika::sys::info;
using onika::sys::err;

template<int... I>
using integers = onika::tuple::indices<I...>;

// convinient operators for std::stream
template<class... T> inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t ) { onika::tuple::print( out, t ); return out; }
template<class T> inline std::ostream& operator << ( std::ostream& out, onika::container::ConstElementAccessorT<T> t ) { out << t.get(); return out; }
template<class T> inline std::ostream& operator << ( std::ostream& out, onika::container::ElementAccessorT<T> t ) { out << t.get(); return out; }

// must be provided by jitti at compile time
// #define UGRID_DESC <...>

// C function signature simplifies function lookup by its name (no c++ mangling)
extern "C"
{
	void initialize();
	int ugridsmeshcompress(vtkUGridDescription*,int,const char*);
}

void initialize()
{
	info()<<"ugridsmeshcompress up and running\n";
}

int ugridsmeshcompress(vtkUGridDescription *ugrid_desc, int nedges, const char* outfname)
{
	info()<<"ugridsmeshcompress started\n"; info().flush();

	if( ugrid_desc==0 || nedges<0 || outfname==0 ) return 0;

	info()<<"make wrapper\n"; info().flush();
	UGridWrapper<UGRID_DESC> wrapper( * ugrid_desc );

	// wrap mesh arrays
	info()<<"wrap tuple arrays\n"; info().flush();
	auto cellValues = wrapper.cellValues(); // cell centered values
	auto vertices = wrapper.vertices(); // vertex position and values
	auto cells = wrapper.cells(); // cell-to-vertex connectivity

	vtkIdType nverts = vertices.size();
	vtkIdType nCells = cellValues.size();
	info()<<nverts<<" vertices, "<<nCells <<" cells\n"; info().flush();

	// wraps direct (cell to vertex) and build reverse (vertex to cell) connectivity
	auto c2v = wrap_ugrid_smesh_c2v( cells, ONIKA_CONST(3) );
	info()<<"check connectivity : "<< c2v.checkConsistency(nverts)<<"\n"; info().flush();
	auto v2c = make_v2c( c2v , nverts );
	onika::debug::dbgassert( v2c.checkConsistency() );
	info()<<"mem="<<onika::container::memory_bytes(cells)<<"\n"; info().flush();

	// build edge length metric and shortest edge based cell ordering
	// when vertices have a complex type i.e. tuple of values, each of which can be a tuple,
	// edge_length_op takes the first element and compute the distance based on the first element only.
	// this is why it is important to place vertex coordinates first in the vertex definition
	auto edgeLength = edge_length_op(vertices);
	auto c2e = make_smesh_c2e(c2v);
	auto shortestEdgeOrder = cell_shortest_edge_less( c2e, edgeLength);
	auto orderedCells = ordered_cell_set(nCells, shortestEdgeOrder);

	std::ofstream ofile(outfname);
	onika::codec::AsciiStream out(ofile);
	info()<<"\n-------------- start compressing ----------------\n"; info().flush();
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
		info()<<"Cell #"<<minCell<<", edge "<<edge<<" length = "<<edgeLength(edge)<<"\n"; info().flush();

		onika::compress::smeshEdgeCollapseEncode(v2c, vertices, cellValues, edge, out);
	}
	onika::debug::dbgassert( v2c.checkConsistency() );

	return 1;
}

