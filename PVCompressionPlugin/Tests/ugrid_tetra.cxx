#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/cell2edge.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/mesh/meshalgorithm.h"
#include "onika/container/sequence.h"
#include "onika/debug/dbgassert.h"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkFloatArray.h>
#include <vtkDataArrayTemplate.h>

template<class... T>
inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
{
	onika::tuple::print( out, t );
	return out;
}
template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ConstElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}
template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}

using namespace onika::vtk;
using std::cout;

// algorithms applied to mesh elements
using onika::mesh::edge_length_op;
using onika::mesh::CellShortestEdge;
using onika::container::SequenceIterator;
using onika::mesh::make_smesh_c2e_traits;
using onika::mesh::ordered_cell_set;

int main(int argc, char* argv[])
{
	if( argc<2 ) return 1;

	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(argv[1]);
	reader->Update();
	cout<<"Reader:\n";
	reader->PrintSelf(cout,vtkIndent(0));

	vtkDataObject* data = reader->GetOutputDataObject(0);
	if( data == 0 ) return 1;
	cout<<"OutputDataObject:\n";
	data->PrintSelf(cout,vtkIndent(0));

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(data);
	if( ugrid == 0 ) return 1;

	if( ! allCellsAreTetras(ugrid) )
	{
		cout<<"Not a simplicial mesh\n";
		return 1;
	}

	// wrap cell values
	auto cellValues = zip_array_wrappers(ugrid
			,DataSetAttribute<long,1,true>("GlobalElementId")
			,DataSetAttribute<long,1,true>("PedigreeElementId")
			,DataSetAttribute<int,1,true>("ObjectId")
			);

	// wrap vertex positions together with other values
//	ugrid->GetPointData()->GetArray("Temp")->PrintSelf(cout,vtkIndent(0));
	auto vertices = zip_array_wrappers(ugrid
			, UGridPoints<float,3>()
			, DataSetAttribute<long,1,false>("GlobalNodeId")
			, DataSetAttribute<long,1,false>("PedigreeNodeId")
			, DataSetAttribute<double,1,false>("Temp")
			, DataSetAttribute<double,3,false>("V")
			, DataSetAttribute<double,1,false>("Pres")
			, DataSetAttribute<double,1,false>("AsH3")
			, DataSetAttribute<double,1,false>("GaMe3")
			, DataSetAttribute<double,1,false>("CH4")
			, DataSetAttribute<double,1,false>("H2")
			);

	auto cells = UGridCells().wrap(ugrid);
	vtkIdType nverts = ugrid->GetNumberOfPoints();
	auto c2v = wrap_ugrid_smesh_c2v( cells, ONIKA_CONST(3) );
	auto v2c = make_v2c( c2v , nverts );
	onika::debug::dbgassert( v2c.checkConsistency() );
	int nCells = c2v.getNumberOfCells();
	cout<<nverts<<" vertices, "<<nCells <<" cells, mem="<<onika::container::memory_bytes(cells)<<"\n";

	// build edge length metric and shortest edge based cell ordering
	// when vertices have a complex type i.e. tuple of values, each of which can be a tuple,
	// edge_length_op takes the first element and compute the distance based on the first element only.
	// this is why it is important to place vertex coordinates first in the vertex definition
	auto edgeLength = edge_length_op(vertices);
	auto c2e_traits = make_smesh_c2e_traits(c2v);
	auto shortestEdgeOrder = cell_shortest_edge_less( cells, edgeLength, c2e_traits );
	auto orderedCells = ordered_cell_set(nCells, shortestEdgeOrder);
	auto edge = std::tuple<vtkIdType,vtkIdType>( 461, 467 );
	cout<<"edge "<<edge<<" length = "<<edgeLength(edge)<<"\n";

	cout<< "Cell with shortest edge is "<< ( * orderedCells.begin() )<< "\n";

	return 0;
}
