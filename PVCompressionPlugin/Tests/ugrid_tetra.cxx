#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/debug/dbgassert.h"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>

using namespace onika::vtk;
using std::cout;

template<class Container>
auto wrap_vtk_tetrahedral_mesh_c2v( Container& cells )
ONIKA_AUTO_RET( onika::mesh::C2VWrapper< onika::mesh::c2v_traits< onika::mesh::smesh_c2v_basic_traits<Container,3,1> > >(cells) )

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
//	cout<<"OutputDataObject:\n";
//	data->PrintSelf(cout,vtkIndent(0));

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(data);
	if( ugrid == 0 ) return 1;

	if( ! allCellsAreTetras(ugrid) )
	{
		cout<<"Not a simplicial mesh\n";
		return 1;
	}
	vtkIdTypeArray* vtkcells = ugrid->GetCells()->GetData();
	auto cells = wrap_vtkarray( vtkcells );

	vtkIdType nverts = ugrid->GetNumberOfPoints();
	auto c2v = wrap_vtk_tetrahedral_mesh_c2v(cells);
	onika::debug::dbgassert( c2v.checkConsistency(nverts) );

	cout<<nverts<<" vertices, "<< c2v.getNumberOfCells()<<" cells, mem="<<onika::container::memory_bytes(cells)<<"\n";

	return 0;
}
