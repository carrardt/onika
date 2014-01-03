#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/cell2edge.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/mesh/meshalgorithm.h"
#include "onika/container/sequence.h"
#include "onika/debug/dbgassert.h"
#include "onika/language.h"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
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



// definition of unstructured grid to pass test on
#include "testdata.h"

int main(int argc, char* argv[])
{
	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(UGRID_FILE);
	reader->Update();
//	cout<<"Reader:\n";
//	reader->PrintSelf(cout,vtkIndent(0));

	vtkDataObject* data = reader->GetOutputDataObject(0);
	if( data == 0 ) return 1;
	cout<<"OutputDataObject:\n";
	data->PrintSelf(cout,vtkIndent(0));

	UGridWrapper<UGRID_DESC> wrapper( data );

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(data);
	if( ugrid == 0 ) return 1;

	if( ! allCellsAreTetras(ugrid) )
	{
		cout<<"Not a simplicial mesh\n";
		return 1;
	}

	// wrap mesh arrays
	auto cellValues = wrapper.cellValues(); // cell centered values
	auto vertices = wrapper.vertices(); // vertex position and values
	auto cells = wrapper.cells(); // cell-to-vertex connectivity

	vtkIdType nverts = ugrid->GetNumberOfPoints();
	vtkIdType nCells = ugrid->GetNumberOfCells();

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
	auto edge = std::tuple<vtkIdType,vtkIdType>( 461, 467 );
	cout<<"edge "<<edge<<" length = "<<edgeLength(edge)<<"\n";

	auto maxEdgeLen = edgeLength(edge);

	for(auto i : orderedCells)
	{
		int ne = c2e.getCellNumberOfEdges(i);
//		cout<<"Cell "<<i<<" has "<<ne<<" edges";
		for(int e=0;e<ne;e++)
		{
			auto edge = c2e.getCellEdge(i,e);
			auto len = edgeLength(edge);
			if( len > maxEdgeLen ) maxEdgeLen = len;
			//cout<<"\t"<<edge<<" : len="<<edgeLength(edge)<<"\n";
		}
	}
	cout<< "Cell with shortest edge is "<< ( * orderedCells.begin() )<< "\n";
	cout<< "longest edge length = "<<maxEdgeLen<< "\n";

	return 0;
}
