/*
 * genugriddesc.cxx
 *
 *  Created on: 2 janv. 2014
 *      Author: thierry
 */
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

#include <iostream>
#include <fstream>

void vtk_array_desc(vtkDataArray* array, std::ostream& out )
{
	out << array->GetDataTypeAsString();
	out << ',';
	out << array->GetNumberOfComponents();
}

void vtk_dsa_desc(vtkDataSetAttributes* dsa, std::ostream& out )
{
	out << "types<";
	int n = dsa->GetNumberOfArrays();
	for(int i=0;i<n;i++)
	{
		out << dsa->GetArray(i)->GetDataTypeAsString();
		if(i<(n-1)) out << ',';
	}
	out << ">,integers<";
	for(int i=0;i<n;i++)
	{
		out << dsa->GetArray(i)->GetNumberOfComponents();
		if(i<(n-1)) out << ',';
	}
	out << '>';
}

void vtk_ugrid_desc( vtkUnstructuredGrid* data, std::ostream& out )
{
	vtk_array_desc(data->GetPoints()->GetData(),out);
	out<<',';
	vtk_dsa_desc(data->GetCellData(),out);
	out<<',';
	vtk_dsa_desc(data->GetPointData(),out);
	out<<'\n';
}

int main(int argc, char* argv[])
{
	if(argc<3) return 1;

	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(argv[1]);
	reader->Update();
	vtkUnstructuredGrid* data = vtkUnstructuredGrid::SafeDownCast(reader->GetOutputDataObject(0));
	if( data == 0 ) return 1;

	std::ofstream out(argv[2]);
	out << "#undef UGRID_DESC\n#define UGRID_DESC ";
	vtk_ugrid_desc(data,out);
	out << "\n#undef UGRID_FILE\n#define UGRID_FILE \""<<argv[1]<<"\"\n";
	return 0;
}
