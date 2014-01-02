#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/debug/dbgassert.h"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <sys/time.h>

bool onikaEncodeMesh(vtkUnstructuredGrid* input, vtkUnstructuredGrid* output, int nedges, const std::string& outputFileName);

using namespace onika::vtk;
using std::cout;

template<class Container>
auto wrap_vtk_tetrahedral_mesh_c2v( Container& cells )
ONIKA_AUTO_RET( onika::mesh::C2VWrapper< onika::mesh::c2v_traits< onika::mesh::smesh_c2v_basic_traits<Container,3,1> > >(cells) )

int main(int argc, char* argv[])
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand( seed );

	std::string fname="/dev/stdin";
	std::string outfname="/dev/stdout";
	int nedges = 1;

	for(int i=1;i<argc;++i)
	{
		if( std::string(argv[i]) == "-c" )
		{
			++i;
			nedges = atoi(argv[i]);
		}
		else if( std::string(argv[i]) == "-i" )
		{
			++i; fname = argv[i];
		}
		else if( std::string(argv[i]) == "-o" )
		{
			++i; outfname = argv[i];
		}
	}

	std::cout<<"input = "<<fname<<"\n";
	std::cout<<"output = "<<outfname<<"\n";
	std::cout<<"edges = "<<nedges<<"\n";

	std::ifstream ifile(fname.c_str());
	if( !ifile )
	{
		std::cerr<<"Failed to open "<<fname<<"\n";
		return 1;
	}

	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(argv[1]);
	reader->Update();
	cout<<"Reader:\n";
	reader->PrintSelf(cout,vtkIndent(0));

	vtkDataObject* data = reader->GetOutputDataObject(0);
	if( data == 0 ) return 1;
	cout<<"\nOutputDataObject:\n";
	data->PrintSelf(cout,vtkIndent(0));

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(data);
	if( ugrid == 0 ) return 1;

	if( ! allCellsAreTetras(ugrid) )
	{
		cout<<"Not a simplicial mesh\n";
		return 1;
	}

	vtkUnstructuredGrid* output = vtkUnstructuredGrid::New();
	struct timeval T1, T2;
	gettimeofday(&T1,NULL);
	if( ! onikaEncodeMesh(ugrid,output,nedges,outfname) )
	{
		cout<<"Compression failed\n";
		return 1;
	}
	gettimeofday(&T2,NULL);
	cout<<"execution time : "<<(T2.tv_sec-T1.tv_sec)*1000.0 + (T2.tv_usec-T1.tv_usec)*0.001<<" ms\n";

	return 0;
}
