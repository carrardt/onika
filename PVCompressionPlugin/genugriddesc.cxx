#include "vtkUGridDescription.h"
#include <vtkXMLUnstructuredGridReader.h>
#include <fstream>

int main(int argc, char* argv[])
{
	if(argc<3) return 1;

	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(argv[1]);
	reader->Update();
	vtkUnstructuredGrid* data = vtkUnstructuredGrid::SafeDownCast(reader->GetOutputDataObject(0));
	if( data == 0 ) return 1;

	std::ofstream out(argv[2]);
	out << "#define UGRID_DESC ";
	onika::vtk::printUGridSignature(data,out);
	out << "\n#define UGRID_FILE \""<<argv[1]<<"\"\n";
	return 0;
}

